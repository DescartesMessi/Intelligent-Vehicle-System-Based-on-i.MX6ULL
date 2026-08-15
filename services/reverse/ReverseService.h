#ifndef REVERSE_SERVICE_H
#define REVERSE_SERVICE_H

#include <QString>
#include <QObject>

class QTimer;

class ReverseService : public QObject
{
    Q_OBJECT

public:
    explicit ReverseService(QObject *parent = nullptr);
    ~ReverseService();

    bool startMonitoring();
    void stopMonitoring();

    bool isMonitoring() const;
    bool isAlarmActive() const;

    void setAlarmThresholdMm(quint32 thresholdMm);
    quint32 alarmThresholdMm() const;

signals:
    void distanceChanged(quint32 distanceMm,
                         bool valid);

    void alarmChanged(bool active);

    void statusChanged(const QString &message);
    void errorOccurred(const QString &message);

private slots:
    void pollDistance();

private:
    void setBuzzer(bool enabled);

private:
    int m_sr04Fd;
    int m_beepFd;

    QTimer *m_pollTimer;

    bool m_monitoring;
    bool m_alarmActive;

    quint32 m_alarmThresholdMm;
};

#endif