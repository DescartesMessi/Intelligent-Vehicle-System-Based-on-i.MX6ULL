#include "MPlayerAudioPlayer.h"

#include <QFileInfo>
#include <QProcessEnvironment>
#include <QStringList>
#include <QtGlobal>

MPlayerAudioPlayer::MPlayerAudioPlayer(QObject *parent)
    : QObject(parent),
      m_playerPath(QStringLiteral("/usr/bin/mplayer")),
      m_paused(false),
      m_hasMedia(false),
      m_ignoreNextEof(false),
      m_quitting(false),
      m_volume(80)
{
    m_process.setProcessChannelMode(QProcess::MergedChannels);

    QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();
    environment.insert(QStringLiteral("LC_ALL"), QStringLiteral("C"));
    m_process.setProcessEnvironment(environment);

    connect(&m_process, &QProcess::started,
            this, &MPlayerAudioPlayer::onProcessStarted);

    connect(&m_process, &QProcess::readyRead,
            this, &MPlayerAudioPlayer::onProcessOutput);

    connect(&m_process,
            static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(
                &QProcess::finished),
            this, &MPlayerAudioPlayer::onProcessFinished);

    connect(&m_process, &QProcess::errorOccurred,
            this, &MPlayerAudioPlayer::onProcessError);

    connect(&m_queryTimer, &QTimer::timeout,
            this, &MPlayerAudioPlayer::queryPlaybackInformation);

    m_queryTimer.setInterval(500);
}

MPlayerAudioPlayer::~MPlayerAudioPlayer()
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

bool MPlayerAudioPlayer::play(const QString &filePath)
{
    QFileInfo fileInfo(filePath);

    if (!fileInfo.exists() || !fileInfo.isFile()) {
        emit errorOccurred(QStringLiteral("音乐文件不存在：%1").arg(filePath));
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

void MPlayerAudioPlayer::startMPlayer()
{
    QStringList arguments;

    arguments << QStringLiteral("-slave");
    arguments << QStringLiteral("-quiet");
    arguments << QStringLiteral("-idle");
    arguments << QStringLiteral("-noconsolecontrols");

    arguments << QStringLiteral("-ao");
    arguments << QStringLiteral("alsa:device=plughw=0.1");

    arguments << QStringLiteral("-srate");
    arguments << QStringLiteral("48000");

    arguments << QStringLiteral("-af");
    arguments << QStringLiteral("resample=48000:0:0,format=s16le");

    arguments << QStringLiteral("-channels");
    arguments << QStringLiteral("2");

    arguments << QStringLiteral("-vo");
    arguments << QStringLiteral("null");

    m_process.start(m_playerPath, arguments);
}

bool MPlayerAudioPlayer::loadFile(const QString &filePath)
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

void MPlayerAudioPlayer::onProcessStarted()
{
    sendCommand(QStringLiteral("volume %1 1").arg(m_volume));

    if (!m_pendingFile.isEmpty())
        loadFile(m_pendingFile);
}

void MPlayerAudioPlayer::pauseOrResume()
{
    if (!m_hasMedia || m_process.state() != QProcess::Running)
        return;

    sendCommand(QStringLiteral("pause"));

    m_paused = !m_paused;
    emit pausedChanged(m_paused);
}

void MPlayerAudioPlayer::stop()
{
    if (!m_hasMedia || m_process.state() != QProcess::Running)
        return;

    m_ignoreNextEof = true;
    sendCommand(QStringLiteral("stop"));

    m_queryTimer.stop();
    m_hasMedia = false;
    m_paused = false;
    m_currentFile.clear();

    emit playbackStopped();
    emit pausedChanged(false);
    emit positionChanged(0);
}

void MPlayerAudioPlayer::setVolume(int volume)
{
    m_volume = qBound(0, volume, 100);

    if (m_process.state() == QProcess::Running)
        sendCommand(QStringLiteral("volume %1 1").arg(m_volume));
}

void MPlayerAudioPlayer::seekAbsolute(int seconds)
{
    if (!m_hasMedia || m_process.state() != QProcess::Running)
        return;

    seconds = qMax(0, seconds);
    sendCommand(QStringLiteral("seek %1 2").arg(seconds));
}

bool MPlayerAudioPlayer::isRunning() const
{
    return m_hasMedia && m_process.state() == QProcess::Running;
}

bool MPlayerAudioPlayer::isPaused() const
{
    return m_paused;
}

void MPlayerAudioPlayer::queryPlaybackInformation()
{
    if (!m_hasMedia || m_process.state() != QProcess::Running)
        return;

    sendCommand(QStringLiteral("pausing_keep_force get_time_pos"));
    sendCommand(QStringLiteral("pausing_keep_force get_time_length"));
}

void MPlayerAudioPlayer::sendCommand(const QString &command)
{
    if (m_process.state() != QProcess::Running)
        return;

    QByteArray commandData = command.toLocal8Bit();
    commandData.append('\n');
    m_process.write(commandData);
}

QString MPlayerAudioPlayer::escapeFilePath(const QString &filePath) const
{
    QString escaped = filePath;
    escaped.replace(QStringLiteral("\\"), QStringLiteral("\\\\"));
    escaped.replace(QStringLiteral("\""), QStringLiteral("\\\""));
    return escaped;
}

void MPlayerAudioPlayer::onProcessOutput()
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
                m_queryTimer.stop();
                emit playbackFinished();
            }
        }
    }
}

void MPlayerAudioPlayer::onProcessFinished(int exitCode,QProcess::ExitStatus status)
{
    Q_UNUSED(exitCode)
    Q_UNUSED(status)

    m_queryTimer.stop();
    m_hasMedia = false;
    m_paused = false;
    m_currentFile.clear();
    m_pendingFile.clear();

    if (!m_quitting)
        emit playbackStopped();
}

void MPlayerAudioPlayer::onProcessError(QProcess::ProcessError error)
{
    Q_UNUSED(error)

    if (m_quitting)
        return;

    emit errorOccurred(QStringLiteral("MPlayer错误：%1").arg(m_process.errorString()));
}