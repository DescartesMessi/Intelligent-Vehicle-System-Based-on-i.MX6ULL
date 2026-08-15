#ifndef MJPG_STREAMER_SERVICE_H
#define MJPG_STREAMER_SERVICE_H

#include <QObject>
#include <QProcess>
#include <QString>


class MjpgStreamerService : public QObject
{
    Q_OBJECT

public:
    explicit MjpgStreamerService(QObject *parent = nullptr);
    ~MjpgStreamerService();

    bool start();
    void stop();

    bool isRunning() const;
    QString streamUrl() const;

signals:
    void started(const QString &url);
    void stopped();
    void errorOccurred(const QString &message);
    void statusChanged(const QString &message);

private slots:
    void onProcessStarted();
    void onProcessFinished(int exitCode,
                           QProcess::ExitStatus exitStatus);
    void onProcessError(QProcess::ProcessError error);

private:
    QProcess *m_process;

    QString m_program;
    QString m_inputPlugin;
    QString m_outputPlugin;
    QString m_webRoot;
    QString m_streamUrl;

    int m_port;
};

#endif