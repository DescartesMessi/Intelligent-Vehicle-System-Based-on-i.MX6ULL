#ifndef SENTINEL_PAGE_H
#define SENTINEL_PAGE_H

#include <QtGlobal>
#include <QWidget>

class QLabel;
class QPushButton;
class QHideEvent;
class QShowEvent;

class SentinelService;

class SentinelPage : public QWidget
{
    Q_OBJECT

public:
    explicit SentinelPage(QWidget *parent = nullptr);
    ~SentinelPage();

signals:
    void backRequested();

protected:
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;

private slots:
    void startMonitoring();
    void stopMonitoring();
    void returnToHome();

    void onSensorStateChanged(bool detected,
                              quint32 sequence);
    void onStreamStarted(const QString &url);
    void onStreamStopped();
    void onStatusChanged(const QString &message);
    void onServiceError(const QString &message);

private:
    SentinelService *m_service;

    QLabel *m_titleLabel;
    QLabel *m_sensorLabel;
    QLabel *m_streamLabel;
    QLabel *m_urlLabel;
    QLabel *m_statusLabel;

    QPushButton *m_backButton;
    QPushButton *m_startButton;
    QPushButton *m_stopButton;
};

#endif