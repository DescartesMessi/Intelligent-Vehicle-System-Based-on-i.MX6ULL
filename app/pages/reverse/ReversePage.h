#ifndef REVERSE_PAGE_H
#define REVERSE_PAGE_H

#include <QImage>
#include <QMutex>
#include <QWidget>

class QLabel;
class QPushButton;
class QTimer;

class V4l2CameraService;
class ReverseService;

class ReversePage : public QWidget
{
    Q_OBJECT

public:
    explicit ReversePage(const QString &photoDirectory,
                         QWidget *parent = nullptr);
    ~ReversePage();

signals:
    void backRequested();

protected:
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;

private slots:
    void startOrStop();
    void takePhoto();
    void returnToHome();

    void renderLatestFrame();

    void onFrameReady(const QImage &image);
    void onCaptureStarted(const QString &devicePath,
                          int width,
                          int height,
                          quint32 pixelFormat);
    void onCaptureStopped();
    void onCameraError(const QString &message);

    void onDistanceChanged(quint32 distanceMm, bool valid);
    void onAlarmChanged(bool active);
    void onReverseStatusChanged(const QString &message);
    void onReverseError(const QString &message);

private:
    void startAll();
    void stopAll();
    void updateRunningState(bool running);
    QString fourccToString(quint32 format) const;

private:
    QString m_photoDirectory;

    V4l2CameraService *m_cameraService;
    ReverseService *m_reverseService;

    QLabel *m_titleLabel;
    QLabel *m_previewLabel;
    QLabel *m_distanceLabel;
    QLabel *m_alarmLabel;
    QLabel *m_statusLabel;
    QLabel *m_photoLabel;

    QPushButton *m_backButton;
    QPushButton *m_startButton;
    QPushButton *m_photoButton;

    QTimer *m_renderTimer;

    QMutex m_frameMutex;
    QImage m_latestFrame;

    bool m_running;
};

#endif