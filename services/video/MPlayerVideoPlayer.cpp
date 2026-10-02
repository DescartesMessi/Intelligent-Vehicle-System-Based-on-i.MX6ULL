#include "MPlayerVideoPlayer.h"

#include "MetricsProbe.h"

#include <QFileInfo>
#include <QProcessEnvironment>
#include <QStringList>
#include <QStandardPaths>
#include <QtGlobal>

/*
 * 找 mplayer 可执行文件。
 *
 * 不能写死 /usr/bin/mplayer：本项目的 NFS 开发 rootfs 里 mplayer 装在 /usr/bin，
 * 而 eMMC 运行 rootfs（厂商基础镜像）里在 /bin，写死路径在 eMMC 上会直接
 * execve 失败，界面提示 "MPlayer错误：execve: No such file or directory"。
 */
static QString resolveMPlayerPath()
{
    const QStringList candidates = {
        QStringLiteral("/usr/bin/mplayer"),
        QStringLiteral("/bin/mplayer"),
        QStringLiteral("/usr/local/bin/mplayer"),
    };

    for (const QString &path : candidates) {
        const QFileInfo info(path);

        if (info.exists() && info.isExecutable())
            return path;
    }

    const QString fromPath =
        QStandardPaths::findExecutable(QStringLiteral("mplayer"));

    return fromPath.isEmpty() ? QStringLiteral("/usr/bin/mplayer") : fromPath;
}

MPlayerVideoPlayer::MPlayerVideoPlayer(QObject *parent)
    : QObject(parent),m_playerPath(resolveMPlayerPath()),m_paused(false),m_hasMedia(false),m_fullscreen(false),
    m_ignoreNextEof(false),m_quitting(false),m_volume(70)
{
    vsmetrics::log(QStringLiteral("视频播放器 mplayer 路径: %1")
                       .arg(m_playerPath));

    /* 合并标准输出和错误输出，统一处理 MPlayer 状态信息 */
    m_process.setProcessChannelMode(QProcess::MergedChannels);

    QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();
    environment.insert(QStringLiteral("LC_ALL"), QStringLiteral("C"));
    m_process.setProcessEnvironment(environment);

    connect(&m_process, &QProcess::started, this, &MPlayerVideoPlayer::onProcessStarted);
    connect(&m_process, &QProcess::readyRead, this, &MPlayerVideoPlayer::onProcessOutput);
    connect(&m_process, static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, &MPlayerVideoPlayer::onProcessFinished);
    connect(&m_process, &QProcess::errorOccurred, this, &MPlayerVideoPlayer::onProcessError);
    connect(&m_queryTimer, &QTimer::timeout, this, &MPlayerVideoPlayer::queryPlaybackInformation);
    /* 降低播放状态查询频率，减少主线程和 MPlayer 通信开销 */
    m_queryTimer.setInterval(1000);
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

    /* 启用从标准输入接收控制命令的从模式 */
    arguments << QStringLiteral("-slave");

    /* 减少 MPlayer 控制台输出，避免大量日志影响系统性能 */
    arguments << QStringLiteral("-quiet");

    /* 保持 MPlayer 进程常驻，便于连续切换视频 */
    arguments << QStringLiteral("-idle");

    /* 禁止控制台按键接管播放控制 */
    arguments << QStringLiteral("-noconsolecontrols");

    /* 使用 Linux framebuffer 输出视频 */
    arguments << QStringLiteral("-vo");
    arguments << QStringLiteral("fbdev2:/dev/fb0");

    /* 设置视频在 LCD 上的显示位置 */
    arguments << QStringLiteral("-geometry");
    arguments << QStringLiteral("0:55");

    /* 将视频缩放到当前视频显示区域 */
    arguments << QStringLiteral("-zoom");
    arguments << QStringLiteral("-x");
    arguments << QStringLiteral("520");
    arguments << QStringLiteral("-y");
    arguments << QStringLiteral("330");

    /* 使用指定 ALSA 声卡播放音频 */
    arguments << QStringLiteral("-ao");
    arguments << QStringLiteral("alsa:device=plughw=0.1");

    /* 统一音频采样率，减少音频设备重新协商 */
    arguments << QStringLiteral("-srate");
    arguments << QStringLiteral("48000");

    /* 固定双声道输出 */
    arguments << QStringLiteral("-channels");
    arguments << QStringLiteral("2");

    /* 允许 MPlayer 丢弃来不及解码的帧 */
    arguments << QStringLiteral("-framedrop");

    /* 强制丢弃过期视频帧，优先降低播放延迟 */
    arguments << QStringLiteral("-hardframedrop");

    /* 增大文件读取缓存，缓解 NFS 或慢速存储造成的读取抖动 */
    arguments << QStringLiteral("-cache");
    arguments << QStringLiteral("8192");

    /* 缓存达到 10% 后开始播放，减少播放过程中等待数据 */
    arguments << QStringLiteral("-cache-min");
    arguments << QStringLiteral("5");

    /* 调整音视频同步策略，降低播放过程中逐渐累积的延迟 */
    arguments << QStringLiteral("-autosync");
    arguments << QStringLiteral("30");

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

    /* 重新开始播放状态查询 */
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

    /* 只查询当前位置，降低控制命令频率 */
    sendCommand(QStringLiteral("pausing_keep_force get_time_pos"));

    /* 定期查询总时长，保证切换视频后界面时间信息正常 */
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

void MPlayerVideoPlayer::onProcessFinished(int exitCode, QProcess::ExitStatus status)
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

    emit errorOccurred(QStringLiteral("MPlayer错误：%1").arg(m_process.errorString()));
}
