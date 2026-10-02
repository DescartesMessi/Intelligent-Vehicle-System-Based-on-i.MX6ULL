#include "V4l2CameraService.h"

#include "MetricsProbe.h"

#include <algorithm>
#include <cerrno>
#include <cstring>
#include <vector>

#include <QMutexLocker>
#include <QElapsedTimer>

#include <fcntl.h>
#include <linux/videodev2.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/select.h>
#include <unistd.h>

namespace {

/*
 * 两次 YUYV->RGB 转换之间的最小间隔。
 * 界面刷新定时器是 100ms，这里取 80ms 略快一点，
 * 保证界面每次刷新都能拿到较新的帧，同时把无人消费的帧直接丢弃。
 */
constexpr qint64 kMinConvertIntervalMs = 80;

struct MappedBuffer
{
    void *address;
    size_t length;

    MappedBuffer()
        : address(nullptr),
          length(0)
    {
    }
};

int xioctl(int fileDescriptor, unsigned long request, void *argument)
{
    int result;

    do {
        result = ::ioctl(fileDescriptor, request, argument);
    } while (result < 0 && errno == EINTR);

    return result;
}

int clampColor(int value)
{
    return std::max(0, std::min(255, value));
}

QString systemError(const QString &operation)
{
    return QStringLiteral("%1失败：%2")
        .arg(operation, QString::fromLocal8Bit(std::strerror(errno)));
}

}

V4l2CameraService::V4l2CameraService(QObject *parent)
    : QThread(parent),
      m_width(640),
      m_height(480),
      m_framesPerSecond(15),
      m_stopRequested(false)
{
}

V4l2CameraService::~V4l2CameraService()
{
    stopCapture();
}

bool V4l2CameraService::startCapture(const QString &devicePath,
                                     int width,
                                     int height,
                                     int framesPerSecond)
{
    if (devicePath.isEmpty()) {
        emit errorOccurred(QStringLiteral("摄像头设备路径为空"));
        return false;
    }

    if (isRunning())
        stopCapture();

    {
        QMutexLocker locker(&m_configMutex);
        m_devicePath = devicePath;
        m_width = width;
        m_height = height;
        m_framesPerSecond = framesPerSecond;
    }

    m_stopRequested.store(false);
    start();

    return true;
}

void V4l2CameraService::stopCapture()
{
    m_stopRequested.store(true);

    if (isRunning())
        wait(2000);
}

bool V4l2CameraService::isCapturing() const
{
    return isRunning() && !m_stopRequested.load();
}

void V4l2CameraService::run()
{
    QString devicePath;
    int requestedWidth;
    int requestedHeight;
    int requestedFramesPerSecond;

    {
        QMutexLocker locker(&m_configMutex);
        devicePath = m_devicePath;
        requestedWidth = m_width;
        requestedHeight = m_height;
        requestedFramesPerSecond = m_framesPerSecond;
    }

    int fileDescriptor = -1;
    bool streaming = false;
    QString errorMessage;
    std::vector<MappedBuffer> buffers;

    v4l2_format format;
    std::memset(&format, 0, sizeof(format));

    do {
        fileDescriptor = ::open(devicePath.toLocal8Bit().constData(),
                                O_RDWR | O_NONBLOCK);

        if (fileDescriptor < 0) {
            errorMessage = systemError(
                QStringLiteral("打开摄像头 %1").arg(devicePath));
            break;
        }

        v4l2_capability capability;
        std::memset(&capability, 0, sizeof(capability));

        if (xioctl(fileDescriptor, VIDIOC_QUERYCAP, &capability) < 0) {
            errorMessage = systemError(QStringLiteral("VIDIOC_QUERYCAP"));
            break;
        }

        quint32 capabilities = capability.capabilities;

        if (capabilities & V4L2_CAP_DEVICE_CAPS)
            capabilities = capability.device_caps;

        if (!(capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
            errorMessage = QStringLiteral("%1不是视频采集设备").arg(devicePath);
            break;
        }

        if (!(capabilities & V4L2_CAP_STREAMING)) {
            errorMessage = QStringLiteral("%1不支持V4L2流式采集").arg(devicePath);
            break;
        }

        format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        format.fmt.pix.width = requestedWidth;
        format.fmt.pix.height = requestedHeight;
        format.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
        format.fmt.pix.field = V4L2_FIELD_ANY;

        if (xioctl(fileDescriptor, VIDIOC_S_FMT, &format) < 0) {
            errorMessage = systemError(QStringLiteral("VIDIOC_S_FMT"));
            break;
        }

        const quint32 pixelFormat = format.fmt.pix.pixelformat;

        if (pixelFormat != V4L2_PIX_FMT_YUYV &&
            pixelFormat != V4L2_PIX_FMT_MJPEG &&
            pixelFormat != V4L2_PIX_FMT_JPEG) {
            errorMessage = QStringLiteral("不支持摄像头格式：%1")
                .arg(fourccToString(pixelFormat));
            break;
        }

        v4l2_streamparm streamParameters;
        std::memset(&streamParameters, 0, sizeof(streamParameters));
        streamParameters.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        streamParameters.parm.capture.timeperframe.numerator = 1;
        streamParameters.parm.capture.timeperframe.denominator =
            requestedFramesPerSecond;

        xioctl(fileDescriptor, VIDIOC_S_PARM, &streamParameters);

        v4l2_requestbuffers requestBuffers;
        std::memset(&requestBuffers, 0, sizeof(requestBuffers));
        requestBuffers.count = 4;
        requestBuffers.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        requestBuffers.memory = V4L2_MEMORY_MMAP;

        if (xioctl(fileDescriptor, VIDIOC_REQBUFS, &requestBuffers) < 0) {
            errorMessage = systemError(QStringLiteral("VIDIOC_REQBUFS"));
            break;
        }

        if (requestBuffers.count < 2) {
            errorMessage = QStringLiteral("摄像头分配的缓冲区数量不足");
            break;
        }

        for (quint32 index = 0; index < requestBuffers.count; ++index) {
            v4l2_buffer buffer;
            std::memset(&buffer, 0, sizeof(buffer));
            buffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            buffer.memory = V4L2_MEMORY_MMAP;
            buffer.index = index;

            if (xioctl(fileDescriptor, VIDIOC_QUERYBUF, &buffer) < 0) {
                errorMessage = systemError(QStringLiteral("VIDIOC_QUERYBUF"));
                break;
            }

            MappedBuffer mappedBuffer;
            mappedBuffer.length = buffer.length;
            mappedBuffer.address = ::mmap(nullptr,
                                          buffer.length,
                                          PROT_READ | PROT_WRITE,
                                          MAP_SHARED,
                                          fileDescriptor,
                                          buffer.m.offset);

            if (mappedBuffer.address == MAP_FAILED) {
                mappedBuffer.address = nullptr;
                errorMessage = systemError(QStringLiteral("mmap"));
                break;
            }

            buffers.push_back(mappedBuffer);
        }

        if (!errorMessage.isEmpty())
            break;

        for (quint32 index = 0; index < buffers.size(); ++index) {
            v4l2_buffer buffer;
            std::memset(&buffer, 0, sizeof(buffer));
            buffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            buffer.memory = V4L2_MEMORY_MMAP;
            buffer.index = index;

            if (xioctl(fileDescriptor, VIDIOC_QBUF, &buffer) < 0) {
                errorMessage = systemError(QStringLiteral("VIDIOC_QBUF"));
                break;
            }
        }

        if (!errorMessage.isEmpty())
            break;

        v4l2_buf_type bufferType = V4L2_BUF_TYPE_VIDEO_CAPTURE;

        if (xioctl(fileDescriptor, VIDIOC_STREAMON, &bufferType) < 0) {
            errorMessage = systemError(QStringLiteral("VIDIOC_STREAMON"));
            break;
        }

        streaming = true;

        const int actualWidth = format.fmt.pix.width;
        const int actualHeight = format.fmt.pix.height;
        const int bytesPerLine = format.fmt.pix.bytesperline;

        emit captureStarted(devicePath,
                            actualWidth,
                            actualHeight,
                            pixelFormat);

        /*
         * 帧率节流：
         * 界面按固定节奏取帧（CameraPage 的刷新定时器为 100ms），
         * 而 UVC 摄像头通常以 30fps 交付。若对每一帧都做 YUYV->RGB
         * 转换，约 2/3 的转换结果根本不会被显示，纯属浪费 CPU——
         * 本板 CPU 只有 396MHz，这部分开销很可观。
         * 这里按略快于界面刷新的节奏（80ms）限制转换次数，
         * 未做转换的帧直接重新入队，不产生任何额外开销。
         */
        QElapsedTimer frameThrottle;
        frameThrottle.start();
        qint64 lastConvertMs = -kMinConvertIntervalMs;

        /* 指标采集：驱动交付帧数 / 实际转换（解码）帧数 */
        quint64 framesDequeued = 0;
        quint64 framesConverted = 0;
        quint64 framesLastWindow = 0;
        QElapsedTimer statsTimer;
        statsTimer.start();

        while (!m_stopRequested.load()) {
            fd_set fileDescriptors;
            FD_ZERO(&fileDescriptors);
            FD_SET(fileDescriptor, &fileDescriptors);

            timeval timeout;
            timeout.tv_sec = 1;
            timeout.tv_usec = 0;

            const int selectResult = ::select(fileDescriptor + 1,
                                              &fileDescriptors,
                                              nullptr,
                                              nullptr,
                                              &timeout);

            if (selectResult < 0) {
                if (errno == EINTR)
                    continue;

                errorMessage = systemError(QStringLiteral("select"));
                break;
            }

            if (selectResult == 0)
                continue;

            v4l2_buffer buffer;
            std::memset(&buffer, 0, sizeof(buffer));
            buffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            buffer.memory = V4L2_MEMORY_MMAP;

            if (xioctl(fileDescriptor, VIDIOC_DQBUF, &buffer) < 0) {
                if (errno == EAGAIN)
                    continue;

                errorMessage = systemError(QStringLiteral("VIDIOC_DQBUF"));
                break;
            }

            ++framesDequeued;

            if (buffer.index >= buffers.size()) {
                errorMessage = QStringLiteral("摄像头返回了无效缓冲区索引");
                break;
            }

            const unsigned char *frameData =
                static_cast<const unsigned char *>(
                    buffers[buffer.index].address);

            const qint64 nowMs = frameThrottle.elapsed();

            if (nowMs - lastConvertMs >= kMinConvertIntervalMs) {
                lastConvertMs = nowMs;

                QImage image;

                if (pixelFormat == V4L2_PIX_FMT_YUYV) {
                    image = convertYuyvToRgb(frameData,
                                             buffer.bytesused,
                                             actualWidth,
                                             actualHeight,
                                             bytesPerLine);
                } else {
                    image = QImage::fromData(frameData,
                                             buffer.bytesused,
                                             "JPG");
                }

                if (!image.isNull()) {
                    emit frameReady(image);
                    ++framesConverted;
                } else if (pixelFormat != V4L2_PIX_FMT_YUYV) {
                    errorMessage =
                        QStringLiteral("MJPEG解码失败，请部署Qt JPEG图片插件");
                }
            }

            if (xioctl(fileDescriptor, VIDIOC_QBUF, &buffer) < 0) {
                errorMessage = systemError(QStringLiteral("VIDIOC_QBUF"));
                break;
            }

            if (statsTimer.elapsed() >= 2000) {
                const double seconds = statsTimer.restart() / 1000.0;
                const quint64 windowFrames = framesDequeued - framesLastWindow;

                framesLastWindow = framesDequeued;

                vsmetrics::log(
                    QStringLiteral("CAMERA dq=%1/s conv=%2/s 累计(dq=%3 conv=%4) "
                                   "size=%5x%6 fourcc=%7 bytes=%8")
                        .arg(windowFrames / seconds, 0, 'f', 1)
                        .arg(framesConverted ? framesConverted / seconds : 0.0, 0, 'f', 1)
                        .arg(framesDequeued)
                        .arg(framesConverted)
                        .arg(actualWidth)
                        .arg(actualHeight)
                        .arg(fourccToString(pixelFormat))
                        .arg(buffer.bytesused));
            }

            if (!errorMessage.isEmpty())
                break;
        }
    } while (false);

    if (streaming) {
        v4l2_buf_type bufferType = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        xioctl(fileDescriptor, VIDIOC_STREAMOFF, &bufferType);
    }

    for (const MappedBuffer &buffer : buffers) {
        if (buffer.address)
            ::munmap(buffer.address, buffer.length);
    }

    if (fileDescriptor >= 0)
        ::close(fileDescriptor);

    if (!errorMessage.isEmpty())
        emit errorOccurred(errorMessage);

    emit captureStopped();
}

QImage V4l2CameraService::convertYuyvToRgb(const unsigned char *data,
                                           unsigned int bytesUsed,
                                           int width,
                                           int height,
                                           int bytesPerLine) const
{
    if (!data || width <= 0 || height <= 0)
        return QImage();

    int sourceStride = bytesPerLine;

    if (sourceStride < width * 2)
        sourceStride = width * 2;

    if (bytesUsed < static_cast<unsigned int>(sourceStride * height))
        return QImage();

    QImage image(width, height, QImage::Format_RGB888);

    if (image.isNull())
        return QImage();

    for (int y = 0; y < height; ++y) {
        const unsigned char *source = data + y * sourceStride;
        unsigned char *destination = image.scanLine(y);

        for (int x = 0; x + 1 < width; x += 2) {
            const int sourceIndex = x * 2;

            const int y0 = source[sourceIndex];
            const int u = source[sourceIndex + 1];
            const int y1 = source[sourceIndex + 2];
            const int v = source[sourceIndex + 3];

            const int d = u - 128;
            const int e = v - 128;

            const int c0 = y0 - 16;
            const int red0 = clampColor((298 * c0 + 409 * e + 128) >> 8);
            const int green0 =
                clampColor((298 * c0 - 100 * d - 208 * e + 128) >> 8);
            const int blue0 = clampColor((298 * c0 + 516 * d + 128) >> 8);

            const int c1 = y1 - 16;
            const int red1 = clampColor((298 * c1 + 409 * e + 128) >> 8);
            const int green1 =
                clampColor((298 * c1 - 100 * d - 208 * e + 128) >> 8);
            const int blue1 = clampColor((298 * c1 + 516 * d + 128) >> 8);

            const int destinationIndex = x * 3;

            destination[destinationIndex] = red0;
            destination[destinationIndex + 1] = green0;
            destination[destinationIndex + 2] = blue0;

            destination[destinationIndex + 3] = red1;
            destination[destinationIndex + 4] = green1;
            destination[destinationIndex + 5] = blue1;
        }
    }

    return image;
}

QString V4l2CameraService::fourccToString(quint32 format) const
{
    char text[5];
    text[0] = format & 0xff;
    text[1] = (format >> 8) & 0xff;
    text[2] = (format >> 16) & 0xff;
    text[3] = (format >> 24) & 0xff;
    text[4] = '\0';

    return QString::fromLatin1(text);
}
