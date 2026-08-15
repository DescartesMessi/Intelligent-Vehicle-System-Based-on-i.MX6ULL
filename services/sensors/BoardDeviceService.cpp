#include "BoardDeviceService.h"

#include "liteon_ap3216c.h"
#include "smarthome_dht11.h"
#include "smarthome_icm20608.h"
#include "smarthome_sr04.h"
#include "smarthome_sr501.h"

#include <QElapsedTimer>
#include <QMetaType>

#include <cerrno>
#include <cstring>

#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

namespace
{

QString systemError()
{
    return QString::fromLocal8Bit(std::strerror(errno));
}

void closeDevice(int &fd)
{
    if (fd >= 0) {
        ::close(fd);
        fd = -1;
    }
}

}

BoardDeviceService::BoardDeviceService(QObject *parent)
    : QThread(parent),
      m_stopRequested(false),
      m_buzzerEnabled(false)
{
    qRegisterMetaType<BoardAp3216cData>("BoardAp3216cData");
    qRegisterMetaType<BoardDht11Data>("BoardDht11Data");
    qRegisterMetaType<BoardIcm20608Data>("BoardIcm20608Data");
    qRegisterMetaType<BoardSr04Data>("BoardSr04Data");
    qRegisterMetaType<BoardSr501Data>("BoardSr501Data");
}

BoardDeviceService::~BoardDeviceService()
{
    stopMonitoring();
    setBuzzer(false);
}

void BoardDeviceService::startMonitoring()
{
    if (isRunning())
        return;

    m_stopRequested.store(false);
    start();
}

void BoardDeviceService::stopMonitoring()
{
    m_stopRequested.store(true);

    if (isRunning())
        wait(3000);
}

bool BoardDeviceService::setBuzzer(bool enabled)
{
    const int fd = ::open("/dev/beep_device", O_WRONLY);

    if (fd < 0) {
        emit deviceError(
            QStringLiteral("蜂鸣器"),
            QStringLiteral("打开 /dev/beep_device 失败：%1")
                .arg(systemError()));
        return false;
    }

    const QByteArray command =
        enabled ? QByteArray("1\n") : QByteArray("0\n");

    const ssize_t written =
        ::write(fd, command.constData(), command.size());

    const int savedErrno = errno;
    ::close(fd);

    if (written != static_cast<ssize_t>(command.size())) {
        errno = savedErrno;

        emit deviceError(
            QStringLiteral("蜂鸣器"),
            QStringLiteral("写入蜂鸣器失败：%1")
                .arg(systemError()));
        return false;
    }

    m_buzzerEnabled.store(enabled);
    emit buzzerUpdated(enabled);

    return true;
}

bool BoardDeviceService::buzzerEnabled() const
{
    return m_buzzerEnabled.load();
}

void BoardDeviceService::run()
{
    int ap3216cFd =
        ::open("/dev/ap3216c", O_RDWR);

    int dht11Fd =
        ::open("/dev/dht11", O_RDONLY);

    int icm20608Fd =
        ::open("/dev/icm20608", O_RDWR);

    int sr04Fd =
        ::open("/dev/sr04", O_RDWR);

    int sr501Fd =
        ::open("/dev/sr501", O_RDONLY | O_NONBLOCK);

    if (ap3216cFd < 0) {
        emit deviceError(
            QStringLiteral("AP3216C"),
            QStringLiteral("打开设备失败：%1").arg(systemError()));
    }

    if (dht11Fd < 0) {
        emit deviceError(
            QStringLiteral("DHT11"),
            QStringLiteral("打开设备失败：%1").arg(systemError()));
    }

    if (icm20608Fd < 0) {
        emit deviceError(
            QStringLiteral("ICM20608"),
            QStringLiteral("打开设备失败：%1").arg(systemError()));
    }

    if (sr04Fd < 0) {
        emit deviceError(
            QStringLiteral("SR04"),
            QStringLiteral("打开设备失败：%1").arg(systemError()));
    }

    if (sr501Fd < 0) {
        emit deviceError(
            QStringLiteral("SR501"),
            QStringLiteral("打开设备失败：%1").arg(systemError()));
    }

    QElapsedTimer timer;
    timer.start();

    qint64 lastFastRead = -500;
    qint64 lastDhtRead = -2500;

    while (!m_stopRequested.load()) {
        const qint64 now = timer.elapsed();

        /*
         * AP3216C、ICM20608、SR04、SR501
         * 每 500 ms 读取一次。
         */
        if (now - lastFastRead >= 500) {
            lastFastRead = now;

            if (ap3216cFd >= 0) {
                ap3216c_sample raw;
                std::memset(&raw, 0, sizeof(raw));

                if (::ioctl(ap3216cFd,
                            AP3216C_GET_SAMPLE,
                            &raw) == 0) {
                    BoardAp3216cData data;

                    data.ir = raw.ir;
                    data.als = raw.als;
                    data.ps = raw.ps;
                    data.irValid = raw.ir_valid != 0;
                    data.psValid = raw.ps_valid != 0;
                    data.objectNear = raw.object_near != 0;

                    emit ap3216cUpdated(data);
                }
            }

            if (icm20608Fd >= 0) {
                icm20608_sample raw;
                std::memset(&raw, 0, sizeof(raw));

                if (::ioctl(icm20608Fd,
                            ICM20608_IOC_GET_SAMPLE,
                            &raw) == 0) {
                    BoardIcm20608Data data;

                    data.accelX =
                        static_cast<double>(raw.accel_x) / 16384.0;
                    data.accelY =
                        static_cast<double>(raw.accel_y) / 16384.0;
                    data.accelZ =
                        static_cast<double>(raw.accel_z) / 16384.0;

                    data.gyroX =
                        static_cast<double>(raw.gyro_x) / 131.0;
                    data.gyroY =
                        static_cast<double>(raw.gyro_y) / 131.0;
                    data.gyroZ =
                        static_cast<double>(raw.gyro_z) / 131.0;

                    data.temperature =
                        static_cast<double>(raw.temperature) / 326.8
                        + 25.0;

                    data.whoAmI = raw.who_am_i;

                    emit icm20608Updated(data);
                }
            }

            if (sr04Fd >= 0) {
                sr04_measurement raw;
                std::memset(&raw, 0, sizeof(raw));

                if (::ioctl(sr04Fd,
                            SR04_IOC_GET_DISTANCE,
                            &raw) == 0) {
                    BoardSr04Data data;

                    data.distanceMm = raw.distance_mm;
                    data.distanceCm =
                        static_cast<double>(raw.distance_mm) / 10.0;
                    data.pulseUs = raw.pulse_us;
                    data.valid = raw.valid != 0;
                    data.nearAlarm =
                        data.valid && data.distanceMm <= 80;

                    emit sr04Updated(data);
                }
            }

            if (sr501Fd >= 0) {
                sr501_status raw;
                std::memset(&raw, 0, sizeof(raw));

                if (::ioctl(sr501Fd,
                            SR501_IOC_GET_STATUS,
                            &raw) == 0) {
                    BoardSr501Data data;

                    data.detected = raw.detected != 0;
                    data.valid = raw.valid != 0;
                    data.sequence = raw.sequence;

                    emit sr501Updated(data);
                }
            }
        }

        /*
         * DHT11 最小采样周期较长，这里使用 2500 ms。
         */
        if (now - lastDhtRead >= 2500) {
            lastDhtRead = now;

            if (dht11Fd >= 0) {
                dht11_measurement raw;
                std::memset(&raw, 0, sizeof(raw));

                if (::ioctl(dht11Fd,
                            DHT11_IOC_GET_MEASUREMENT,
                            &raw) == 0 &&
                    raw.valid != 0) {
                    BoardDht11Data data;

                    data.temperatureInteger =
                        raw.temperature_integer;
                    data.temperatureDecimal =
                        raw.temperature_decimal;
                    data.humidityInteger =
                        raw.humidity_integer;
                    data.humidityDecimal =
                        raw.humidity_decimal;

                    emit dht11Updated(data);
                }
            }
        }

        msleep(50);
    }

    closeDevice(ap3216cFd);
    closeDevice(dht11Fd);
    closeDevice(icm20608Fd);
    closeDevice(sr04Fd);
    closeDevice(sr501Fd);
}