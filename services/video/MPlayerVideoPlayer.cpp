#include "MPlayerVideoPlayer.h"

#include <QFileInfo>
#include <QProcessEnvironment>
#include <QStringList>
#include <QtGlobal>

MPlayerVideoPlayer::MPlayerVideoPlayer(QObject *parent)
    : QObject(parent),
      m_playerPath(QStringLiteral("/usr/bin/mplayer")),
      m_paused(false),
      m_hasMedia(false),
      m_fullscreen(false),
      m_ignoreNextEof(false),
      m_quitting(false),
      m_volume(70)
{
    m_process.setProcessChannelMode(QProcess::MergedChannels);

    QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();
    environment.insert(QStringLiteral("LC_ALL"), QStringLiteral("C"));
    m_process.setProcessEnvironment(environment);

    connect(&m_process, &QProcess::started,
            this, &MPlayerVideoPlayer::onProcessStarted);

    connect(&m_process, &QProcess::readyRead,
            this, &MPlayerVideoPlayer::onProcessOutput);

    connect(&m_process,
            static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(
                &QProcess::finished),
            this, &MPlayerVideoPlayer::onProcessFinished);

    connect(&m_process, &QProcess::errorOccurred,
            this, &MPlayerVideoPlayer::onProcessError);

    connect(&m_queryTimer, &QTimer::timeout,
            this, &MPlayerVideoPlayer::queryPlaybackInformation);

    m_queryTimer.setInterval(500);
}

MPlayerVideoPlayer::~MPlayerVideoPlayer()
{
    m_quitting = true;
    m_queryTimer.stop();

    if (m_process.state() == QProcess::NotRunning)
        return;

    sendCommand(QStringLiteral("quit"));

    if (!m_process.waitForFinished(1000)) {
        m_process.kill();
        m_process.waitForFinished(1000);
    }
}

bool MPlayerVideoPlayer::play(const QString &filePath)
{
    QFileInfo fileInfo(filePath);

    if (!fileInfo.exists() || !fileInfo.isFile()) {
        emit errorOccurred(QStringLiteral("视频文件不存在：%1").arg(filePath));
        return false;
    }

    m_pendingFile = fileInfo.absoluteFilePath();

    if (m_process.state() == QProcess::NotRunning) {
        startMPlayer();
        return true;
    }

    if (m_process.state() == QProcess::Starting)
        return true;

    return loadFile(m_pendingFile);
}

void MPlayerVideoPlayer::startMPlayer()
{
    QStringList arguments;

    arguments << QStringLiteral("-slave");
    arguments << QStringLiteral("-quiet");
    arguments << QStringLiteral("-idle");
    arguments << QStringLiteral("-noconsolecontrols");

    arguments << QStringLiteral("-vo");
    arguments << QStringLiteral("fbdev2:/dev/fb0");

    arguments << QStringLiteral("-geometry");
    arguments << QStringLiteral("0:55");

    arguments << QStringLiteral("-zoom");
    arguments << QStringLiteral("-x");
    arguments << QStringLiteral("520");
    arguments << QStringLiteral("-y");
    arguments << QStringLiteral("330");

    arguments << QStringLiteral("-ao");
    arguments << QStringLiteral("alsa:device=plughw=0.1");

    arguments << QStringLiteral("-srate");
    arguments << QStringLiteral("48000");

    arguments << QStringLiteral("-af");
    arguments << QStringLiteral("resample=48000:0:0,format=s16le");

    arguments << QStringLiteral("-channels");
    arguments << QStringLiteral("2");

    arguments << QStringLiteral("-framedrop");

    arguments << QStringLiteral("-hardframedrop");
    arguments << QStringLiteral("-autosync");
    arguments << QStringLiteral("30");
    arguments << QStringLiteral("-cache");
    arguments << QStringLiteral("8192");
    arguments << QStringLiteral("-cache-min");
    arguments << QStringLiteral("10");
    arguments << QStringLiteral("-lavdopts");
    arguments << QStringLiteral("fast:threads=2");


    m_process.start(m_playerPath, arguments);
}

bool MPlayerVideoPlayer::loadFile(const QString &filePath)
{
    if (m_process.state() != QProcess::Running)
        return false;

    if (m_hasMedia)
        m_ignoreNextEof = true;

    const QString escapedPath = escapeFilePath(filePath);
    sendCommand(QStringLiteral("loadfile \"%1\" 0").arg(escapedPath));

    m_currentFile = filePath;
    m_pendingFile.clear();
    m_paused = false;
    m_hasMedia = true;

    sendCommand(QStringLiteral("volume %1 1").arg(m_volume));

    m_queryTimer.start();

    emit playbackStarted(m_currentFile);
    emit pausedChanged(false);

    return true;
}

void MPlayerVideoPlayer::onProcessStarted()
{
    sendCommand(QStringLiteral("volume %1 1").arg(m_volume));

    if (!m_pendingFile.isEmpty())
        loadFile(m_pendingFile);
}

void MPlayerVideoPlayer::pauseOrResume()
{
    if (!m_hasMedia || m_process.state() != QProcess::Running)
        return;

    sendCommand(QStringLiteral("pause"));

    m_paused = !m_paused;
    emit pausedChanged(m_paused);
}

void MPlayerVideoPlayer::stop()
{
    if (!m_hasMedia || m_process.state() != QProcess::Running)
        return;

    m_ignoreNextEof = true;
    sendCommand(QStringLiteral("stop"));

    m_queryTimer.stop();
    m_hasMedia = false;
    m_paused = false;
    m_fullscreen = false;
    m_currentFile.clear();

    emit playbackStopped();
    emit pausedChanged(false);
    emit fullscreenChanged(false);
    emit positionChanged(0);
}

void MPlayerVideoPlayer::setVolume(int volume)
{
    m_volume = qBound(0, volume, 100);

    if (m_process.state() == QProcess::Running)
        sendCommand(QStringLiteral("volume %1 1").arg(m_volume));
}

void MPlayerVideoPlayer::seekAbsolute(int seconds)
{
    if (!m_hasMedia || m_process.state() != QProcess::Running)
        return;

    seconds = qMax(0, seconds);
    sendCommand(QStringLiteral("seek %1 2").arg(seconds));
}

void MPlayerVideoPlayer::toggleFullscreen()
{
    if (!m_hasMedia || m_process.state() != QProcess::Running)
        return;

    sendCommand(QStringLiteral("vo_fullscreen"));
    m_fullscreen = !m_fullscreen;
    emit fullscreenChanged(m_fullscreen);
}

bool MPlayerVideoPlayer::isPlaying() const
{
    return m_hasMedia && m_process.state() == QProcess::Running;
}

bool MPlayerVideoPlayer::isPaused() const
{
    return m_paused;
}

bool MPlayerVideoPlayer::isFullscreen() const
{
    return m_fullscreen;
}

void MPlayerVideoPlayer::queryPlaybackInformation()
{
    if (!m_hasMedia || m_process.state() != QProcess::Running)
        return;

    sendCommand(QStringLiteral("pausing_keep_force get_time_pos"));
    sendCommand(QStringLiteral("pausing_keep_force get_time_length"));
}

void MPlayerVideoPlayer::sendCommand(const QString &command)
{
    if (m_process.state() != QProcess::Running)
        return;

    QByteArray commandData = command.toUtf8();
    commandData.append('\n');
    m_process.write(commandData);
}

QString MPlayerVideoPlayer::escapeFilePath(const QString &filePath) const
{
    QString escaped = filePath;
    escaped.replace(QStringLiteral("\\"), QStringLiteral("\\\\"));
    escaped.replace(QStringLiteral("\""), QStringLiteral("\\\""));
    return escaped;
}

void MPlayerVideoPlayer::onProcessOutput()
{
    m_outputBuffer.append(m_process.readAll());

    while (true) {
        const int newlinePosition = m_outputBuffer.indexOf('\n');

        if (newlinePosition < 0)
            break;

        const QByteArray lineData = m_outputBuffer.left(newlinePosition);
        m_outputBuffer.remove(0, newlinePosition + 1);

        const QString line = QString::fromLocal8Bit(lineData).trimmed();

        if (line.startsWith(QStringLiteral("ANS_TIME_POSITION="))) {
            bool valid = false;
            const double seconds = line.section('=', 1).toDouble(&valid);

            if (valid)
                emit positionChanged(static_cast<qint64>(seconds * 1000.0));
        } else if (line.startsWith(QStringLiteral("ANS_LENGTH="))) {
            bool valid = false;
            const double seconds = line.section('=', 1).toDouble(&valid);

            if (valid)
                emit durationChanged(static_cast<qint64>(seconds * 1000.0));
        } else if (line.contains(QStringLiteral("EOF code:"))) {
            if (m_ignoreNextEof) {
                m_ignoreNextEof = false;
                continue;
            }

            if (m_hasMedia) {
                m_hasMedia = false;
                m_paused = false;
                m_fullscreen = false;
                m_queryTimer.stop();

                emit fullscreenChanged(false);
                emit playbackFinished();
            }
        }
    }
}

void MPlayerVideoPlayer::onProcessFinished(int exitCode,
                                           QProcess::ExitStatus status)
{
    Q_UNUSED(exitCode)
    Q_UNUSED(status)

    m_queryTimer.stop();
    m_hasMedia = false;
    m_paused = false;
    m_fullscreen = false;
    m_currentFile.clear();
    m_pendingFile.clear();

    if (!m_quitting)
        emit playbackStopped();
}

void MPlayerVideoPlayer::onProcessError(QProcess::ProcessError error)
{
    Q_UNUSED(error)

    if (m_quitting)
        return;

    emit errorOccurred(QStringLiteral("MPlayer错误：%1")
                       .arg(m_process.errorString()));
}