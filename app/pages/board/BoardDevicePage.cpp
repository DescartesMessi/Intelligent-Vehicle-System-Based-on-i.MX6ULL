#include "BoardDevicePage.h"

#include <QCheckBox>
#include <QGridLayout>
#include <QHideEvent>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QShowEvent>
#include <QVBoxLayout>

BoardDevicePage::BoardDevicePage(QWidget *parent)
    : QWidget(parent),
      m_service(new BoardDeviceService(this)),
      m_titleLabel(new QLabel(QStringLiteral("板载设备信息"), this)),
      m_statusLabel(new QLabel(QStringLiteral("设备页面未启动"), this)),
      m_ap3216cLabel(nullptr),
      m_dht11Label(nullptr),
      m_icm20608Label(nullptr),
      m_sr04Label(nullptr),
      m_sr501Label(nullptr),
      m_buzzerLabel(nullptr),
      m_backButton(new QPushButton(QStringLiteral("返回"), this)),
      m_buzzerButton(new QPushButton(QStringLiteral("开启蜂鸣器"), this)),
      m_autoAlarmCheckBox(new QCheckBox(QStringLiteral("SR04自动报警"), this))
{
    setObjectName(QStringLiteral("boardDevicePage"));
    setFixedSize(800, 480);

    setStyleSheet(
        "#boardDevicePage {"
        "    background-color: #101820;"
        "    color: #ffffff;"
        "}"
        "#boardDeviceCard {"
        "    background-color: #182734;"
        "    border: 1px solid #38566b;"
        "    border-radius: 10px;"
        "}"
        "#boardCardTitle {"
        "    color: #70d6ff;"
        "    font-size: 17px;"
        "    font-weight: bold;"
        "}"
        "#boardCardValue {"
        "    color: #f1f7fb;"
        "    font-size: 14px;"
        "}"
        "#boardStatusLabel {"
        "    color: #8bdcff;"
        "    font-size: 14px;"
        "}"
        "QPushButton {"
        "    min-height: 34px;"
        "    background-color: #263f52;"
        "    border: 1px solid #4d718a;"
        "    border-radius: 7px;"
        "    color: #ffffff;"
        "    padding-left: 10px;"
        "    padding-right: 10px;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #1384bd;"
        "}"
        "QCheckBox {"
        "    color: #ffffff;"
        "    font-size: 13px;"
        "}"
    );

    m_titleLabel->setAlignment(Qt::AlignCenter);
    m_titleLabel->setStyleSheet(
        "font-size: 23px;"
        "font-weight: bold;"
        "color: #ffffff;"
    );

    m_statusLabel->setObjectName(QStringLiteral("boardStatusLabel"));
    m_statusLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    m_backButton->setFixedSize(76, 36);

    QWidget *dht11Card =
        createCard(QStringLiteral("DHT11 温湿度"), &m_dht11Label);

    QWidget *ap3216cCard =
        createCard(QStringLiteral("AP3216C 光照/接近"), &m_ap3216cLabel);

    QWidget *icm20608Card =
        createCard(QStringLiteral("ICM20608 六轴传感器"),
                   &m_icm20608Label);

    QWidget *sr04Card =
        createCard(QStringLiteral("SR04 超声波测距"), &m_sr04Label);

    QWidget *sr501Card =
        createCard(QStringLiteral("SR501 人体红外"), &m_sr501Label);

    QWidget *buzzerCard =
        createCard(QStringLiteral("蜂鸣器控制"), &m_buzzerLabel);

    QVBoxLayout *buzzerLayout =
        qobject_cast<QVBoxLayout *>(buzzerCard->layout());

    QHBoxLayout *buzzerControlLayout = new QHBoxLayout;
    buzzerControlLayout->setContentsMargins(0, 0, 0, 0);
    buzzerControlLayout->setSpacing(8);
    buzzerControlLayout->addWidget(m_buzzerButton);
    buzzerControlLayout->addWidget(m_autoAlarmCheckBox);

    buzzerLayout->addLayout(buzzerControlLayout);

    m_autoAlarmCheckBox->setChecked(true);

    QHBoxLayout *headerLayout = new QHBoxLayout;
    headerLayout->setContentsMargins(0, 0, 0, 0);
    headerLayout->addWidget(m_backButton);
    headerLayout->addStretch();
    headerLayout->addWidget(m_titleLabel);
    headerLayout->addStretch();
    headerLayout->addWidget(m_statusLabel);

    QGridLayout *cardLayout = new QGridLayout;
    cardLayout->setContentsMargins(0, 0, 0, 0);
    cardLayout->setHorizontalSpacing(8);
    cardLayout->setVerticalSpacing(8);

    cardLayout->addWidget(dht11Card, 0, 0);
    cardLayout->addWidget(ap3216cCard, 0, 1);
    cardLayout->addWidget(icm20608Card, 1, 0);
    cardLayout->addWidget(sr04Card, 1, 1);
    cardLayout->addWidget(sr501Card, 2, 0);
    cardLayout->addWidget(buzzerCard, 2, 1);

    cardLayout->setColumnStretch(0, 1);
    cardLayout->setColumnStretch(1, 1);

    cardLayout->setRowStretch(0, 1);
    cardLayout->setRowStretch(1, 1);
    cardLayout->setRowStretch(2, 1);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(12, 8, 12, 10);
    mainLayout->setSpacing(8);
    mainLayout->addLayout(headerLayout);
    mainLayout->addLayout(cardLayout, 1);

    connect(m_backButton,
            &QPushButton::clicked,
            this,
            &BoardDevicePage::returnToHome);

    connect(m_buzzerButton,
            &QPushButton::clicked,
            this,
            &BoardDevicePage::onBuzzerButtonClicked);

    connect(m_autoAlarmCheckBox,
            &QCheckBox::stateChanged,
            this,
            &BoardDevicePage::onAutoAlarmChanged);

    connect(m_service,
            &BoardDeviceService::ap3216cUpdated,
            this,
            &BoardDevicePage::onAp3216cUpdated);

    connect(m_service,
            &BoardDeviceService::dht11Updated,
            this,
            &BoardDevicePage::onDht11Updated);

    connect(m_service,
            &BoardDeviceService::icm20608Updated,
            this,
            &BoardDevicePage::onIcm20608Updated);

    connect(m_service,
            &BoardDeviceService::sr04Updated,
            this,
            &BoardDevicePage::onSr04Updated);

    connect(m_service,
            &BoardDeviceService::sr501Updated,
            this,
            &BoardDevicePage::onSr501Updated);

    connect(m_service,
            &BoardDeviceService::buzzerUpdated,
            this,
            &BoardDevicePage::onBuzzerUpdated);

    connect(m_service,
            &BoardDeviceService::deviceError,
            this,
            &BoardDevicePage::onDeviceError);

    m_dht11Label->setText(
        QStringLiteral("温度：等待数据\n湿度：等待数据\n状态：未读取"));

    m_ap3216cLabel->setText(
        QStringLiteral("IR：等待数据\nALS：等待数据\nPS：等待数据\n接近：等待数据"));

    m_icm20608Label->setText(
        QStringLiteral("加速度：等待数据\n角速度：等待数据\n温度：等待数据\nWHO_AM_I：等待数据"));

    m_sr04Label->setText(
        QStringLiteral("距离：等待数据\n回波脉宽：等待数据\n状态：未读取\n报警：未读取"));

    m_sr501Label->setText(
        QStringLiteral("人体：等待数据\n有效：等待数据\n事件序号：等待数据"));

    m_buzzerLabel->setText(
        QStringLiteral("状态：关闭\n控制方式：手动/自动"));

    m_service->setBuzzer(false);
}

BoardDevicePage::~BoardDevicePage()
{
    m_service->stopMonitoring();
}

QWidget *BoardDevicePage::createCard(const QString &title,
                                     QLabel **valueLabel)
{
    QWidget *card = new QWidget(this);
    card->setObjectName(QStringLiteral("boardDeviceCard"));

    QVBoxLayout *layout = new QVBoxLayout(card);
    layout->setContentsMargins(12, 8, 12, 8);
    layout->setSpacing(5);

    QLabel *titleLabel = new QLabel(title, card);
    titleLabel->setObjectName(QStringLiteral("boardCardTitle"));

    QLabel *contentLabel = new QLabel(card);
    contentLabel->setObjectName(QStringLiteral("boardCardValue"));
    contentLabel->setWordWrap(true);
    contentLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    layout->addWidget(titleLabel);
    layout->addWidget(contentLabel, 1);

    *valueLabel = contentLabel;

    return card;
}

void BoardDevicePage::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);

    m_statusLabel->setText(QStringLiteral("设备采集中"));
    m_service->startMonitoring();
}

void BoardDevicePage::hideEvent(QHideEvent *event)
{
    m_service->stopMonitoring();

    QWidget::hideEvent(event);
}

void BoardDevicePage::returnToHome()
{
    m_service->stopMonitoring();
    emit backRequested();
}

void BoardDevicePage::onAp3216cUpdated(
    const BoardAp3216cData &data)
{
    const QString irStatus =
        data.irValid
            ? QStringLiteral("有效")
            : QStringLiteral("无效");

    const QString psStatus =
        data.psValid
            ? QStringLiteral("有效")
            : QStringLiteral("无效");

    const QString nearStatus =
        data.objectNear
            ? QStringLiteral("是")
            : QStringLiteral("否");

    m_ap3216cLabel->setText(
        QStringLiteral(
            "IR：%1（%2）\n"
            "ALS：%3\n"
            "PS：%4（%5）\n"
            "接近：%6")
            .arg(data.ir)
            .arg(irStatus)
            .arg(data.als)
            .arg(data.ps)
            .arg(psStatus)
            .arg(nearStatus));
}

void BoardDevicePage::onDht11Updated(
    const BoardDht11Data &data)
{
    m_dht11Label->setText(
        QStringLiteral(
            "温度：%1.%2 ℃\n"
            "湿度：%3.%4 %RH\n"
            "状态：正常")
            .arg(data.temperatureInteger)
            .arg(data.temperatureDecimal)
            .arg(data.humidityInteger)
            .arg(data.humidityDecimal));
}

void BoardDevicePage::onIcm20608Updated(
    const BoardIcm20608Data &data)
{
    m_icm20608Label->setText(
        QStringLiteral(
            "加速度(g)：%1，%2，%3\n"
            "角速度(dps)：%4，%5，%6\n"
            "温度：%7 ℃\n"
            "WHO_AM_I：0x%8")
            .arg(data.accelX, 0, 'f', 3)
            .arg(data.accelY, 0, 'f', 3)
            .arg(data.accelZ, 0, 'f', 3)
            .arg(data.gyroX, 0, 'f', 2)
            .arg(data.gyroY, 0, 'f', 2)
            .arg(data.gyroZ, 0, 'f', 2)
            .arg(data.temperature, 0, 'f', 2)
            .arg(static_cast<int>(data.whoAmI),
                 2,
                 16,
                 QLatin1Char('0'))
            .toUpper());
}

void BoardDevicePage::onSr04Updated(
    const BoardSr04Data &data)
{
    const QString alarmText =
        data.nearAlarm
            ? QStringLiteral("近距离报警")
            : QStringLiteral("安全");

    m_sr04Label->setText(
        QStringLiteral(
            "距离：%1 mm（%2 cm）\n"
            "回波脉宽：%3 us\n"
            "状态：%4\n"
            "报警：%5")
            .arg(data.distanceMm)
            .arg(data.distanceCm, 0, 'f', 1)
            .arg(data.pulseUs)
            .arg(data.valid
                    ? QStringLiteral("有效")
                    : QStringLiteral("无效"))
            .arg(alarmText));

    if (data.nearAlarm) {
        m_sr04Label->setStyleSheet(
            "color: #ff6b6b; font-size: 14px;");
    } else {
        m_sr04Label->setStyleSheet(
            "color: #f1f7fb; font-size: 14px;");
    }

    if (m_autoAlarmCheckBox->isChecked()) {
        const bool currentBuzzerState =
            m_service->buzzerEnabled();

        if (currentBuzzerState != data.nearAlarm)
            m_service->setBuzzer(data.nearAlarm);
    }
}

void BoardDevicePage::onSr501Updated(
    const BoardSr501Data &data)
{
    m_sr501Label->setText(
        QStringLiteral(
            "人体：%1\n"
            "有效：%2\n"
            "事件序号：%3")
            .arg(data.detected
                     ? QStringLiteral("检测到活动")
                     : QStringLiteral("未检测到"))
            .arg(data.valid
                     ? QStringLiteral("是")
                     : QStringLiteral("否"))
            .arg(data.sequence));
}

void BoardDevicePage::onBuzzerUpdated(bool enabled)
{
    m_buzzerLabel->setText(
        QStringLiteral(
            "状态：%1\n"
            "控制方式：%2")
            .arg(enabled
                     ? QStringLiteral("开启")
                     : QStringLiteral("关闭"))
            .arg(m_autoAlarmCheckBox->isChecked()
                     ? QStringLiteral("自动")
                     : QStringLiteral("手动")));

    m_buzzerButton->setText(
        enabled
            ? QStringLiteral("关闭蜂鸣器")
            : QStringLiteral("开启蜂鸣器"));
}

void BoardDevicePage::onBuzzerButtonClicked()
{
    m_service->setBuzzer(!m_service->buzzerEnabled());
}

void BoardDevicePage::onAutoAlarmChanged(int state)
{
    const bool enabled = state == Qt::Checked;

    m_buzzerLabel->setText(
        QStringLiteral(
            "状态：%1\n"
            "控制方式：%2")
            .arg(m_service->buzzerEnabled()
                     ? QStringLiteral("开启")
                     : QStringLiteral("关闭"))
            .arg(enabled
                     ? QStringLiteral("自动")
                     : QStringLiteral("手动")));
}

void BoardDevicePage::onDeviceError(
    const QString &deviceName,
    const QString &message)
{
    m_statusLabel->setText(
        QStringLiteral("%1：%2")
            .arg(deviceName, message));
}