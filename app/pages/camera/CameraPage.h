#ifndef CAMERA_PAGE_H
#define CAMERA_PAGE_H

#include <QImage>
#include <QMutex>
#include <QString>
#include <QTimer>
#include <QWidget>

class QComboBox;
class QLabel;
class QPushButton;

class V4l2CameraService;

class CameraPage : public QWidget
{
    Q_OBJECT

public:
    explicit CameraPage(const QString &photoDirectory, QWidget *parent = nullptr);
    ~CameraPage();

signals:
    void backRequested();

private slots:
    void scanCameraDevices();
    void startOrStopCamera();
    void capturePhoto();
    void returnToHome();
    void onFrameReady(const QImage &image);
    void renderLatestFrame();
    void onCaptureStarted(const QString &devicePath,int width,int height,quint32 pixelFormat);
    void onCaptureStopped();
    void onCameraError(const QString &message);

private:
    QString fourccToString(quint32 format) const;

private:
    QString m_photoDirectory;

    QImage m_currentFrame;

    QMutex m_frameMutex; /* 保护摄像头线程和界面线程之间的最新帧 */
    QImage m_latestFrame; /* 只保存最新一帧，避免旧帧排队造成延时 */

    V4l2CameraService *m_cameraService;

    QTimer *m_renderTimer; /* 定时从最新帧缓存中刷新界面 */

    QLabel *m_titleLabel;
    QLabel *m_previewLabel;
    QLabel *m_photoLabel;
    QLabel *m_statusLabel;

    QComboBox *m_deviceComboBox;

    QPushButton *m_backButton;
    QPushButton *m_refreshButton;
    QPushButton *m_startButton;
    QPushButton *m_photoButton;
};

#endif