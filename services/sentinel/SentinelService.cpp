#include "SentinelService.h"

#include "MjpgStreamerService.h"
#include "smarthome_sr501.h"

#include <QTimer>

#include <cstring>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

SentinelService::SentinelService(QObject *parent)
    : QObject(parent),
      m_sensorFd(-1),
      m_monitoring(false),
      m_lastDetected(false),
      m_hasSensorState(false),
      m_sensorErrorReported(false),
      m_pollTimer(new QTimer(this)),
      m_stopDelayTimer(new QTimer(this)),
      m_streamer(new MjpgStreamerService(this))
{
    m_pollTimer->setInterval(300);
    m_stopDelayTimer->setSingleShot(true);
    m_stopDelayTimer->setInterval(10000);

    connect(m_pollTimer,
            &QTimer::timeout,
            this,
            &SentinelService::pollSr501);

    connect(m_stopDelayTimer,
            &QTimer::timeout,
            this,
            &SentinelService::stopStreamerAfterDelay);

    connect(m_streamer,
            &MjpgStreamerService::started,
            this,
            &SentinelService::streamStarted);

    connect(m_streamer,
            &MjpgStreamerService::stopped,
            this,
            [this]() {
        emit streamStopped();
    });

    connect(m_streamer,
            &MjpgStreamerService::errorOccurred,
            this,
            &SentinelService::errorOccurred);

    connect(m_streamer,
            &MjpgStreamerService::statusChanged,
            this,
            &SentinelService::statusChanged);
}

SentinelService::~SentinelService()
{
    stopMonitoring();
}

bool SentinelService::startMonitoring()
{
    if (m_monitoring)
        return true;

    m_sensorFd = ::open(
        "/dev/sr501",
        O_RDONLY | O_NONBLOCK);

    if (m_sensorFd < 0) {
        emit errorOccurred(
            QStringLiteral(
                "无法打开 /dev/sr501"));
        return false;
    }

    m_monitoring = true;
    m_lastDetected = false;
    m_hasSensorState = false;
    m_sensorErrorReported = false;

    m_pollTimer->start();

    emit statusChanged(
        QStringLiteral("SR501 哨兵监测已启动"));

    pollSr501();

    return true;
}

void SentinelService::stopMonitoring()
{
    if (!m_monitoring &&
        m_sensorFd < 0) {
        return;
    }

    m_pollTimer->stop();
    m_stopDelayTimer->stop();

    m_streamer->stop();

    if (m_sensorFd >= 0) {
        ::close(m_sensorFd);
        m_sensorFd = -1;
    }

    m_monitoring = false;

    emit statusChanged(
        QStringLiteral("SR501 哨兵监测已停止"));
}

bool SentinelService::isMonitoring() const
{
    return m_monitoring;
}

bool SentinelService::isStreaming() const
{
    return m_streamer->isRunning();
}

QString SentinelService::streamUrl() const
{
    return m_streamer->streamUrl();
}

void SentinelService::pollSr501()
{
    if (m_sensorFd < 0)
        return;

    struct sr501_status status;
    std::memset(&status, 0, sizeof(status));

    const int result =
        ::ioctl(m_sensorFd,
                SR501_IOC_GET_STATUS,
                &status);

    if (result < 0) {
        if (!m_sensorErrorReported) {
            m_sensorErrorReported = true;

            emit errorOccurred(
                QStringLiteral(
                    "SR501 状态读取失败"));
        }

        return;
    }

    m_sensorErrorReported = false;

    if (status.valid == 0)
        return;

    const bool detected =
        status.detected != 0;

    if (!m_hasSensorState ||
        detected != m_lastDetected) {
        m_hasSensorState = true;
        m_lastDetected = detected;

        emit sensorStateChanged(
            detected,
            status.sequence);
    }

    if (detected) {
        m_stopDelayTimer->stop();

        if (!m_streamer->isRunning()) {
            emit statusChanged(
                QStringLiteral(
                    "检测到人员，正在启动远程监控"));

            m_streamer->start();
        }
    } else {
        if (m_streamer->isRunning() &&
            !m_stopDelayTimer->isActive()) {
            emit statusChanged(
                QStringLiteral(
                    "人员离开，10 秒后关闭远程监控"));

            m_stopDelayTimer->start();
        }
    }
}

void SentinelService::stopStreamerAfterDelay()
{
    if (m_lastDetected)
        return;

    if (m_streamer->isRunning()) {
        emit statusChanged(
            QStringLiteral(
                "无人活动，关闭远程监控"));

        m_streamer->stop();
    }
}