#ifndef V4L2_CAMERA_SERVICE_H
#define V4L2_CAMERA_SERVICE_H

#include <atomic>

#include <QImage>
#include <QMutex>
#include <QString>
#include <QThread>

class V4l2CameraService : public QThread
{
    Q_OBJECT

public:
    explicit V4l2CameraService(QObject *parent = nullptr);
    ~V4l2CameraService();

    bool startCapture(const QString &devicePath,
                    int width = 640,
                    int height = 480,
                    int framesPerSecond = 15);

    void stopCapture();
    bool isCapturing() const;

signals:
    void frameReady(const QImage &image);
    void captureStarted(const QString &devicePath,
                        int width,
                        int height,
                        quint32 pixelFormat);
    void captureStopped();
    void errorOccurred(const QString &message);

protected:
    void run() override;

private:
    QImage convertYuyvToRgb(const unsigned char *data,
                            unsigned int bytesUsed,
                            int width,
                            int height,
                            int bytesPerLine) const;

    QString fourccToString(quint32 format) const;

private:
    mutable QMutex m_configMutex;
    QString m_devicePath;
    int m_width;
    int m_height;
    int m_framesPerSecond;
    std::atomic_bool m_stopRequested;
};

#endif