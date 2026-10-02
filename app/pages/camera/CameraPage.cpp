#include "CameraPage.h"

#include "MetricsProbe.h"
#include "V4l2CameraService.h"

#include <QAtomicInteger>
#include <QComboBox>
#include <QDateTime>
#include <QDir>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QLabel>
#include <QMutexLocker>
#include <QPixmap>
#include <QPushButton>
#include <QVBoxLayout>

namespace {

/*
 * 指标采集用计数器：
 * onFrameReady 在采集线程执行，renderLatestFrame 在 GUI 线程执行，
 * 因此用原子量累加，避免为了测量再引入一把锁。
 */
QAtomicInteger<quint64> gFramesReceived(0);
QAtomicInteger<quint64> gFramesRendered(0);

} /* namespace */

CameraPage::CameraPage(const QString &photoDirectory,
                       QWidget *parent)
    : QWidget(parent),
      m_photoDirectory(photoDirectory),
      m_currentFrame(),
      m_frameMutex(),
      m_latestFrame(),
      m_cameraService(new V4l2CameraService(this)),
      m_renderTimer(new QTimer(this)), /* 创建唯一的界面刷新定时器 */
      m_titleLabel(new QLabel(QStringLiteral("摄像头与拍照"), this)),
      m_previewLabel(new QLabel(QStringLiteral("等待摄像头画面"), this)),
      m_photoLabel(new QLabel(QStringLiteral("照片预览"), this)),
      m_statusLabel(new QLabel(QStringLiteral("就绪"), this)),
      m_deviceComboBox(new QComboBox(this)),
      m_backButton(new QPushButton(QStringLiteral("返回"), this)),
      m_refreshButton(new QPushButton(QStringLiteral("刷新设备"), this)),
      m_startButton(new QPushButton(QStringLiteral("开始"), this)),
      m_photoButton(new QPushButton(QStringLiteral("拍照"), this))
{
    setObjectName(QStringLiteral("cameraPage"));
    setFixedSize(800, 480);

    /*摄像头目标帧率为 10 fps 时，100 ms 刷新一次可以避免 GUI 线程重复渲染。 */
    m_renderTimer->setInterval(100);
    m_titleLabel->setAlignment(Qt::AlignCenter);
    m_statusLabel->setAlignment(Qt::AlignCenter);
    m_previewLabel->setMinimumSize(600, 380);
    m_previewLabel->setAlignment(Qt::AlignCenter);
    m_previewLabel->setScaledContents(false);
    m_photoLabel->setFixedSize(150, 112);
    m_photoLabel->setAlignment(Qt::AlignCenter);
    m_photoLabel->setScaledContents(false);
    m_deviceComboBox->setMinimumHeight(40);
    m_startButton->setMinimumHeight(44);
    m_photoButton->setMinimumHeight(44);
    m_refreshButton->setMinimumHeight(40);
    m_backButton->setFixedSize(76, 40);
    m_photoButton->setEnabled(false);

    QHBoxLayout *headerLayout = new QHBoxLayout;
    headerLayout->setContentsMargins(0, 0, 0, 0);
    headerLayout->addWidget(m_backButton);
    headerLayout->addStretch();
    headerLayout->addWidget(m_titleLabel);
    headerLayout->addStretch();
    headerLayout->addWidget(m_refreshButton);

    QVBoxLayout *controlLayout = new QVBoxLayout;
    controlLayout->setContentsMargins(0, 0, 0, 0);
    controlLayout->setSpacing(10);
    controlLayout->addWidget(m_photoLabel);
    controlLayout->addWidget(m_deviceComboBox);
    controlLayout->addWidget(m_startButton);
    controlLayout->addWidget(m_photoButton);
    controlLayout->addWidget(m_statusLabel);
    controlLayout->addStretch();

    QWidget *controlPanel = new QWidget(this);
    controlPanel->setObjectName(QStringLiteral("cameraControlPanel"));
    controlPanel->setFixedWidth(165);
    controlPanel->setLayout(controlLayout);

    QHBoxLayout *contentLayout = new QHBoxLayout;
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(10);
    contentLayout->addWidget(m_previewLabel, 1);
    contentLayout->addWidget(controlPanel);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 8, 10, 8);
    mainLayout->setSpacing(8);
    mainLayout->addLayout(headerLayout);
    mainLayout->addLayout(contentLayout, 1);

    connect(m_backButton,&QPushButton::clicked,this, &CameraPage::returnToHome);

    connect(m_refreshButton,&QPushButton::clicked,this,&CameraPage::scanCameraDevices);

    connect(m_startButton,&QPushButton::clicked,this, &CameraPage::startOrStopCamera);

    connect(m_photoButton,&QPushButton::clicked,this,&CameraPage::capturePhoto);

    /*摄像头线程只把最新帧写入缓存，不把每一帧排入 GUI 事件队列。*/
    connect(m_cameraService,&V4l2CameraService::frameReady,this,&CameraPage::onFrameReady,Qt::DirectConnection);

    /*界面线程按照固定周期渲染最新帧。*/
    connect(m_renderTimer, &QTimer::timeout, this, &CameraPage::renderLatestFrame);
    connect(m_cameraService, &V4l2CameraService::captureStarted,this,&CameraPage::onCaptureStarted);
    connect(m_cameraService,&V4l2CameraService::captureStopped,this,&CameraPage::onCaptureStopped);
    connect(m_cameraService,&V4l2CameraService::errorOccurred, this, &CameraPage::onCameraError);
    scanCameraDevices();
}

CameraPage::~CameraPage()
{
    m_renderTimer->stop();
    if (m_cameraService->isCapturing())
        m_cameraService->stopCapture();
}

void CameraPage::scanCameraDevices()
{
    if (m_cameraService->isCapturing())
        m_cameraService->stopCapture();
    m_renderTimer->stop();
    {
        QMutexLocker locker(&m_frameMutex);

        m_latestFrame = QImage();
        m_currentFrame = QImage();
    }

    m_previewLabel->clear();
    m_previewLabel->setText(QStringLiteral("等待摄像头画面"));

    m_deviceComboBox->clear();

    const QString devicePath(QStringLiteral("/dev/video1"));
    QFileInfo deviceInfo(devicePath);

    if (deviceInfo.exists()) {
        m_deviceComboBox->addItem(
            QStringLiteral("USB摄像头 video1"),
            devicePath);
    }

    const bool hasCamera =
        m_deviceComboBox->count() > 0;

    m_startButton->setEnabled(hasCamera);
    m_photoButton->setEnabled(false);

    if (hasCamera) {
        m_statusLabel->setText(
            QStringLiteral("已检测到摄像头：/dev/video1"));
    } else {
        m_statusLabel->setText(
            QStringLiteral("未检测到摄像头：/dev/video1"));

        m_previewLabel->setText(
            QStringLiteral("摄像头不可用"));
    }
}

void CameraPage::startOrStopCamera()
{
    if (m_cameraService->isCapturing()) {
        m_startButton->setEnabled(false);

        m_renderTimer->stop();

        m_cameraService->stopCapture();
        return;
    }

    const QString devicePath =
        m_deviceComboBox->currentData().toString();

    if (devicePath.isEmpty()) {
        m_statusLabel->setText(
            QStringLiteral("请选择摄像头设备"));
        return;
    }

    m_startButton->setEnabled(false);

    m_statusLabel->setText(
        QStringLiteral("正在打开 %1").arg(devicePath));

    /*使用 320×240、10 fps 降低 YUYV 转换和 GUI 缩放开销。摄像头最终实际格式由 V4L2 返回值决定。
     */
    const bool started =
        m_cameraService->startCapture(devicePath,320,240,15);

    if (!started) {
        m_startButton->setEnabled(true);
        m_statusLabel->setText(QStringLiteral("摄像头启动请求失败"));
    }
}

void CameraPage::capturePhoto()
{
    if (m_currentFrame.isNull()) {
        m_statusLabel->setText( QStringLiteral("当前没有可保存的画面"));
        return;
    }

    QDir photoDir(m_photoDirectory);

    if (!photoDir.exists() &&
        !photoDir.mkpath(QStringLiteral("."))) {
        m_statusLabel->setText(QStringLiteral("无法创建照片目录：%1").arg(m_photoDirectory));
        return;
    }

    const QString timestamp =
        QDateTime::currentDateTime().toString(QStringLiteral("yyyyMMdd_HHmmss_zzz"));

    const QString filePath =
        photoDir.filePath(QStringLiteral("photo_%1.bmp").arg(timestamp));

    if (!m_currentFrame.save(filePath, "BMP")) {
        m_statusLabel->setText(QStringLiteral("照片保存失败：%1").arg(filePath));
        return;
    }

    const QPixmap thumbnail =
        QPixmap::fromImage(m_currentFrame).scaled(m_photoLabel->size(),Qt::KeepAspectRatio,Qt::FastTransformation);

    m_photoLabel->setPixmap(thumbnail);

    m_statusLabel->setText(QStringLiteral("照片已保存：%1").arg(filePath));
}

void CameraPage::returnToHome()
{
    m_renderTimer->stop();
    m_cameraService->stopCapture();

    emit backRequested();
}

void CameraPage::onFrameReady(const QImage &image)
{
    if (image.isNull())
        return;

    /* 指标采集：采集线程交付给界面的帧数（未及时上屏的会被下一帧覆盖） */
    gFramesReceived.fetchAndAddOrdered(1);

    /*该函数运行在摄像头采集线程。这里只替换最新帧，不能直接操作 Qt 界面控件。*/
    QMutexLocker locker(&m_frameMutex);
    m_latestFrame = image;
}

void CameraPage::renderLatestFrame()
{
    QImage image;
    {
        /*将最新帧快速取出后立即释放锁，避免摄像头线程长时间等待。 */
        QMutexLocker locker(&m_frameMutex);
        if (m_latestFrame.isNull())
            return;
        image = m_latestFrame;
    }

    /* 当前帧用于拍照保存。 */
    m_currentFrame = image;
    /*
     * 先缩放 QImage 再转 QPixmap。
     * 原写法 QPixmap::fromImage(image).scaled(...) 会先把整幅 640x480
     * 的 RGB888 转成屏幕格式的 QPixmap，再缩放；而预览控件比原图小，
     * 相当于对用不到的那部分像素白做了一次格式转换。
     */
    const QPixmap preview =
        QPixmap::fromImage(image.scaled(m_previewLabel->size(),
                                        Qt::KeepAspectRatio,
                                        Qt::FastTransformation));
    m_previewLabel->setPixmap(preview);
    m_photoButton->setEnabled(true);

    /* 指标采集：实际上屏帧率，以及"采集了但没上屏"的丢帧比例 */
    gFramesRendered.fetchAndAddOrdered(1);

    static QElapsedTimer renderStats;
    static bool renderStatsStarted = false;
    static quint64 receivedLast = 0;
    static quint64 renderedLast = 0;

    if (!renderStatsStarted) {
        renderStats.start();
        renderStatsStarted = true;
    }

    if (renderStats.elapsed() >= 2000) {
        const double seconds = renderStats.restart() / 1000.0;
        const quint64 received = gFramesReceived.loadAcquire();
        const quint64 rendered = gFramesRendered.loadAcquire();
        const quint64 receivedWindow = received - receivedLast;
        const quint64 renderedWindow = rendered - renderedLast;

        receivedLast = received;
        renderedLast = rendered;

        vsmetrics::log(
            QStringLiteral("RENDER 采集=%1/s 上屏=%2/s 丢帧=%3% 分辨率=%4x%5")
                .arg(receivedWindow / seconds, 0, 'f', 1)
                .arg(renderedWindow / seconds, 0, 'f', 1)
                .arg(receivedWindow
                         ? 100.0 * (receivedWindow - renderedWindow) / receivedWindow
                         : 0.0,
                     0, 'f', 1)
                .arg(image.width())
                .arg(image.height()));
    }
}

void CameraPage::onCaptureStarted(const QString &devicePath, int width,int height,quint32 pixelFormat)
{
    m_startButton->setEnabled(true);
    m_startButton->setText(QStringLiteral("关闭"));
    m_deviceComboBox->setEnabled(false);
    m_refreshButton->setEnabled(false);

    /*只有摄像头真正 STREAMON 成功后才刷新界面。*/
    m_renderTimer->start();
    m_statusLabel->setText(
        QStringLiteral("%1  %2×%3  %4").arg(devicePath).arg(width).arg(height).arg(fourccToString(pixelFormat)));
}

void CameraPage::onCaptureStopped()
{
    m_renderTimer->stop();
    m_startButton->setEnabled(
    m_deviceComboBox->count() > 0);
    m_startButton->setText(QStringLiteral("开始"));
    m_deviceComboBox->setEnabled(true);
    m_refreshButton->setEnabled(true);
    m_photoButton->setEnabled(false);
}
void CameraPage::onCameraError(const QString &message)
{
    m_renderTimer->stop();
    m_statusLabel->setText(message);
    m_startButton->setEnabled(
    m_deviceComboBox->count() > 0);
    m_startButton->setText(QStringLiteral("开始"));
    m_deviceComboBox->setEnabled(true);
    m_refreshButton->setEnabled(true);
    m_photoButton->setEnabled(false);
}

QString CameraPage::fourccToString(quint32 format) const
{
    char text[5];

    text[0] = format & 0xff;
    text[1] = (format >> 8) & 0xff;
    text[2] = (format >> 16) & 0xff;
    text[3] = (format >> 24) & 0xff;
    text[4] = '\0';

    return QString::fromLatin1(text);
}
