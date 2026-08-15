#include "MainWindow.h"

#include <QApplication>
#include <QByteArray>
#include <QDebug>
#include <QFile>
#include <QString>

#include <fcntl.h>
#include <linux/input.h>
#include <sys/ioctl.h>
#include <unistd.h>

static QByteArray findGt9147Event()
{
    for (int index = 0; index < 32; ++index) {
        QByteArray path = QByteArray("/dev/input/event");
        path.append(QByteArray::number(index));

        int fd = ::open(path.constData(), O_RDONLY | O_NONBLOCK);
        if (fd < 0)
            continue;

        char deviceName[128] = {0};
        int ret = ::ioctl(fd, EVIOCGNAME(sizeof(deviceName)), deviceName);

        ::close(fd);

        if (ret >= 0 &&
            QByteArray(deviceName) == QByteArray("gt9147-touchscreen")) {
            return path;
        }
    }

    return QByteArray();
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

    QByteArray touchDevice = findGt9147Event();

    if (!touchDevice.isEmpty()) {
        qputenv("QT_QPA_EVDEV_TOUCHSCREEN_PARAMETERS",
                touchDevice);
    }
}

int main(int argc, char *argv[])
{
    configureQtEnvironment();

    /*
     * 将未单独处理的触摸事件转换为鼠标事件，
     * 使 QPushButton、QToolButton 等控件能够响应触摸。
     */
    QApplication::setAttribute(
        Qt::AA_SynthesizeMouseForUnhandledTouchEvents,
        true);

    QApplication application(argc, argv);

    QByteArray touchDevice =
        qgetenv("QT_QPA_EVDEV_TOUCHSCREEN_PARAMETERS");

    if (touchDevice.isEmpty())
        qWarning() << "GT9147 input device was not found";
    else
        qInfo() << "Using GT9147 input device:" << touchDevice;

    QFile styleFile(QStringLiteral(":/style.qss"));

    if (styleFile.open(QFile::ReadOnly | QFile::Text)) {
        application.setStyleSheet(
            QString::fromUtf8(styleFile.readAll()));

        styleFile.close();
    } else {
        qWarning() << "Failed to open style resource:"
                << styleFile.errorString();
    }

    MainWindow window(
        QStringLiteral("/music"),
        QStringLiteral("/video"),
        QStringLiteral("/photo"));

    window.showFullScreen();

    return application.exec();
}