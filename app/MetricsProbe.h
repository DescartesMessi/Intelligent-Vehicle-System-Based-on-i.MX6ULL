#ifndef METRICS_PROBE_H
#define METRICS_PROBE_H

/*
 * 性能指标采集开关（默认关闭，不影响正常使用）
 *
 * 板端用法：
 *     VS_METRICS=1 ./VehicleSystem                      只打印采样式指标
 *     VS_METRICS=1 VS_AUTOPILOT=5000 ./VehicleSystem    自动巡检各页面并采样
 *
 * 采集内容：
 *     - 进程启动到首页首帧的耗时（应用启动时间）
 *     - 每 2s 一次的 RSS / 线程数 / CPU（jiffies/2s，HZ=100 时约等于单核百分比）
 *     - 摄像头采集帧率 / 转换帧率 / 渲染帧率 / 丢帧率
 *     - 哨兵：SR501 触发时刻 → MJPG-streamer 进程启动时刻
 *
 * 之所以做成运行时开关而不是编译期宏：上板只有一份二进制，
 * 需要指标时用环境变量打开即可，不必为测量单独出固件。
 */

#include <QByteArray>
#include <QDebug>
#include <QElapsedTimer>
#include <QFile>
#include <QList>
#include <QString>

#include <cstdio>

namespace vsmetrics {

inline bool enabled()
{
    static const bool on = (qgetenv("VS_METRICS") == QByteArray("1"));
    return on;
}

/* 进程内单调时钟，第一次调用即作为零点（在 main() 最开始调用） */
inline QElapsedTimer &clock()
{
    static QElapsedTimer timer;
    static bool started = false;

    if (!started) {
        timer.start();
        started = true;
    }

    return timer;
}

inline qint64 ms()
{
    return clock().elapsed();
}

inline void log(const QString &line)
{
    if (!enabled())
        return;

    /*
     * 直接按 UTF-8 写 stderr，不走 qInfo()：
     * 本板 Qt 缺少可用的 QTextCodec（启动时会打印
     * "QIconvCodec::convertToUnicode ... iconv_open failed"），
     * qInfo() 会把中文降级成 '?'，日志就没法看了。
     */
    const QByteArray utf8 = line.toUtf8();

    std::fprintf(stderr, "[VS %7lldms] %s\n",
                 static_cast<long long>(ms()), utf8.constData());
}

/* /proc/self/status: VmRSS（kB） */
inline qint64 rssKb()
{
    QFile file(QStringLiteral("/proc/self/status"));

    if (!file.open(QIODevice::ReadOnly))
        return -1;

    /* 注意：procfs 文件的 size 为 0，QFile::atEnd() 会立刻返回 true，
     * 必须用 readAll() 一次性读完再按行切分。 */
    const QList<QByteArray> lines = file.readAll().split('\n');

    for (const QByteArray &line : lines) {
        if (line.startsWith("VmRSS:")) {
            const QList<QByteArray> parts = line.mid(6).trimmed().split(' ');
            return parts.value(0).toLongLong();
        }
    }

    return -1;
}

/* /proc/self/status: Threads */
inline int threadCount()
{
    QFile file(QStringLiteral("/proc/self/status"));

    if (!file.open(QIODevice::ReadOnly))
        return -1;

    const QList<QByteArray> lines = file.readAll().split('\n');

    for (const QByteArray &line : lines) {
        if (line.startsWith("Threads:"))
            return line.mid(8).trimmed().toInt();
    }

    return -1;
}

/*
 * /proc/self/stat 的 utime + stime（单位 jiffies）
 *
 * 注意：进程名可能带空格和括号，所以从最后一个 ')' 之后开始切分，
 * 该字段在 stat 里是第 3 个（state），utime/stime 是第 14/15 个。
 */
inline qint64 cpuJiffies()
{
    QFile file(QStringLiteral("/proc/self/stat"));

    if (!file.open(QIODevice::ReadOnly))
        return -1;

    const QByteArray data = file.readAll();
    const int close = data.lastIndexOf(')');

    if (close < 0)
        return -1;

    const QList<QByteArray> fields = data.mid(close + 2).split(' ');

    return fields.value(11).toLongLong() + fields.value(12).toLongLong();
}

} /* namespace vsmetrics */

#endif /* METRICS_PROBE_H */
