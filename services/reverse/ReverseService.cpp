#include "ReverseService.h"

#include "smarthome_sr04.h"

#include <QTimer>

#include <cstring>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

ReverseService::ReverseService(QObject *parent)
    : QObject(parent),
      m_sr04Fd(-1),
      m_beepFd(-1),
      m_pollTimer(new QTimer(this)),
      m_monitoring(false),
      m_alarmActive(false),
      m_alarmThresholdMm(300)
{
    m_pollTimer->setInterval(200);

    connect(m_pollTimer,
            &QTimer::timeout,
            this,
            &ReverseService::pollDistance);
}

ReverseService::~ReverseService()
{
    stopMonitoring();
}

bool ReverseService::startMonitoring()
{
    if (m_monitoring)
        return true;

    m_sr04Fd = ::open(
        "/dev/sr04",
        O_RDONLY | O_NONBLOCK);

    if (m_sr04Fd < 0) {
        emit errorOccurred(
            QStringLiteral("无法打开 /dev/sr04"));
        return false;
    }

    m_beepFd = ::open(
        "/dev/beep_device",
        O_WRONLY);

    if (m_beepFd < 0) {
        emit statusChanged(
            QStringLiteral(
                "SR04 已连接，蜂鸣器打开失败"));
    }

    m_monitoring = true;
    m_alarmActive = false;

    setBuzzer(false);

    m_pollTimer->start();

    emit statusChanged(
        QStringLiteral("SR04 测距监控已启动"));

    pollDistance();

    return true;
}

void ReverseService::stopMonitoring()
{
    m_pollTimer->stop();

    setBuzzer(false);

    if (m_sr04Fd >= 0) {
        ::close(m_sr04Fd);
        m_sr04Fd = -1;
    }

    if (m_beepFd >= 0) {
        ::close(m_beepFd);
        m_beepFd = -1;
    }

    m_monitoring = false;
    m_alarmActive = false;

    emit alarmChanged(false);
}

bool ReverseService::isMonitoring() const
{
    return m_monitoring;
}

bool ReverseService::isAlarmActive() const
{
    return m_alarmActive;
}

void ReverseService::setAlarmThresholdMm(
    quint32 thresholdMm)
{
    m_alarmThresholdMm = thresholdMm;
}

quint32 ReverseService::alarmThresholdMm() const
{
    return m_alarmThresholdMm;
}

void ReverseService::pollDistance()
{
    if (m_sr04Fd < 0)
        return;

    struct sr04_measurement measurement;
    std::memset(&measurement, 0, sizeof(measurement));

    const int result =
        ::ioctl(m_sr04Fd,
                SR04_IOC_GET_DISTANCE,
                &measurement);

    if (result < 0) {
        emit distanceChanged(0, false);
        emit statusChanged(
            QStringLiteral("SR04 测距失败"));

        setBuzzer(false);
        return;
    }

    if (measurement.valid == 0) {
        emit distanceChanged(0, false);
        emit statusChanged(
            QStringLiteral("SR04 数据无效"));

        setBuzzer(false);
        return;
    }

    const quint32 distanceMm =
        measurement.distance_mm;

    emit distanceChanged(distanceMm, true);

    const bool shouldAlarm =
        distanceMm > 0 &&
        distanceMm <= m_alarmThresholdMm;

    setBuzzer(shouldAlarm);

    if (shouldAlarm) {
        emit statusChanged(
            QStringLiteral(
                "距离过近，蜂鸣器报警"));
    }
}

void ReverseService::setBuzzer(bool enabled)
{
    if (m_alarmActive == enabled)
        return;

    m_alarmActive = enabled;

    if (m_beepFd >= 0) {
        const char *value =
            enabled ? "1\n" : "0\n";

        ::write(
            m_beepFd,
            value,
            2);
    }

    emit alarmChanged(enabled);
}