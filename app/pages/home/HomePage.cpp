#include "HomePage.h"

#include "MetricsProbe.h"

#include <QDateTime>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QHideEvent>
#include <QLabel>
#include <QShowEvent>
#include <QSize>
#include <QStyle>
#include <QTimer>
#include <QToolButton>
#include <QVBoxLayout>

#include "smarthome_dht11.h"

#include <cstring>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

HomePage::HomePage(QWidget *parent)
    : QWidget(parent),
      m_dateLabel(new QLabel(this)),
      m_timeLabel(new QLabel(this)),
      m_temperatureLabel(
          new QLabel(QStringLiteral("温度 -- ℃"), this)),
      m_humidityLabel(
          new QLabel(QStringLiteral("湿度 -- %"), this)),
      m_statusLabel(
          new QLabel(QStringLiteral("系统状态：正常"), this)),
      m_clockTimer(new QTimer(this)),
      m_environmentTimer(new QTimer(this))
{
    vsmetrics::log(QStringLiteral("  [首页] 构造开始"));

    setFixedSize(800, 480);
    setObjectName(QStringLiteral("homePage"));

    m_environmentTimer->setInterval(2500);

    setStyleSheet(
        "#homePage {"
        "    background-color: #f4f7fb;"
        "}"
        "QLabel {"
        "    color: #1f2937;"
        "}"
        "QToolButton {"
        "    background-color: #ffffff;"
        "    border: 2px solid #d7dee8;"
        "    border-radius: 18px;"
        "    color: #1f2937;"
        "    font-size: 19px;"
        "    font-weight: bold;"
        "    padding: 8px;"
        "}"
        "QToolButton:hover {"
        "    background-color: #eaf4ff;"
        "    border: 2px solid #2f9bea;"
        "}"
        "QToolButton:pressed {"
        "    background-color: #cce8ff;"
        "}"
    );

    QGridLayout *layout = new QGridLayout(this);
    layout->setContentsMargins(24, 20, 24, 20);
    layout->setHorizontalSpacing(18);
    layout->setVerticalSpacing(16);

    QWidget *timePanel = new QWidget(this);

    vsmetrics::log(QStringLiteral("  [首页] 样式表+网格布局完成"));
    timePanel->setStyleSheet(
        "QWidget {"
        "    background-color: #ffffff;"
        "    border: 2px solid #d7dee8;"
        "    border-radius: 16px;"
        "}"
    );

    QVBoxLayout *timeLayout = new QVBoxLayout(timePanel);
    timeLayout->setContentsMargins(14, 8, 14, 8);
    timeLayout->setSpacing(0);

    m_timeLabel->setAlignment(Qt::AlignCenter);
    m_timeLabel->setStyleSheet(
        "font-size: 32px;"
        "font-weight: bold;"
        "color: #162b49;"
        "border: none;"
    );

    m_dateLabel->setAlignment(Qt::AlignCenter);
    m_dateLabel->setStyleSheet(
        "font-size: 15px;"
        "color: #6b7a90;"
        "border: none;"
    );

    timeLayout->addWidget(m_timeLabel);
    timeLayout->addWidget(m_dateLabel);

    vsmetrics::log(QStringLiteral("  [首页] 时间面板完成"));

    QWidget *environmentPanel = new QWidget(this);
    environmentPanel->setStyleSheet(
        "QWidget {"
        "    background-color: #ffffff;"
        "    border: 2px solid #d7dee8;"
        "    border-radius: 16px;"
        "}"
    );

    QHBoxLayout *environmentLayout =
        new QHBoxLayout(environmentPanel);

    environmentLayout->setContentsMargins(12, 4, 12, 4);
    environmentLayout->setSpacing(20);

    m_temperatureLabel->setStyleSheet(
        "font-size: 18px;"
        "color: #e87532;"
        "border: none;"
    );

    m_humidityLabel->setStyleSheet(
        "font-size: 18px;"
        "color: #2385c7;"
        "border: none;"
    );

    environmentLayout->addWidget(m_temperatureLabel);
    environmentLayout->addWidget(m_humidityLabel);
    environmentLayout->addStretch();

    vsmetrics::log(QStringLiteral("  [首页] 温湿度面板完成"));

    QToolButton *cameraButton = new QToolButton(this);
    cameraButton->setObjectName(
        QStringLiteral("homeCameraButton"));
    cameraButton->setText(QStringLiteral("摄像头"));
    cameraButton->setIcon(
        QIcon(QStringLiteral(":/icons/camera.png")));
    cameraButton->setIconSize(QSize(38, 38));
    cameraButton->setToolButtonStyle(
        Qt::ToolButtonTextBesideIcon);
    cameraButton->setFixedHeight(58);

    vsmetrics::log(QStringLiteral("  [首页] 摄像头按钮完成"));

    /*
     * 退出程序按钮：紧挨摄像头按钮放在同一行。
     * 单独用红色描边区分，避免和普通功能入口混淆；
     * 实际退出动作由 MainWindow 处理（带一次确认），
     * HomePage 只负责发出 exitRequested() 信号。
     */
    QToolButton *exitButton = new QToolButton(this);
    exitButton->setObjectName(QStringLiteral("homeExitButton"));
    exitButton->setText(QStringLiteral("退出"));
    exitButton->setIcon(
        style()->standardIcon(QStyle::SP_DialogCloseButton));
    exitButton->setIconSize(QSize(34, 34));
    exitButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    exitButton->setFixedHeight(58);
    exitButton->setFixedWidth(104);
    exitButton->setStyleSheet(
        "QToolButton#homeExitButton {"
        "    background-color: #fff5f5;"
        "    border: 2px solid #f0b8b8;"
        "    border-radius: 18px;"
        "    color: #c62828;"
        "    font-size: 19px;"
        "    font-weight: bold;"
        "    padding: 8px;"
        "}"
        "QToolButton#homeExitButton:hover {"
        "    background-color: #ffe3e3;"
        "    border: 2px solid #e05252;"
        "}"
        "QToolButton#homeExitButton:pressed {"
        "    background-color: #ffc9c9;"
        "}"
    );

    QWidget *cameraRow = new QWidget(this);

    QHBoxLayout *cameraRowLayout = new QHBoxLayout(cameraRow);

    cameraRowLayout->setContentsMargins(0, 0, 0, 0);
    cameraRowLayout->setSpacing(10);
    cameraRowLayout->addWidget(cameraButton, 1);
    cameraRowLayout->addWidget(exitButton, 0);

    vsmetrics::log(QStringLiteral("  [首页] 退出按钮+一行布局完成"));

    QWidget *leftTopPanel = new QWidget(this);

    QVBoxLayout *leftTopLayout =
        new QVBoxLayout(leftTopPanel);

    leftTopLayout->setContentsMargins(0, 0, 0, 0);
    leftTopLayout->setSpacing(10);
    leftTopLayout->addWidget(timePanel);
    leftTopLayout->addWidget(environmentPanel);
    leftTopLayout->addWidget(cameraRow);
    leftTopLayout->addStretch();

    QToolButton *musicButton = createEntryButton(
        QStringLiteral("播放音乐"),
        QStringLiteral(":/icons/music.png"),
        style()->standardIcon(QStyle::SP_MediaPlay));

    vsmetrics::log(QStringLiteral("  [首页] 时间/环境面板完成"));

    QToolButton *videoButton = createEntryButton(
        QStringLiteral("播放视频"),
        QStringLiteral(":/icons/video.png"),
        style()->standardIcon(QStyle::SP_ComputerIcon));

    QToolButton *diagnosticButton = createEntryButton(
        QStringLiteral("板载设备"),
        QStringLiteral(":/icons/diagnostic.png"),
        style()->standardIcon(
            QStyle::SP_FileDialogDetailedView));

    QToolButton *sentinelButton = createEntryButton(
        QStringLiteral("哨兵监控"),
        QStringLiteral(":/icons/sentinel.png"),
        style()->standardIcon(
            QStyle::SP_MessageBoxInformation));

    QToolButton *reverseButton = createEntryButton(
        QStringLiteral("倒车影像"),
        QStringLiteral(":/icons/reverse.png"),
        style()->standardIcon(QStyle::SP_DriveDVDIcon));

    layout->addWidget(leftTopPanel, 0, 0, 2, 1);
    layout->addWidget(musicButton, 0, 1, 2, 1);
    layout->addWidget(videoButton, 0, 2, 2, 1);
    layout->addWidget(diagnosticButton, 2, 0, 2, 1);
    layout->addWidget(sentinelButton, 2, 1, 2, 1);
    layout->addWidget(reverseButton, 2, 2, 2, 1);

    vsmetrics::log(QStringLiteral("  [首页] 7 个入口按钮完成"));

    layout->setColumnStretch(0, 4);
    layout->setColumnStretch(1, 3);
    layout->setColumnStretch(2, 3);

    layout->setRowStretch(0, 1);
    layout->setRowStretch(1, 1);
    layout->setRowStretch(2, 1);
    layout->setRowStretch(3, 1);

    m_statusLabel->setAlignment(Qt::AlignCenter);
    m_statusLabel->setStyleSheet(
        "font-size: 14px;"
        "color: #5c6b7f;"
        "border: none;"
    );

    layout->addWidget(m_statusLabel, 4, 0, 1, 3);

    connect(musicButton,
            &QToolButton::clicked,
            this,
            [this]() {
        emit audioRequested();
    });

    connect(videoButton,
            &QToolButton::clicked,
            this,
            [this]() {
        emit videoRequested();
    });

    connect(diagnosticButton,
            &QToolButton::clicked,
            this,
            [this]() {
        emit diagnosticRequested();
    });

    connect(sentinelButton,
            &QToolButton::clicked,
            this,
            [this]() {
        emit sentinelRequested();
    });

    connect(reverseButton,
            &QToolButton::clicked,
            this,
            [this]() {
        emit reverseRequested();
    });

    connect(cameraButton,
            &QToolButton::clicked,
            this,
            [this]() {
        emit cameraRequested();
    });

    connect(exitButton,
            &QToolButton::clicked,
            this,
            [this]() {
        emit exitRequested();
    });

    connect(m_clockTimer,
            &QTimer::timeout,
            this,
            &HomePage::updateDateTime);

    connect(m_environmentTimer,
            &QTimer::timeout,
            this,
            &HomePage::updateEnvironment);

    updateDateTime();

    vsmetrics::log(QStringLiteral("  [首页] 构造结束"));
    /* 先显示首页，延迟读取 DHT11，避免传感器阻塞启动过程 */
    m_temperatureLabel->setText(QStringLiteral("温度 -- ℃"));
    m_humidityLabel->setText(QStringLiteral("湿度 -- %"));
    m_statusLabel->setText(QStringLiteral("系统状态：正常    DHT11：等待读取"));

    /* 设置温湿度读取周期 */
    m_environmentTimer->setInterval(2500);

    /* 延迟 1500ms 读取第一次温湿度 */
    QTimer::singleShot(1500, this, [this]() {
        updateEnvironment();
        m_environmentTimer->start();
    });

    /* 启动时间显示定时器 */
    m_clockTimer->start(1000);
}









void HomePage::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);

    updateDateTime();

    m_clockTimer->start(1000);

    /*
     * 只在首页显示时读取 DHT11。
     * 避免摄像头页面运行时阻塞 GUI 线程。
     */
    updateEnvironment();
    m_environmentTimer->start();
}

void HomePage::hideEvent(QHideEvent *event)
{
    m_clockTimer->stop();
    m_environmentTimer->stop();

    QWidget::hideEvent(event);
}

QToolButton *HomePage::createEntryButton(
    const QString &text,
    const QString &iconPath,
    const QIcon &fallbackIcon)
{
    QToolButton *button = new QToolButton(this);

    QIcon icon(iconPath);

    if (icon.isNull())
        icon = fallbackIcon;

    button->setText(text);
    button->setIcon(icon);
    button->setIconSize(QSize(86, 86));
    button->setToolButtonStyle(
        Qt::ToolButtonTextUnderIcon);
    button->setSizePolicy(
        QSizePolicy::Expanding,
        QSizePolicy::Expanding);
    button->setMinimumSize(190, 145);

    return button;
}

QLabel *HomePage::createPanelTitle(const QString &text)
{
    QLabel *label = new QLabel(text, this);
    label->setAlignment(Qt::AlignCenter);
    return label;
}

void HomePage::updateDateTime()
{
    const QDateTime now =
        QDateTime::currentDateTime();

    m_timeLabel->setText(
        now.toString(QStringLiteral("hh:mm:ss")));

    m_dateLabel->setText(
        now.toString(QStringLiteral("yyyy年 MM月 dd日")));
}

void HomePage::updateEnvironment()
{
    const int fd =
        ::open("/dev/dht11", O_RDONLY);

    if (fd < 0) {
        m_temperatureLabel->setText(
            QStringLiteral("温度 -- ℃"));

        m_humidityLabel->setText(
            QStringLiteral("湿度 -- %"));

        m_statusLabel->setText(
            QStringLiteral(
                "系统状态：正常    DHT11：打开失败"));

        return;
    }

    dht11_measurement measurement;
    std::memset(&measurement, 0, sizeof(measurement));

    const int result =
        ::ioctl(fd,
                DHT11_IOC_GET_MEASUREMENT,
                &measurement);

    ::close(fd);

    if (result < 0 || measurement.valid == 0) {
        m_temperatureLabel->setText(
            QStringLiteral("温度 -- ℃"));

        m_humidityLabel->setText(
            QStringLiteral("湿度 -- %"));

        m_statusLabel->setText(
            QStringLiteral(
                "系统状态：正常    DHT11：读取失败"));

        return;
    }

    m_temperatureLabel->setText(
        QStringLiteral("温度 %1.%2 ℃")
            .arg(measurement.temperature_integer)
            .arg(measurement.temperature_decimal));

    m_humidityLabel->setText(
        QStringLiteral("湿度 %1.%2 %")
            .arg(measurement.humidity_integer)
            .arg(measurement.humidity_decimal));

    m_statusLabel->setText(
        QStringLiteral(
            "系统状态：正常    DHT11：已连接"));
}
