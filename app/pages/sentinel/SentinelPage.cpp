#include "SentinelPage.h"

#include "SentinelService.h"

#include <QFont>
#include <QHBoxLayout>
#include <QHideEvent>
#include <QLabel>
#include <QPushButton>
#include <QShowEvent>
#include <QVBoxLayout>

SentinelPage::SentinelPage(QWidget *parent)
    : QWidget(parent),
      m_service(new SentinelService(this)),
      m_titleLabel(new QLabel(
          QStringLiteral("哨兵监控"), this)),
      m_sensorLabel(new QLabel(
          QStringLiteral("SR501：未启动"), this)),
      m_streamLabel(new QLabel(
          QStringLiteral("远程监控：未启动"), this)),
      m_urlLabel(new QLabel(
          QStringLiteral("远程地址：--"), this)),
      m_statusLabel(new QLabel(
          QStringLiteral("系统状态：就绪"), this)),
      m_backButton(new QPushButton(
          QStringLiteral("返回"), this)),
      m_startButton(new QPushButton(
          QStringLiteral("开始监控"), this)),
      m_stopButton(new QPushButton(
          QStringLiteral("停止监控"), this))
{
    setObjectName(QStringLiteral("sentinelPage"));
    setFixedSize(800, 480);

    m_titleLabel->setAlignment(Qt::AlignCenter);
    m_sensorLabel->setAlignment(Qt::AlignCenter);
    m_streamLabel->setAlignment(Qt::AlignCenter);
    m_urlLabel->setAlignment(Qt::AlignCenter);
    m_statusLabel->setAlignment(Qt::AlignCenter);

    QFont titleFont;
    titleFont.setPointSize(25);
    titleFont.setBold(true);
    m_titleLabel->setFont(titleFont);

    QFont infoFont;
    infoFont.setPointSize(18);
    m_sensorLabel->setFont(infoFont);
    m_streamLabel->setFont(infoFont);

    m_urlLabel->setTextInteractionFlags(
        Qt::TextSelectableByMouse);

    m_stopButton->setEnabled(false);

    QHBoxLayout *headerLayout = new QHBoxLayout;
    headerLayout->setContentsMargins(0, 0, 0, 0);
    headerLayout->addWidget(m_backButton);
    headerLayout->addStretch();
    headerLayout->addWidget(m_titleLabel);
    headerLayout->addStretch();

    QVBoxLayout *infoLayout = new QVBoxLayout;
    infoLayout->setSpacing(18);
    infoLayout->addStretch();
    infoLayout->addWidget(m_sensorLabel);
    infoLayout->addWidget(m_streamLabel);
    infoLayout->addWidget(m_urlLabel);
    infoLayout->addWidget(m_statusLabel);
    infoLayout->addStretch();

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->setSpacing(20);
    buttonLayout->addWidget(m_startButton);
    buttonLayout->addWidget(m_stopButton);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 15, 20, 15);
    mainLayout->setSpacing(15);
    mainLayout->addLayout(headerLayout);
    mainLayout->addLayout(infoLayout, 1);
    mainLayout->addLayout(buttonLayout);

    connect(m_backButton,
            &QPushButton::clicked,
            this,
            &SentinelPage::returnToHome);

    connect(m_startButton,
            &QPushButton::clicked,
            this,
            &SentinelPage::startMonitoring);

    connect(m_stopButton,
            &QPushButton::clicked,
            this,
            &SentinelPage::stopMonitoring);

    connect(m_service,
            &SentinelService::sensorStateChanged,
            this,
            &SentinelPage::onSensorStateChanged);

    connect(m_service,
            &SentinelService::streamStarted,
            this,
            &SentinelPage::onStreamStarted);

    connect(m_service,
            &SentinelService::streamStopped,
            this,
            &SentinelPage::onStreamStopped);

    connect(m_service,
            &SentinelService::statusChanged,
            this,
            &SentinelPage::onStatusChanged);

    connect(m_service,
            &SentinelService::errorOccurred,
            this,
            &SentinelPage::onServiceError);

    m_urlLabel->setText(
        QStringLiteral("远程地址：%1")
            .arg(m_service->streamUrl()));
}

SentinelPage::~SentinelPage()
{
    m_service->stopMonitoring();
}

void SentinelPage::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);

    startMonitoring();
}

void SentinelPage::hideEvent(QHideEvent *event)
{
    stopMonitoring();

    QWidget::hideEvent(event);
}

void SentinelPage::startMonitoring()
{
    if (m_service->isMonitoring())
        return;

    if (!m_service->startMonitoring())
        return;

    m_startButton->setEnabled(false);
    m_stopButton->setEnabled(true);

    m_sensorLabel->setText(
        QStringLiteral("SR501：监测中"));
}

void SentinelPage::stopMonitoring()
{
    if (!m_service->isMonitoring())
        return;

    m_service->stopMonitoring();

    m_startButton->setEnabled(true);
    m_stopButton->setEnabled(false);

    m_sensorLabel->setText(
        QStringLiteral("SR501：已停止"));

    m_streamLabel->setText(
        QStringLiteral("远程监控：已停止"));
}

void SentinelPage::returnToHome()
{
    stopMonitoring();
    emit backRequested();
}

void SentinelPage::onSensorStateChanged(
    bool detected,
    quint32 sequence)
{
    Q_UNUSED(sequence);

    if (detected) {
        m_sensorLabel->setText(
            QStringLiteral("SR501：检测到人员"));
    } else {
        m_sensorLabel->setText(
            QStringLiteral("SR501：未检测到人员"));
    }
}

void SentinelPage::onStreamStarted(const QString &url)
{
    m_streamLabel->setText(
        QStringLiteral("远程监控：已启动"));

    m_urlLabel->setText(
        QStringLiteral("远程地址：%1").arg(url));
}

void SentinelPage::onStreamStopped()
{
    m_streamLabel->setText(
        QStringLiteral("远程监控：已停止"));
}

void SentinelPage::onStatusChanged(
    const QString &message)
{
    m_statusLabel->setText(
        QStringLiteral("系统状态：%1").arg(message));
}

void SentinelPage::onServiceError(
    const QString &message)
{
    m_statusLabel->setText(
        QStringLiteral("错误：%1").arg(message));

    m_startButton->setEnabled(true);
    m_stopButton->setEnabled(false);
}