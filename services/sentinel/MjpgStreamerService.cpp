#include "MjpgStreamerService.h"

#include <QProcess>
#include <QStringList>

MjpgStreamerService::MjpgStreamerService(QObject *parent)
    : QObject(parent),
      m_process(new QProcess(this)),
      m_program(QStringLiteral(
          "/usr/local/bin/mjpg_streamer")),
      m_inputPlugin(QStringLiteral(
          "/usr/local/lib/mjpg-streamer/input_uvc.so")),
      m_outputPlugin(QStringLiteral(
          "/usr/local/lib/mjpg-streamer/output_http.so")),
      m_webRoot(QStringLiteral("/www")),
      m_streamUrl(QStringLiteral(
          "http://192.168.31.50:8080/?action=stream")),
      m_port(8080)
{
    connect(m_process,
            &QProcess::started,
            this,
            &MjpgStreamerService::onProcessStarted);

    connect(m_process,
            QOverload<int, QProcess::ExitStatus>::of(
                &QProcess::finished),
            this,
            &MjpgStreamerService::onProcessFinished);

    connect(m_process,
            QOverload<QProcess::ProcessError>::of(
                &QProcess::errorOccurred),
            this,
            &MjpgStreamerService::onProcessError);
}

MjpgStreamerService::~MjpgStreamerService()
{
    stop();
}

bool MjpgStreamerService::start()
{
    if (isRunning())
        return true;

    if (m_process->state() != QProcess::NotRunning)
        return false;

    const QString inputArgument =
        QStringLiteral(
            "%1 -d /dev/video1 -r 640x480 -f 25 -q 70")
            .arg(m_inputPlugin);

    const QString outputArgument =
        QStringLiteral(
            "%1 -w %2 -p %3")
            .arg(m_outputPlugin)
            .arg(m_webRoot)
            .arg(m_port);

    QStringList arguments;
    arguments << QStringLiteral("-i")
              << inputArgument
              << QStringLiteral("-o")
              << outputArgument;

    m_process->setProgram(m_program);
    m_process->setArguments(arguments);

    emit statusChanged(
        QStringLiteral("正在启动 MJPG-streamer"));

    m_process->start();

    return true;
}

void MjpgStreamerService::stop()
{
    if (m_process->state() == QProcess::NotRunning)
        return;

    emit statusChanged(
        QStringLiteral("正在停止 MJPG-streamer"));

    m_process->terminate();

    if (!m_process->waitForFinished(1500)) {
        m_process->kill();
        m_process->waitForFinished(1000);
    }
}

bool MjpgStreamerService::isRunning() const
{
    return m_process->state() == QProcess::Running ||
           m_process->state() == QProcess::Starting;
}

QString MjpgStreamerService::streamUrl() const
{
    return m_streamUrl;
}

void MjpgStreamerService::onProcessStarted()
{
    emit statusChanged(
        QStringLiteral("MJPG-streamer 已启动"));

    emit started(m_streamUrl);
}

void MjpgStreamerService::onProcessFinished(
    int exitCode,
    QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitStatus);

    emit stopped();

    if (exitCode != 0) {
        emit errorOccurred(
            QStringLiteral(
                "MJPG-streamer 异常退出，退出码：%1")
                .arg(exitCode));
    } else {
        emit statusChanged(
            QStringLiteral("MJPG-streamer 已停止"));
    }
}

void MjpgStreamerService::onProcessError(
    QProcess::ProcessError error)
{
    Q_UNUSED(error);

    emit errorOccurred(
        QStringLiteral("MJPG-streamer 启动失败：%1")
            .arg(m_process->errorString()));
}