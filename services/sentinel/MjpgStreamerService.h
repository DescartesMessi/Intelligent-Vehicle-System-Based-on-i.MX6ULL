#ifndef MJPG_STREAMER_SERVICE_H
#define MJPG_STREAMER_SERVICE_H

#include <QObject>
#include <QProcess>
#include <QString>

class QTimer;

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
    void onPortPollTimeout();
    void onProcessFinished(int exitCode,
                           QProcess::ExitStatus exitStatus);
    void onProcessError(QProcess::ProcessError error);

private:
    /*
     * 清理上一次异常退出留下的 mjpg_streamer：
     * 它占着 8080 端口会让本次启动直接失败（用户看到"已启动"后又立刻"已停止"）。
     * 返回 true 表示端口已经空闲。
     */
    bool ensurePortAvailable(QString *failReason);

    QProcess *m_process;
    QTimer *m_portPollTimer;

    int m_portWaitElapsed;

    QString m_program;
    QString m_inputPlugin;
    QString m_outputPlugin;
    QString m_webRoot;
    QString m_streamUrl;

    int m_port;
};

#endif
