#include "CameraPage.h"

#include "V4l2CameraService.h"

#include <QComboBox>
#include <QDateTime>
#include <QDir>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QLabel>
#include <QPixmap>
#include <QPushButton>
#include <QVBoxLayout>

CameraPage::CameraPage(const QString &photoDirectory, QWidget *parent)
    : QWidget(parent),
      m_photoDirectory(photoDirectory),
      m_cameraService(new V4l2CameraService(this)),
      m_titleLabel(new QLabel(QStringLiteral("摄像头与拍照"), this)),
      m_previewLabel(new QLabel(QStringLiteral("等待摄像头画面"), this)),
      m_photoLabel(new QLabel(QStringLiteral("照片预览"), this)),
      m_statusLabel(new QLabel(QStringLiteral("就绪"), this)),
      m_deviceComboBox(new QComboBox(this)),
      m_backButton(new QPushButton(QStringLiteral("返回"), this)),
      m_refreshButton(new QPushButton(QStringLiteral("刷新设备"), this)),
      m_startButton(new QPushButton(QStringLiteral("开始"), this)),
      m_photoButton(new QPushButton(QStringLiteral("拍照"), this))
{
    setObjectName(QStringLiteral("cameraPage"));
    setFixedSize(800, 480);

    m_titleLabel->setObjectName(QStringLiteral("cameraTitleLabel"));
    m_previewLabel->setObjectName(QStringLiteral("cameraPreviewLabel"));
    m_photoLabel->setObjectName(QStringLiteral("cameraPhotoLabel"));
    m_statusLabel->setObjectName(QStringLiteral("cameraStatusLabel"));
    m_deviceComboBox->setObjectName(QStringLiteral("cameraDeviceComboBox"));
    m_backButton->setObjectName(QStringLiteral("cameraBackButton"));
    m_refreshButton->setObjectName(QStringLiteral("cameraRefreshButton"));
    m_startButton->setObjectName(QStringLiteral("cameraStartButton"));
    m_photoButton->setObjectName(QStringLiteral("cameraPhotoButton"));

    m_titleLabel->setAlignment(Qt::AlignCenter);
    m_statusLabel->setAlignment(Qt::AlignCenter);

    m_previewLabel->setMinimumSize(600, 380);
    m_previewLabel->setAlignment(Qt::AlignCenter);
    m_previewLabel->setScaledContents(false);

    m_photoLabel->setFixedSize(150, 112);
    m_photoLabel->setAlignment(Qt::AlignCenter);
    m_photoLabel->setScaledContents(false);

    m_deviceComboBox->setMinimumHeight(40);
    m_startButton->setMinimumHeight(44);
    m_photoButton->setMinimumHeight(44);
    m_refreshButton->setMinimumHeight(40);
    m_backButton->setFixedSize(76, 40);

    m_photoButton->setEnabled(false);

    QHBoxLayout *headerLayout = new QHBoxLayout;
    headerLayout->setContentsMargins(0, 0, 0, 0);
    headerLayout->addWidget(m_backButton);
    headerLayout->addStretch();
    headerLayout->addWidget(m_titleLabel);
    headerLayout->addStretch();
    headerLayout->addWidget(m_refreshButton);

    QVBoxLayout *controlLayout = new QVBoxLayout;
    controlLayout->setContentsMargins(0, 0, 0, 0);
    controlLayout->setSpacing(10);
    controlLayout->addWidget(m_photoLabel);
    controlLayout->addWidget(m_deviceComboBox);
    controlLayout->addWidget(m_startButton);
    controlLayout->addWidget(m_photoButton);
    controlLayout->addWidget(m_statusLabel);
    controlLayout->addStretch();

    QWidget *controlPanel = new QWidget(this);
    controlPanel->setObjectName(QStringLiteral("cameraControlPanel"));
    controlPanel->setFixedWidth(165);
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

    connect(m_backButton, &QPushButton::clicked,
            this, &CameraPage::returnToHome);

    connect(m_refreshButton, &QPushButton::clicked,
            this, &CameraPage::scanCameraDevices);

    connect(m_startButton, &QPushButton::clicked,
            this, &CameraPage::startOrStopCamera);

    connect(m_photoButton, &QPushButton::clicked,
            this, &CameraPage::capturePhoto);

    connect(m_cameraService, &V4l2CameraService::frameReady,
            this, &CameraPage::onFrameReady);

    connect(m_cameraService, &V4l2CameraService::captureStarted,
            this, &CameraPage::onCaptureStarted);

    connect(m_cameraService, &V4l2CameraService::captureStopped,
            this, &CameraPage::onCaptureStopped);

    connect(m_cameraService, &V4l2CameraService::errorOccurred,
            this, &CameraPage::onCameraError);

    scanCameraDevices();
}

CameraPage::~CameraPage()
{
    m_cameraService->stopCapture();
}

void CameraPage::scanCameraDevices()
{
    if (m_cameraService->isCapturing())
        m_cameraService->stopCapture();

    m_deviceComboBox->clear();

    for (int index = 0; index < 10; ++index) {
        const QString devicePath =
            QStringLiteral("/dev/video%1").arg(index);

        QFileInfo deviceInfo(devicePath);

        if (!deviceInfo.exists())
            continue;

        m_deviceComboBox->addItem(
            QStringLiteral("video%1").arg(index),
            devicePath);
    }

    const bool hasCamera = m_deviceComboBox->count() > 0;

    m_startButton->setEnabled(hasCamera);
    m_photoButton->setEnabled(false);

    if (hasCamera) {
        m_statusLabel->setText(
            QStringLiteral("检测到 %1 个视频设备")
                .arg(m_deviceComboBox->count()));
    } else {
        m_statusLabel->setText(QStringLiteral("未检测到摄像头"));
        m_previewLabel->setText(QStringLiteral("无摄像头设备"));
    }
}

void CameraPage::startOrStopCamera()
{
    if (m_cameraService->isCapturing()) {
        m_startButton->setEnabled(false);
        m_cameraService->stopCapture();
        return;
    }

    const QString devicePath =
        m_deviceComboBox->currentData().toString();

    if (devicePath.isEmpty()) {
        m_statusLabel->setText(QStringLiteral("请选择摄像头设备"));
        return;
    }

    m_startButton->setEnabled(false);
    m_statusLabel->setText(QStringLiteral("正在打开 %1").arg(devicePath));

    m_cameraService->startCapture(devicePath, 640, 480, 15);
}

void CameraPage::capturePhoto()
{
    if (m_currentFrame.isNull()) {
        m_statusLabel->setText(QStringLiteral("当前没有可保存的画面"));
        return;
    }

    QDir photoDir(m_photoDirectory);

    if (!photoDir.exists() && !photoDir.mkpath(QStringLiteral("."))) {
        m_statusLabel->setText(
            QStringLiteral("无法创建照片目录：%1").arg(m_photoDirectory));
        return;
    }

    const QString timestamp =
        QDateTime::currentDateTime().toString(
            QStringLiteral("yyyyMMdd_HHmmss_zzz"));

    const QString filePath =
        photoDir.filePath(
            QStringLiteral("photo_%1.bmp").arg(timestamp));

    if (!m_currentFrame.save(filePath, "BMP")) {
        m_statusLabel->setText(
            QStringLiteral("照片保存失败：%1").arg(filePath));
        return;
    }

    const QPixmap thumbnail = QPixmap::fromImage(m_currentFrame).scaled(
        m_photoLabel->size(),
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation);

    m_photoLabel->setPixmap(thumbnail);
    m_statusLabel->setText(
        QStringLiteral("照片已保存：%1").arg(filePath));
}

void CameraPage::returnToHome()
{
    m_cameraService->stopCapture();
    emit backRequested();
}

void CameraPage::onFrameReady(const QImage &image)
{
    if (image.isNull())
        return;

    m_currentFrame = image.copy();

    const QPixmap preview = QPixmap::fromImage(image).scaled(
        m_previewLabel->size(),
        Qt::KeepAspectRatio,
        Qt::FastTransformation);

    m_previewLabel->setPixmap(preview);
    m_photoButton->setEnabled(true);
}

void CameraPage::onCaptureStarted(const QString &devicePath,
                                  int width,
                                  int height,
                                  quint32 pixelFormat)
{
    m_startButton->setEnabled(true);
    m_startButton->setText(QStringLiteral("关闭"));
    m_deviceComboBox->setEnabled(false);
    m_refreshButton->setEnabled(false);

    m_statusLabel->setText(
        QStringLiteral("%1  %2×%3  %4")
            .arg(devicePath)
            .arg(width)
            .arg(height)
            .arg(fourccToString(pixelFormat)));
}

void CameraPage::onCaptureStopped()
{
    m_startButton->setEnabled(m_deviceComboBox->count() > 0);
    m_startButton->setText(QStringLiteral("开始"));
    m_deviceComboBox->setEnabled(true);
    m_refreshButton->setEnabled(true);
    m_photoButton->setEnabled(false);
}

void CameraPage::onCameraError(const QString &message)
{
    m_statusLabel->setText(message);
    m_startButton->setEnabled(m_deviceComboBox->count() > 0);
    m_startButton->setText(QStringLiteral("开始"));
    m_deviceComboBox->setEnabled(true);
    m_refreshButton->setEnabled(true);
    m_photoButton->setEnabled(false);
}

QString CameraPage::fourccToString(quint32 format) const
{
    char text[5];
    text[0] = format & 0xff;
    text[1] = (format >> 8) & 0xff;
    text[2] = (format >> 16) & 0xff;
    text[3] = (format >> 24) & 0xff;
    text[4] = '\0';

    return QString::fromLatin1(text);
}