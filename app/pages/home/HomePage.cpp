#include "HomePage.h"

#include <QDateTime>
#include <QFile>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QPixmap>
#include <QPushButton>
#include <QStyle>
#include <QTimer>
#include <QToolButton>
#include <QVBoxLayout>
#include <QDebug>
#include <QFile>
#include <QPixmap>

HomePage::HomePage(QWidget *parent)
    : QWidget(parent),
    m_dateLabel(new QLabel(this)),
    m_timeLabel(new QLabel(this)),
    m_temperatureLabel(new QLabel(QStringLiteral("温度 -- ℃"), this)),
    m_humidityLabel(new QLabel(QStringLiteral("湿度 -- %"), this)),
    m_statusLabel(new QLabel(QStringLiteral("系统状态：正常"), this)),
    m_clockTimer(new QTimer(this))
{
    setFixedSize(800, 480);
    setObjectName(QStringLiteral("homePage"));

    setStyleSheet(
        "#homePage { background-color: #f4f7fb; }"
        "QLabel { color: #1f2937; }"
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
    m_timeLabel->setStyleSheet("font-size: 32px;""font-weight: bold;""color: #162b49;""border: none;");

    m_dateLabel->setAlignment(Qt::AlignCenter);
    m_dateLabel->setStyleSheet("font-size: 15px;""color: #6b7a90;""border: none;");

    timeLayout->addWidget(m_timeLabel);
    timeLayout->addWidget(m_dateLabel);

    QWidget *environmentPanel = new QWidget(this);
    environmentPanel->setStyleSheet(
        "QWidget {"
        "    background-color: #ffffff;"
        "    border: 2px solid #d7dee8;"
        "    border-radius: 16px;"
        "}"
    );

    QHBoxLayout *environmentLayout = new QHBoxLayout(environmentPanel);
    environmentLayout->setContentsMargins(12, 4, 12, 4);
    environmentLayout->setSpacing(20);

    m_temperatureLabel->setStyleSheet(
        "font-size: 18px;"
        "color: #e87532;"
        "border: none;"
    );

    m_humidityLabel->setStyleSheet("font-size: 18px;""color: #2385c7;""border: none;");
    environmentLayout->addWidget(m_temperatureLabel);
    environmentLayout->addWidget(m_humidityLabel);
    environmentLayout->addStretch();

    QWidget *leftTopPanel = new QWidget(this);
    QVBoxLayout *leftTopLayout = new QVBoxLayout(leftTopPanel);
    leftTopLayout->setContentsMargins(0, 0, 0, 0);
    leftTopLayout->setSpacing(10);
    leftTopLayout->addWidget(timePanel);
    leftTopLayout->addWidget(environmentPanel);
    leftTopLayout->addStretch();

    QToolButton *musicButton = createEntryButton(
        QStringLiteral("播放音乐"),
        QStringLiteral(":/icons/music.png"),
        style()->standardIcon(QStyle::SP_MediaPlay));

    QToolButton *videoButton = createEntryButton(
        QStringLiteral("播放视频"),
        QStringLiteral(":/icons/video.png"),
        style()->standardIcon(QStyle::SP_ComputerIcon));

    QToolButton *diagnosticButton = createEntryButton(
        QStringLiteral("板载设备"),
        QStringLiteral(":/icons/diagnostic.png"),
        style()->standardIcon(QStyle::SP_FileDialogDetailedView));

    QToolButton *sentinelButton = createEntryButton(
        QStringLiteral("哨兵监控"),
        QStringLiteral(":/icons/sentinel.png"),
        style()->standardIcon(QStyle::SP_MessageBoxInformation));

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

    connect(musicButton, &QToolButton::clicked, this, [this]() {
        emit audioRequested();
    });

    connect(videoButton, &QToolButton::clicked, this, [this]() {
        emit videoRequested();
    });

    connect(diagnosticButton, &QToolButton::clicked, this, [this]() {
        emit diagnosticRequested();
    });

    connect(sentinelButton, &QToolButton::clicked, this, [this]() {
        emit sentinelRequested();
    });

    connect(reverseButton, &QToolButton::clicked, this, [this]() {
        emit reverseRequested();
    });

    connect(m_clockTimer, &QTimer::timeout, this, [this]() {
        updateDateTime();
    });

    updateDateTime();
    m_clockTimer->start(1000);
}

QToolButton *HomePage::createEntryButton(const QString &text,const QString &iconPath,const QIcon &fallbackIcon)
{
    QToolButton *button = new QToolButton(this);
    QIcon icon(iconPath);
    if (icon.isNull())
        icon = fallbackIcon;
    button->setText(text);
    button->setIcon(icon);
    button->setIconSize(QSize(86, 86));
    button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
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
    const QDateTime now = QDateTime::currentDateTime();

    m_timeLabel->setText(now.toString(QStringLiteral("hh:mm:ss")));
    m_dateLabel->setText(now.toString(QStringLiteral("yyyy年 MM月 dd日")));

    m_statusLabel->setText(
        QStringLiteral("系统状态：正常    DHT11：等待传感器数据"));
}