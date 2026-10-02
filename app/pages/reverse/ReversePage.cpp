#include "ReversePage.h"

#include "ReverseService.h"
#include "V4l2CameraService.h"

#include <QDateTime>
#include <QDir>
#include <QHBoxLayout>
#include <QHideEvent>
#include <QLabel>
#include <QMutexLocker>
#include <QPixmap>
#include <QPushButton>
#include <QShowEvent>
#include <QTimer>
#include <QVBoxLayout>

ReversePage::ReversePage(const QString &photoDirectory,
                         QWidget *parent)
    : QWidget(parent),
      m_photoDirectory(photoDirectory),
      m_cameraService(new V4l2CameraService(this)),
      m_reverseService(new ReverseService(this)),
      m_titleLabel(new QLabel(QStringLiteral("倒车影像"), this)),
      m_previewLabel(new QLabel(QStringLiteral("等待摄像头画面"), this)),
      m_distanceLabel(new QLabel(QStringLiteral("-- mm"), this)),
      m_alarmLabel(new QLabel(QStringLiteral("安全"), this)),
      m_statusLabel(new QLabel(QStringLiteral("等待启动"), this)),
      m_photoLabel(new QLabel(QStringLiteral("暂无照片"), this)),
      m_backButton(new QPushButton(QStringLiteral("返回"), this)),
      m_startButton(new QPushButton(QStringLiteral("启动监控"), this)),
      m_photoButton(new QPushButton(QStringLiteral("拍照"), this)),
      m_renderTimer(new QTimer(this)),
      m_running(false)
{
    setObjectName(QStringLiteral("reversePage"));
    setFixedSize(800, 480);

    m_titleLabel->setAlignment(Qt::AlignCenter);
    m_titleLabel->setStyleSheet(
        "font-size: 24px;"
        "font-weight: bold;"
        "color: #17365d;"
    );

    m_previewLabel->setMinimumSize(540, 380);
    m_previewLabel->setAlignment(Qt::AlignCenter);
    m_previewLabel->setStyleSheet(
        "background-color: #101820;"
        "border: 2px solid #278bd1;"
        "border-radius: 10px;"
        "color: white;"
        "font-size: 20px;"
    );

    m_distanceLabel->setAlignment(Qt::AlignCenter);
    m_distanceLabel->setStyleSheet(
        "font-size: 34px;"
        "font-weight: bold;"
        "color: #1677bd;"
    );

    m_alarmLabel->setAlignment(Qt::AlignCenter);
    m_alarmLabel->setStyleSheet(
        "font-size: 24px;"
        "font-weight: bold;"
        "color: #198754;"
    );

    m_statusLabel->setWordWrap(true);
    m_statusLabel->setAlignment(Qt::AlignCenter);
    m_statusLabel->setStyleSheet(
        "font-size: 15px;"
        "color: #495057;"
    );

    m_photoLabel->setFixedSize(150, 110);
    m_photoLabel->setAlignment(Qt::AlignCenter);
    m_photoLabel->setStyleSheet(
        "background-color: #e9f2fb;"
        "border: 1px solid #8bb8dc;"
        "color: #4c6a82;"
    );

    m_backButton->setFixedSize(80, 42);
    m_startButton->setMinimumHeight(42);
    m_photoButton->setMinimumHeight(42);

    m_photoButton->setEnabled(false);

    QHBoxLayout *headerLayout = new QHBoxLayout;
    headerLayout->setContentsMargins(0, 0, 0, 0);
    headerLayout->addWidget(m_backButton);
    headerLayout->addStretch();
    headerLayout->addWidget(m_titleLabel);
    headerLayout->addStretch();

    QVBoxLayout *controlLayout = new QVBoxLayout;
    controlLayout->setContentsMargins(0, 0, 0, 0);
    controlLayout->setSpacing(10);

    controlLayout->addWidget(new QLabel(QStringLiteral("障碍物距离"), this));
    controlLayout->addWidget(m_distanceLabel);
    controlLayout->addWidget(m_alarmLabel);
    controlLayout->addSpacing(8);
    controlLayout->addWidget(m_photoLabel);
    controlLayout->addWidget(m_startButton);
    controlLayout->addWidget(m_photoButton);
    controlLayout->addWidget(m_statusLabel);
    controlLayout->addStretch();

    QWidget *controlPanel = new QWidget(this);
    controlPanel->setFixedWidth(190);
    controlPanel->setLayout(controlLayout);

    QHBoxLayout *contentLayout = new QHBoxLayout;
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(10);
    contentLayout->addWidget(m_previewLabel, 1);
    contentLayout->addWidget(controlPanel);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 8, 10, 8);
    mainLayout->setSpacing(8);
    mainLayout->addLayout(headerLayout);
    mainLayout->addLayout(contentLayout, 1);

    connect(m_backButton,
            &QPushButton::clicked,
            this,
            &ReversePage::returnToHome);

    connect(m_startButton,
            &QPushButton::clicked,
            this,
            &ReversePage::startOrStop);

    connect(m_photoButton,
            &QPushButton::clicked,
            this,
            &ReversePage::takePhoto);

    connect(m_renderTimer,
            &QTimer::timeout,
            this,
            &ReversePage::renderLatestFrame);

    connect(m_cameraService,
            &V4l2CameraService::frameReady,
            this,
            &ReversePage::onFrameReady);

    connect(m_cameraService,
            &V4l2CameraService::captureStarted,
            this,
            &ReversePage::onCaptureStarted);

    connect(m_cameraService,
            &V4l2CameraService::captureStopped,
            this,
            &ReversePage::onCaptureStopped);

    connect(m_cameraService,
            &V4l2CameraService::errorOccurred,
            this,
            &ReversePage::onCameraError);

    connect(m_reverseService,
            &ReverseService::distanceChanged,
            this,
            &ReversePage::onDistanceChanged);

    connect(m_reverseService,
            &ReverseService::alarmChanged,
            this,
            &ReversePage::onAlarmChanged);

    connect(m_reverseService,
            &ReverseService::statusChanged,
            this,
            &ReversePage::onReverseStatusChanged);

    connect(m_reverseService,
            &ReverseService::errorOccurred,
            this,
            &ReversePage::onReverseError);

    m_renderTimer->setInterval(80);
}

ReversePage::~ReversePage()
{
    stopAll();
}

void ReversePage::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    startAll();
}

void ReversePage::hideEvent(QHideEvent *event)
{
    stopAll();
    QWidget::hideEvent(event);
}

void ReversePage::startAll()
{
    if (m_running)
        return;

    m_statusLabel->setText(QStringLiteral("正在启动倒车模块..."));
    m_startButton->setEnabled(false);

    const bool sensorStarted = m_reverseService->startMonitoring();

    if (!sensorStarted) {
        m_statusLabel->setText(
            QStringLiteral("SR04 或蜂鸣器设备打开失败"));
    }

    /*
     * 使用较低分辨率和帧率，保证 i.MX6ULL 上界面流畅。
     * 摄像头实际格式和分辨率由 V4L2 驱动返回。
     */
    const bool cameraStarted =
        m_cameraService->startCapture(
            QStringLiteral("/dev/video1"),
            320,
            240,
            10);

    if (!cameraStarted) {
        m_statusLabel->setText(
            QStringLiteral("摄像头启动失败"));
    }

    m_renderTimer->start();
    m_running = sensorStarted || cameraStarted;
    updateRunningState(m_running);
}

void ReversePage::stopAll()
{
    m_renderTimer->stop();

    if (m_cameraService->isCapturing())
        m_cameraService->stopCapture();

    if (m_reverseService->isMonitoring())
        m_reverseService->stopMonitoring();

    m_running = false;
    updateRunningState(false);
}

void ReversePage::startOrStop()
{
    if (m_running) {
        stopAll();
        m_statusLabel->setText(QStringLiteral("倒车模块已停止"));
    } else {
        startAll();
    }
}

void ReversePage::updateRunningState(bool running)
{
    m_running = running;

    if (running) {
        m_startButton->setText(QStringLiteral("停止监控"));
        m_photoButton->setEnabled(!m_latestFrame.isNull());
    } else {
        m_startButton->setText(QStringLiteral("启动监控"));
        m_startButton->setEnabled(true);
        m_photoButton->setEnabled(false);
    }
}

void ReversePage::takePhoto()
{
    QImage image;

    {
        QMutexLocker locker(&m_frameMutex);
        image = m_latestFrame.copy();
    }

    if (image.isNull()) {
        m_statusLabel->setText(QStringLiteral("当前没有可保存的画面"));
        return;
    }

    QDir photoDirectory(m_photoDirectory);

    if (!photoDirectory.exists() &&
        !photoDirectory.mkpath(QStringLiteral("."))) {
        m_statusLabel->setText(
            QStringLiteral("无法创建照片目录：%1")
                .arg(m_photoDirectory));
        return;
    }

    const QString timestamp =
        QDateTime::currentDateTime().toString(
            QStringLiteral("yyyyMMdd_HHmmss_zzz"));

    const QString filePath =
        photoDirectory.filePath(
            QStringLiteral("reverse_%1.jpg").arg(timestamp));

    if (!image.save(filePath, "JPG", 85)) {
        m_statusLabel->setText(
            QStringLiteral("照片保存失败：%1").arg(filePath));
        return;
    }

    const QPixmap thumbnail =
        QPixmap::fromImage(image.scaled(
            m_photoLabel->size(),
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation));

    m_photoLabel->setPixmap(thumbnail);

    m_statusLabel->setText(
        QStringLiteral("倒车照片已保存：%1").arg(filePath));
}

void ReversePage::returnToHome()
{
    stopAll();
    emit backRequested();
}

void ReversePage::onFrameReady(const QImage &image)
{
    if (image.isNull())
        return;

    /*
     * 不在摄像头线程中直接刷新界面。
     * 这里只保留最新一帧，避免 QImage 信号积压导致卡顿。
     */
    QMutexLocker locker(&m_frameMutex);
    m_latestFrame = image.copy();

    m_photoButton->setEnabled(true);
}

void ReversePage::renderLatestFrame()
{
    QImage image;

    {
        QMutexLocker locker(&m_frameMutex);
        if (m_latestFrame.isNull())
            return;

        image = m_latestFrame.copy();
    }

    const QPixmap preview =
        QPixmap::fromImage(image.scaled(
            m_previewLabel->size(),
            Qt::KeepAspectRatio,
            Qt::FastTransformation));

    m_previewLabel->setPixmap(preview);
}

void ReversePage::onCaptureStarted(const QString &devicePath,
                                    int width,
                                    int height,
                                    quint32 pixelFormat)
{
    m_statusLabel->setText(
        QStringLiteral("摄像头：%1\n分辨率：%2×%3  格式：%4")
            .arg(devicePath)
            .arg(width)
            .arg(height)
            .arg(fourccToString(pixelFormat)));
}

void ReversePage::onCaptureStopped()
{
    if (!m_reverseService->isMonitoring()) {
        m_running = false;
        updateRunningState(false);
    }
}

void ReversePage::onCameraError(const QString &message)
{
    m_statusLabel->setText(
        QStringLiteral("摄像头错误：%1").arg(message));
}

void ReversePage::onDistanceChanged(quint32 distanceMm,
                                    bool valid)
{
    if (!valid) {
        m_distanceLabel->setText(QStringLiteral("-- mm"));
        return;
    }

    const double distanceCm =
        static_cast<double>(distanceMm) / 10.0;

    m_distanceLabel->setText(
        QStringLiteral("%1 mm\n%2 cm")
            .arg(distanceMm)
            .arg(QString::number(distanceCm, 'f', 1)));
}

void ReversePage::onAlarmChanged(bool active)
{
    if (active) {
        m_alarmLabel->setText(QStringLiteral("危险：距离过近"));
        m_alarmLabel->setStyleSheet(
            "font-size: 24px;"
            "font-weight: bold;"
            "color: #dc3545;"
        );
    } else {
        m_alarmLabel->setText(QStringLiteral("安全"));
        m_alarmLabel->setStyleSheet(
            "font-size: 24px;"
            "font-weight: bold;"
            "color: #198754;"
        );
    }
}

void ReversePage::onReverseStatusChanged(const QString &message)
{
    m_statusLabel->setText(message);
}

void ReversePage::onReverseError(const QString &message)
{
    m_statusLabel->setText(
        QStringLiteral("倒车传感器错误：%1").arg(message));
}

QString ReversePage::fourccToString(quint32 format) const
{
    char text[5];

    text[0] = format & 0xff;
    text[1] = (format >> 8) & 0xff;
    text[2] = (format >> 16) & 0xff;
    text[3] = (format >> 24) & 0xff;
    text[4] = '\0';

    return QString::fromLatin1(text);
}
