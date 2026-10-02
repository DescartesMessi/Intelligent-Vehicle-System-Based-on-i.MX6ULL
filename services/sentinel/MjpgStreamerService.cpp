#include "MjpgStreamerService.h"

#include "MetricsProbe.h"

#include <QDir>
#include <QFile>
#include <QProcess>
#include <QStringList>
#include <QThread>
#include <QTimer>

#include <csignal>
#include <cerrno>
#include <cstring>

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

namespace {

/*
 * 127.0.0.1:<port> 上是否已经有进程在监听（不依赖 QtNetwork）
 *
 * 必须用非阻塞 connect + 200ms 超时：如果监听方 backlog 满或者长时间
 * 不 accept，阻塞式 connect 会把调用线程卡住很久——实测踩过这个坑：
 * 残留的 mjpg_streamer 占着 8080 时，GUI 线程被阻塞式 connect 卡了
 * 约 6 分钟（看起来像"应用死了"），直到残留进程被杀掉才恢复。
 */
bool isPortListening(int port)
{
    const int fd = ::socket(AF_INET, SOCK_STREAM, 0);

    if (fd < 0)
        return false;

    const int flags = ::fcntl(fd, F_GETFL, 0);

    if (flags >= 0)
        ::fcntl(fd, F_SETFL, flags | O_NONBLOCK);

    struct sockaddr_in address;
    std::memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(static_cast<quint16>(port));
    address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    bool listening = false;
    const int rc = ::connect(fd,
                             reinterpret_cast<struct sockaddr *>(&address),
                             sizeof(address));

    if (rc == 0) {
        listening = true;
    } else if (errno == EINPROGRESS) {
        struct pollfd descriptor;

        descriptor.fd = fd;
        descriptor.events = POLLOUT;
        descriptor.revents = 0;

        const int pollResult = ::poll(&descriptor, 1, 200);

        if (pollResult > 0) {
            int socketError = 0;
            socklen_t length = sizeof(socketError);

            if (::getsockopt(fd, SOL_SOCKET, SO_ERROR,
                             &socketError, &length) == 0)
                listening = (socketError == 0);
        } else if (pollResult == 0) {
            /* 超时：当作"端口被占但不响应"，交给清理流程处理 */
            listening = true;
        }
    }

    ::close(fd);
    return listening;
}

/* 扫描 /proc，找出命令行为 mjpg_streamer 的进程号 */
QList<int> findStreamerProcesses()
{
    QList<int> pids;
    const QDir procDir(QStringLiteral("/proc"));
    const QStringList entries =
        procDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

    for (const QString &entry : entries) {
        bool ok = false;
        const int pid = entry.toInt(&ok);

        if (!ok || pid <= 1)
            continue;

        QFile cmdline(
            QStringLiteral("/proc/%1/cmdline").arg(pid));

        if (!cmdline.open(QIODevice::ReadOnly))
            continue;

        /* /proc/<pid>/cmdline 用 '\0' 分隔参数 */
        const QByteArray data = cmdline.readAll();

        if (data.contains("mjpg_streamer"))
            pids.append(pid);
    }

    return pids;
}

} /* namespace */

MjpgStreamerService::MjpgStreamerService(QObject *parent)
    : QObject(parent),
      m_process(new QProcess(this)),
      m_portPollTimer(new QTimer(this)),
      m_portWaitElapsed(0),
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

    /*
     * 进程起来不等于服务可用：端口被占或摄像头被占用时它会立刻退出。
     * 这里用定时器轮询端口，最多等 3 秒再判定"启动成功"，
     * 避免界面先显示"已启动"又马上变成"已停止"。
     */
    m_portPollTimer->setInterval(200);

    connect(m_portPollTimer,
            &QTimer::timeout,
            this,
            &MjpgStreamerService::onPortPollTimeout);
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

    QString failReason;

    if (!ensurePortAvailable(&failReason)) {
        vsmetrics::log(QStringLiteral("SENTINEL %1").arg(failReason));
        emit errorOccurred(failReason);
        return false;
    }

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

bool MjpgStreamerService::ensurePortAvailable(QString *failReason)
{
    if (!isPortListening(m_port))
        return true;

    /* 端口被占：把残留的 mjpg_streamer 收掉（可能来自上一次异常退出） */
    QList<int> stalePids = findStreamerProcesses();

    for (int pid : stalePids) {
        vsmetrics::log(
            QStringLiteral("SENTINEL 清理残留 mjpg_streamer pid=%1")
                .arg(pid));
        ::kill(pid, SIGTERM);
    }

    for (int i = 0; i < 5 && isPortListening(m_port); ++i)
        QThread::msleep(100);

    if (isPortListening(m_port)) {
        for (int pid : findStreamerProcesses())
            ::kill(pid, SIGKILL);

        for (int i = 0; i < 10 && isPortListening(m_port); ++i)
            QThread::msleep(100);
    }

    if (isPortListening(m_port)) {
        if (failReason) {
            *failReason = QStringLiteral(
                "端口 %1 已被其它进程占用，远程监控无法启动")
                .arg(m_port);
        }
        return false;
    }

    vsmetrics::log(
        QStringLiteral("SENTINEL 端口 %1 已释放，继续启动")
            .arg(m_port));
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
    /* 先只记后台，等端口真的 LISTEN 了再对外宣布"已启动" */
    vsmetrics::log(
        QStringLiteral("SENTINEL MJPG-streamer 进程已拉起，等待端口 %1 就绪")
            .arg(m_port));

    m_portWaitElapsed = 0;
    m_portPollTimer->start();
}

void MjpgStreamerService::onPortPollTimeout()
{
    if (m_process->state() != QProcess::Running) {
        m_portPollTimer->stop();
        return; /* 进程已退出，由 onProcessFinished 报告原因 */
    }

    if (isPortListening(m_port)) {
        m_portPollTimer->stop();

        vsmetrics::log(
            QStringLiteral("SENTINEL MJPG-streamer 端口就绪 → %1")
                .arg(m_streamUrl));

        emit statusChanged(
            QStringLiteral("MJPG-streamer 已启动"));

        emit started(m_streamUrl);
        return;
    }

    m_portWaitElapsed += m_portPollTimer->interval();

    if (m_portWaitElapsed >= 3000) {
        m_portPollTimer->stop();

        vsmetrics::log(QStringLiteral(
            "SENTINEL MJPG-streamer 3 秒内未监听端口"));

        emit errorOccurred(
            QStringLiteral(
                "MJPG-streamer 启动后 3 秒内未监听端口 %1"
                "（常见原因：摄像头被其它页面占用）")
                .arg(m_port));

        m_process->kill();
    }
}

void MjpgStreamerService::onProcessFinished(
    int exitCode,
    QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitStatus);

    m_portPollTimer->stop();

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
