#include "MainWindow.h"
#include "MetricsProbe.h"

#include <QAbstractButton>
#include <QApplication>
#include <QByteArray>
#include <QDebug>
#include <QFile>
#include <QList>
#include <QSharedPointer>
#include <QStackedWidget>
#include <QString>
#include <QTimer>
#include <QWidget>

#include <fcntl.h>
#include <linux/input.h>
#include <sys/ioctl.h>
#include <unistd.h>

/* 读取 evdev 能力位：是否支持某事件类型下的某个 code */
static bool evdevHasBit(int fd, int type, int code)
{
    unsigned char bits[(KEY_MAX + 7) / 8] = {0};
    const int len = (type == 0) ? (EV_MAX + 7) / 8 : (code / 8 + 1);

    if (::ioctl(fd, EVIOCGBIT(type, len), bits) < 0)
        return false;

    return (bits[code / 8] >> (code % 8)) & 1;
}

static bool evdevHasEventType(int fd, int type)
{
    unsigned char bits[(EV_MAX + 7) / 8] = {0};

    if (::ioctl(fd, EVIOCGBIT(0, sizeof(bits)), bits) < 0)
        return false;

    return (bits[type / 8] >> (type % 8)) & 1;
}

struct InputDevices {
    QByteArray touchscreen;
    QByteArray mouse;
    QByteArray keyboard;
};

/*
 * 扫描 /dev/input/event*，按能力位区分触摸屏 / 鼠标 / 键盘。
 *
 * 识别依据：
 *   触摸屏 —— 设备名为 gt9147-touchscreen（本板固定）
 *   鼠标   —— 支持相对坐标 REL_X/REL_Y，或带 BTN_MOUSE 按键
 *   键盘   —— 支持 EV_KEY 且含字母键
 * 触摸屏只有 BTN_TOUCH、没有字母键和 BTN_MOUSE，因此不会被误判成鼠标或键盘。
 */
static InputDevices probeInputDevices()
{
    InputDevices found;

    for (int index = 0; index < 32; ++index) {
        QByteArray path = QByteArray("/dev/input/event");
        path.append(QByteArray::number(index));

        int fd = ::open(path.constData(), O_RDONLY | O_NONBLOCK);
        if (fd < 0)
            continue;

        char deviceName[128] = {0};
        ::ioctl(fd, EVIOCGNAME(sizeof(deviceName)), deviceName);

        const bool isGt9147 =
            QByteArray(deviceName) == QByteArray("gt9147-touchscreen");
        const bool hasRel = evdevHasEventType(fd, EV_REL);
        const bool hasKey = evdevHasEventType(fd, EV_KEY);
        const bool hasRelXY = hasRel &&
            evdevHasBit(fd, EV_REL, REL_X) && evdevHasBit(fd, EV_REL, REL_Y);
        const bool hasMouseButton = hasKey && evdevHasBit(fd, EV_KEY, BTN_MOUSE);
        const bool hasLetterKey = hasKey && evdevHasBit(fd, EV_KEY, KEY_A);

        ::close(fd);

        if (isGt9147) {
            if (found.touchscreen.isEmpty())
                found.touchscreen = path;
        } else if (hasRelXY || hasMouseButton) {
            if (found.mouse.isEmpty())
                found.mouse = path;
            if (hasLetterKey && found.keyboard.isEmpty())
                found.keyboard = path;   /* 键鼠一体设备两边都登记 */
        } else if (hasLetterKey) {
            if (found.keyboard.isEmpty())
                found.keyboard = path;
        }
    }

    return found;
}

static void configureQtEnvironment()
{
    qputenv("QT_PLUGIN_PATH",
            QByteArray("/usr/local/qt5/plugins"));

    qputenv("QT_QPA_PLATFORM_PLUGIN_PATH",
            QByteArray("/usr/local/qt5/plugins/platforms"));

    if (qgetenv("QT_QPA_PLATFORM").isEmpty()) {
        qputenv("QT_QPA_PLATFORM",
                QByteArray("linuxfb:fb=/dev/fb0"));
    }

    qputenv("QT_QPA_FB_WIDTH", QByteArray("800"));
    qputenv("QT_QPA_FB_HEIGHT", QByteArray("480"));

    const InputDevices input = probeInputDevices();

    if (!input.touchscreen.isEmpty()) {
        qputenv("QT_QPA_EVDEV_TOUCHSCREEN_PARAMETERS",
                input.touchscreen);
        qInfo() << "Touchscreen:" << input.touchscreen;
    } else {
        qWarning() << "GT9147 touchscreen was not found";
    }

    if (!input.mouse.isEmpty())
        qInfo() << "Mouse:" << input.mouse;
    else
        qInfo() << "Mouse: not present (hotplug will be probed by Qt)";

    if (!input.keyboard.isEmpty())
        qInfo() << "Keyboard:" << input.keyboard;
    else
        qInfo() << "Keyboard: not present (hotplug will be probed by Qt)";

    /*
     * 通过 QT_QPA_GENERIC_PLUGINS 加载 evdev 鼠标/键盘插件。
     * 识别到具体设备时直接指定路径；一个都没有时退化为让 Qt 自行探测
     * （插件内部支持热插拔，运行中再插上鼠标也能识别）。
     */
    QList<QByteArray> genericPlugins;

    if (!input.mouse.isEmpty()) {
        genericPlugins.append(
            QByteArray("evdevmouse:") + input.mouse);
    }

    if (!input.keyboard.isEmpty()) {
        genericPlugins.append(
            QByteArray("evdevkeyboard:") + input.keyboard);
    }

    if (!genericPlugins.isEmpty()) {
        qputenv("QT_QPA_GENERIC_PLUGINS",
                genericPlugins.join(','));
    } else if (qgetenv("QT_QPA_GENERIC_PLUGINS").isEmpty()) {
        qputenv("QT_QPA_GENERIC_PLUGINS",
                QByteArray("evdevmouse,evdevkeyboard"));
    }
}

/* 当前正在显示的页面（QStackedWidget 的当前页） */
static QWidget *currentPage(QWidget *root)
{
    QStackedWidget *stack = root->findChild<QStackedWidget *>();

    return stack ? stack->currentWidget() : nullptr;
}

/* 在指定范围内按按钮文字点击（巡检用，等价于手指点一下） */
static bool clickButtonByText(QWidget *scope, const QString &text)
{
    if (!scope)
        return false;

    const QList<QAbstractButton *> buttons =
        scope->findChildren<QAbstractButton *>();

    for (QAbstractButton *button : buttons) {
        if (button->isVisible() && button->isEnabled() &&
            button->text() == text) {
            button->click();
            return true;
        }
    }

    return false;
}

/*
 * 每 2s 采样一次：当前页面、RSS、线程数、这段时间消耗的 CPU jiffies。
 * 本板 HZ=100，所以 "jiffies / 2s ÷ 2" 约等于单核占用百分比。
 */
static void startMetricsSampler(QWidget *root)
{
    if (!vsmetrics::enabled())
        return;

    QTimer *timer = new QTimer(root);
    timer->setInterval(2000);

    QObject::connect(timer, &QTimer::timeout, root, [root]() {
        static qint64 lastJiffies = vsmetrics::cpuJiffies();

        const qint64 nowJiffies = vsmetrics::cpuJiffies();
        const qint64 delta = nowJiffies - lastJiffies;
        lastJiffies = nowJiffies;

        QWidget *page = currentPage(root);

        vsmetrics::log(
            QStringLiteral("METRIC page=%1 rss=%2kB threads=%3 cpu=%4jiffies/2s")
                .arg(page ? page->objectName() : QStringLiteral("?"))
                .arg(vsmetrics::rssKb())
                .arg(vsmetrics::threadCount())
                .arg(delta));
    });

    timer->start();
    vsmetrics::log(QStringLiteral("指标采样已启动（每 2s 一条 METRIC）"));
}

/*
 * 自动巡检：按固定顺序进入各功能页（可带一个页面内动作），
 * 用于在没有人工点击的情况下稳定复现"各页面占用"。
 *
 * VS_AUTOPILOT 的值是每步停留时间（ms），0 或缺省表示不巡检。
 * 巡检走两轮：第一轮含页面首次构造，第二轮是已构造页面的切换开销。
 */
static void startAutopilot(QWidget *root)
{
    const int dwellMs = qgetenv("VS_AUTOPILOT").toInt();

    if (dwellMs <= 0)
        return;

    struct Step {
        const char *entry;
        const char *action;
        const char *label;
    };

    /*
     * 注意：这里的字符串是 UTF-8 字面量，必须用 QString::fromUtf8 转成
     * QString 再和按钮文字比较。用 QLatin1String 会把一个汉字拆成 3 个
     * 字节字符，比较永远不相等（第一次巡检"点不到按钮"就是这个原因）。
     */
    static const Step steps[] = {
        {nullptr,      nullptr,      "首页空闲"},
        {"播放音乐",   nullptr,      "音乐页"},
        {"播放视频",   nullptr,      "视频页"},
        {"板载设备",   nullptr,      "板载设备页"},
        {"摄像头",     "开始",       "摄像头预览"},
        {"哨兵监控",   "开始监控",   "哨兵监控"},
        {"倒车影像",   "启动监控",   "倒车影像"},
    };

    const int stepCount = int(sizeof(steps) / sizeof(steps[0]));

    struct State {
        int index = 0;
        int round = 1;
    };

    QSharedPointer<State> state(new State);
    QTimer *timer = new QTimer(root);
    timer->setInterval(dwellMs);

    vsmetrics::log(QStringLiteral("自动巡检启动：每步 %1ms，共 2 轮")
                       .arg(dwellMs));

    QObject::connect(timer, &QTimer::timeout, root, [root, state, timer]() {
        const bool onHome = (currentPage(root) &&
                             currentPage(root)->objectName() ==
                                 QLatin1String("homePage"));

        /* 不在首页就先返回，下一步再进入目标页 */
        if (!onHome) {
            if (!clickButtonByText(currentPage(root), QStringLiteral("返回"))) {
                vsmetrics::log(QStringLiteral("巡检：当前页找不到\"返回\"按钮，"
                                              "强制回首页"));
                QWidget *home = root->findChild<QWidget *>(QStringLiteral("homePage"));
                QStackedWidget *stack = root->findChild<QStackedWidget *>();

                if (home && stack)
                    stack->setCurrentWidget(home);
            }
            return;
        }

        const Step &step = steps[state->index];

        if (step.entry) {
            const qint64 before = vsmetrics::ms();
            const bool ok = clickButtonByText(root, QString::fromUtf8(step.entry));
            const qint64 cost = vsmetrics::ms() - before;

            vsmetrics::log(QStringLiteral("巡检[第%1轮] 进入 %2：%3（页面构造+切换 %4ms）")
                               .arg(state->round)
                               .arg(QString::fromUtf8(step.label))
                               .arg(ok ? QStringLiteral("成功") : QStringLiteral("失败"))
                               .arg(cost));

            if (ok && step.action)
                clickButtonByText(currentPage(root),
                                  QString::fromUtf8(step.action));
        } else {
            vsmetrics::log(QStringLiteral("巡检[第%1轮] %2")
                               .arg(state->round)
                               .arg(QString::fromUtf8(step.label)));
        }

        if (++state->index >= stepCount) {
            state->index = 0;

            if (++state->round > 2) {
                timer->stop();
                vsmetrics::log(QStringLiteral("自动巡检结束（2 轮）"));

                /*
                 * VS_AUTOPILOT_EXIT=1：巡检结束后自动退出。
                 * 无人值守跑指标时避免应用留在板上（例如输入设备被拔掉后
                 * Qt 的 evdev 插件会不断重试，把串口刷满）。
                 */
                if (qgetenv("VS_AUTOPILOT_EXIT") == QByteArray("1")) {
                    vsmetrics::log(QStringLiteral("按 VS_AUTOPILOT_EXIT=1 退出应用"));
                    qApp->quit();
                }
            }
        }
    });

    timer->start();
}

int main(int argc, char *argv[])
{
    vsmetrics::log(QStringLiteral("进程启动（main 入口）"));

    configureQtEnvironment();

    vsmetrics::log(QStringLiteral("阶段1 输入设备探测完成"));

    /*
     * 将未单独处理的触摸事件转换为鼠标事件，
     * 使 QPushButton、QToolButton 等控件能够响应触摸。
     */
    QApplication::setAttribute(
        Qt::AA_SynthesizeMouseForUnhandledTouchEvents,
        true);

    QApplication application(argc, argv);

    vsmetrics::log(QStringLiteral("阶段2 QApplication 构造完成"));

    QFile styleFile(QStringLiteral(":/style.qss"));

    if (styleFile.open(QFile::ReadOnly | QFile::Text)) {
        application.setStyleSheet(
            QString::fromUtf8(styleFile.readAll()));

        styleFile.close();
    } else {
        qWarning() << "Failed to open style resource:"
                << styleFile.errorString();
    }

    vsmetrics::log(QStringLiteral("阶段3 样式表加载完成"));

    MainWindow window(
        QStringLiteral("/music"),
        QStringLiteral("/video"),
        QStringLiteral("/photo"));

    vsmetrics::log(QStringLiteral("阶段4 MainWindow（含首页）构造完成"));

    window.showFullScreen();

    vsmetrics::log(QStringLiteral("窗口显示完成（showFullScreen 返回）"));

    startMetricsSampler(&window);
    startAutopilot(&window);

    /* 事件循环的第一轮即首页首帧完成绘制 */
    QTimer::singleShot(0, &window, []() {
        vsmetrics::log(QStringLiteral("事件循环首轮（首页首帧）"));
    });

    return application.exec();
}
