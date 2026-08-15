#ifndef SENTINEL_SERVICE_H
#define SENTINEL_SERVICE_H

#include <QObject>

class QTimer;
class MjpgStreamerService;

class SentinelService : public QObject
{
    Q_OBJECT

public:
    explicit SentinelService(QObject *parent = nullptr);
    ~SentinelService();

    bool startMonitoring();
    void stopMonitoring();

    bool isMonitoring() const;
    bool isStreaming() const;

    QString streamUrl() const;

signals:
    void sensorStateChanged(bool detected,
                            quint32 sequence);

    void streamStarted(const QString &url);
    void streamStopped();

    void statusChanged(const QString &message);
    void errorOccurred(const QString &message);

private slots:
    void pollSr501();
    void stopStreamerAfterDelay();

private:
    int m_sensorFd;

    bool m_monitoring;
    bool m_lastDetected;
    bool m_hasSensorState;
    bool m_sensorErrorReported;

    QTimer *m_pollTimer;
    QTimer *m_stopDelayTimer;

    MjpgStreamerService *m_streamer;
};

#endif