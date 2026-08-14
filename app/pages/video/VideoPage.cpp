#include "VideoPage.h"

#include "MPlayerVideoPlayer.h"

#include <QDir>
#include <QFileInfo>
#include <QFileInfoList>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPushButton>
#include <QSlider>
#include <QTimer>
#include <QVBoxLayout>
#include <QtGlobal>

VideoPage::VideoPage(const QString &videoDirectory, QWidget *parent)
    : QWidget(parent),
      m_videoDirectory(videoDirectory),
      m_durationMs(0),
      m_player(new MPlayerVideoPlayer(this)),
      m_titleLabel(new QLabel(QStringLiteral("本地视频"), this)),
      m_videoSurface(new QLabel(QStringLiteral("请选择视频"), this)),
      m_currentVideoLabel(new QLabel(QStringLiteral("未选择视频"), this)),
      m_positionLabel(new QLabel(QStringLiteral("00:00"), this)),
      m_durationLabel(new QLabel(QStringLiteral("/ 00:00"), this)),
      m_statusLabel(new QLabel(QStringLiteral("就绪"), this)),
      m_videoList(new QListWidget(this)),
      m_durationSlider(new QSlider(Qt::Horizontal, this)),
      m_volumeSlider(new QSlider(Qt::Horizontal, this)),
      m_backButton(new QPushButton(QStringLiteral("返回"), this)),
      m_refreshButton(new QPushButton(QStringLiteral("刷新"), this)),
      m_playButton(new QPushButton(this)),
      m_nextButton(new QPushButton(this)),
      m_volumeDownButton(new QPushButton(this)),
      m_volumeUpButton(new QPushButton(this)),
      m_fullscreenButton(new QPushButton(this)),
      m_stopButton(new QPushButton(QStringLiteral("停止"), this))
{
    setObjectName(QStringLiteral("videoPage"));
    setFixedSize(800, 480);

    m_titleLabel->setObjectName(QStringLiteral("videoTitleLabel"));
    m_videoSurface->setObjectName(QStringLiteral("videoSurface"));
    m_currentVideoLabel->setObjectName(QStringLiteral("currentVideoLabel"));
    m_statusLabel->setObjectName(QStringLiteral("videoStatusLabel"));
    m_videoList->setObjectName(QStringLiteral("videoList"));
    m_durationSlider->setObjectName(QStringLiteral("videoDurationSlider"));
    m_volumeSlider->setObjectName(QStringLiteral("videoVolumeSlider"));

    m_backButton->setObjectName(QStringLiteral("videoBackButton"));
    m_refreshButton->setObjectName(QStringLiteral("videoRefreshButton"));
    m_playButton->setObjectName(QStringLiteral("videoPlayButton"));
    m_nextButton->setObjectName(QStringLiteral("videoNextButton"));
    m_volumeDownButton->setObjectName(QStringLiteral("videoVolumeDownButton"));
    m_volumeUpButton->setObjectName(QStringLiteral("videoVolumeUpButton"));
    m_fullscreenButton->setObjectName(QStringLiteral("videoFullscreenButton"));
    m_stopButton->setObjectName(QStringLiteral("videoStopButton"));

    m_backButton->setFixedSize(72, 38);
    m_refreshButton->setFixedSize(72, 38);

    m_playButton->setIcon(QIcon(QStringLiteral(":/video/play.png")));
    m_nextButton->setIcon(QIcon(QStringLiteral(":/video/next.png")));
    m_volumeDownButton->setIcon(QIcon(QStringLiteral(":/video/volume-down.png")));
    m_volumeUpButton->setIcon(QIcon(QStringLiteral(":/video/volume-up.png")));
    m_fullscreenButton->setIcon(QIcon(QStringLiteral(":/video/fullscreen.png")));

    const QSize controlIconSize(30, 30);
    m_playButton->setIconSize(controlIconSize);
    m_nextButton->setIconSize(controlIconSize);
    m_volumeDownButton->setIconSize(controlIconSize);
    m_volumeUpButton->setIconSize(controlIconSize);
    m_fullscreenButton->setIconSize(controlIconSize);

    m_playButton->setFixedSize(44, 44);
    m_nextButton->setFixedSize(44, 44);
    m_volumeDownButton->setFixedSize(44, 44);
    m_volumeUpButton->setFixedSize(44, 44);
    m_fullscreenButton->setFixedSize(44, 44);
    m_stopButton->setFixedSize(58, 44);

    m_titleLabel->setAlignment(Qt::AlignCenter);
    m_currentVideoLabel->setAlignment(Qt::AlignCenter);
    m_statusLabel->setAlignment(Qt::AlignCenter);

    m_videoSurface->setFixedSize(520, 330);
    m_videoSurface->setAlignment(Qt::AlignCenter);

    m_videoList->setFixedWidth(250);
    m_videoList->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_videoList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    m_durationSlider->setRange(0, 0);

    m_volumeSlider->setRange(0, 100);
    m_volumeSlider->setValue(70);
    m_volumeSlider->setFixedWidth(90);

    QHBoxLayout *headerLayout = new QHBoxLayout;
    headerLayout->setContentsMargins(0, 0, 0, 0);
    headerLayout->addWidget(m_backButton);
    headerLayout->addStretch();
    headerLayout->addWidget(m_titleLabel);
    headerLayout->addStretch();
    headerLayout->addWidget(m_refreshButton);

    QVBoxLayout *leftLayout = new QVBoxLayout;
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->setSpacing(4);
    leftLayout->addWidget(m_currentVideoLabel);
    leftLayout->addWidget(m_videoSurface);

    QVBoxLayout *rightLayout = new QVBoxLayout;
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(5);
    rightLayout->addWidget(m_videoList);
    rightLayout->addWidget(m_statusLabel);

    QWidget *rightPanel = new QWidget(this);
    rightPanel->setObjectName(QStringLiteral("videoRightPanel"));
    rightPanel->setLayout(rightLayout);

    QHBoxLayout *contentLayout = new QHBoxLayout;
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(10);
    contentLayout->addLayout(leftLayout);
    contentLayout->addWidget(rightPanel);

    QHBoxLayout *timeLayout = new QHBoxLayout;
    timeLayout->setContentsMargins(0, 0, 0, 0);
    timeLayout->addWidget(m_positionLabel);
    timeLayout->addWidget(m_durationLabel);
    timeLayout->addStretch();

    QHBoxLayout *controlLayout = new QHBoxLayout;
    controlLayout->setContentsMargins(0, 0, 0, 0);
    controlLayout->setSpacing(8);
    controlLayout->addWidget(m_playButton);
    controlLayout->addWidget(m_nextButton);
    controlLayout->addWidget(m_stopButton);
    controlLayout->addSpacing(8);
    controlLayout->addWidget(m_volumeDownButton);
    controlLayout->addWidget(m_volumeSlider);
    controlLayout->addWidget(m_volumeUpButton);
    controlLayout->addLayout(timeLayout);
    controlLayout->addWidget(m_fullscreenButton);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 6, 10, 8);
    mainLayout->setSpacing(5);
    mainLayout->addLayout(headerLayout);
    mainLayout->addLayout(contentLayout);
    mainLayout->addWidget(m_durationSlider);
    mainLayout->addLayout(controlLayout);

    connect(m_backButton, &QPushButton::clicked,
            this, &VideoPage::returnToHome);

    connect(m_refreshButton, &QPushButton::clicked,
            this, &VideoPage::scanVideoFiles);

    connect(m_playButton, &QPushButton::clicked,
            this, &VideoPage::pauseOrResume);

    connect(m_nextButton, &QPushButton::clicked,
            this, &VideoPage::playNextVideo);

    connect(m_stopButton, &QPushButton::clicked,
            this, &VideoPage::stopPlayback);

    connect(m_volumeDownButton, &QPushButton::clicked,
            this, &VideoPage::decreaseVolume);

    connect(m_volumeUpButton, &QPushButton::clicked,
            this, &VideoPage::increaseVolume);

    connect(m_fullscreenButton, &QPushButton::clicked,
            this, &VideoPage::toggleFullscreen);

    connect(m_volumeSlider, &QSlider::valueChanged,
            this, &VideoPage::changeVolume);

    connect(m_durationSlider, &QSlider::sliderReleased,
            this, &VideoPage::seekPlayback);

    connect(m_videoList, &QListWidget::itemDoubleClicked,
            this, &VideoPage::playSelectedItem);

    connect(m_player, &MPlayerVideoPlayer::playbackStarted,
            this, &VideoPage::onPlaybackStarted);

    connect(m_player, &MPlayerVideoPlayer::pausedChanged,
            this, &VideoPage::onPausedChanged);

    connect(m_player, &MPlayerVideoPlayer::playbackStopped,
            this, &VideoPage::onPlaybackStopped);

    connect(m_player, &MPlayerVideoPlayer::playbackFinished,
            this, &VideoPage::onPlaybackFinished);

    connect(m_player, &MPlayerVideoPlayer::fullscreenChanged,
            this, &VideoPage::onFullscreenChanged);

    connect(m_player, &MPlayerVideoPlayer::positionChanged,
            this, &VideoPage::onPositionChanged);

    connect(m_player, &MPlayerVideoPlayer::durationChanged,
            this, &VideoPage::onDurationChanged);

    connect(m_player, &MPlayerVideoPlayer::errorOccurred,
            this, &VideoPage::onPlayerError);

    scanVideoFiles();
}

void VideoPage::scanVideoFiles()
{
    m_videoList->clear();

    QDir videoDir(m_videoDirectory);

    if (!videoDir.exists()) {
        m_statusLabel->setText(
            QStringLiteral("视频目录不存在：%1").arg(m_videoDirectory));
        return;
    }

    QStringList filters;
    filters << QStringLiteral("*.mp4")
            << QStringLiteral("*.MP4")
            << QStringLiteral("*.mkv")
            << QStringLiteral("*.MKV")
            << QStringLiteral("*.avi")
            << QStringLiteral("*.AVI")
            << QStringLiteral("*.wmv")
            << QStringLiteral("*.mov");

    const QFileInfoList files = videoDir.entryInfoList(
        filters, QDir::Files | QDir::Readable, QDir::Name);

    for (const QFileInfo &fileInfo : files) {
        QListWidgetItem *item = new QListWidgetItem(fileInfo.fileName());
        item->setData(Qt::UserRole, fileInfo.absoluteFilePath());
        item->setToolTip(fileInfo.absoluteFilePath());
        m_videoList->addItem(item);
    }

    if (m_videoList->count() > 0)
        m_videoList->setCurrentRow(0);

    m_statusLabel->setText(
        QStringLiteral("共找到 %1 个视频").arg(m_videoList->count()));
}

QString VideoPage::selectedFilePath() const
{
    QListWidgetItem *item = m_videoList->currentItem();

    if (!item)
        return QString();

    return item->data(Qt::UserRole).toString();
}

void VideoPage::playCurrentVideo()
{
    const QString filePath = selectedFilePath();

    if (filePath.isEmpty()) {
        m_statusLabel->setText(QStringLiteral("请先选择视频"));
        return;
    }

    m_player->play(filePath);
}

void VideoPage::playSelectedItem(QListWidgetItem *item)
{
    if (!item)
        return;

    m_player->play(item->data(Qt::UserRole).toString());
}

void VideoPage::selectRow(int row)
{
    const int count = m_videoList->count();

    if (count <= 0)
        return;

    if (row < 0)
        row = count - 1;

    if (row >= count)
        row = 0;

    m_videoList->setCurrentRow(row);
    playCurrentVideo();
}

void VideoPage::playNextVideo()
{
    selectRow(m_videoList->currentRow() + 1);
}

void VideoPage::pauseOrResume()
{
    if (!m_player->isPlaying()) {
        playCurrentVideo();
        return;
    }

    m_player->pauseOrResume();
}

void VideoPage::stopPlayback()
{
    m_player->stop();
    restoreQtDisplay();
}

void VideoPage::seekPlayback()
{
    m_player->seekAbsolute(m_durationSlider->value());
}

void VideoPage::decreaseVolume()
{
    m_volumeSlider->setValue(m_volumeSlider->value() - 5);
}

void VideoPage::increaseVolume()
{
    m_volumeSlider->setValue(m_volumeSlider->value() + 5);
}

void VideoPage::changeVolume(int volume)
{
    m_player->setVolume(volume);
}

void VideoPage::toggleFullscreen()
{
    m_player->toggleFullscreen();
}

void VideoPage::returnToHome()
{
    m_player->stop();
    restoreQtDisplay();
    emit backRequested();
}

void VideoPage::onPlaybackStarted(const QString &filePath)
{
    m_currentVideoLabel->setText(QFileInfo(filePath).fileName());
    m_statusLabel->setText(QStringLiteral("正在播放"));
    m_playButton->setIcon(QIcon(QStringLiteral(":/video/pause.png")));
}

void VideoPage::onPausedChanged(bool paused)
{
    const QString iconPath = paused
        ? QStringLiteral(":/video/play.png")
        : QStringLiteral(":/video/pause.png");

    m_playButton->setIcon(QIcon(iconPath));

    m_statusLabel->setText(
        paused ? QStringLiteral("已暂停") : QStringLiteral("正在播放"));
}

void VideoPage::onPlaybackStopped()
{
    m_statusLabel->setText(QStringLiteral("已停止"));
    m_playButton->setIcon(QIcon(QStringLiteral(":/video/play.png")));
    m_durationSlider->setValue(0);
    m_positionLabel->setText(QStringLiteral("00:00"));
    restoreQtDisplay();
}

void VideoPage::onPlaybackFinished()
{
    m_statusLabel->setText(QStringLiteral("播放完成"));
    restoreQtDisplay();
    playNextVideo();
}

void VideoPage::onFullscreenChanged(bool fullscreen)
{
    const QString iconPath = fullscreen
        ? QStringLiteral(":/video/windowed.png")
        : QStringLiteral(":/video/fullscreen.png");

    m_fullscreenButton->setIcon(QIcon(iconPath));
}

void VideoPage::onPositionChanged(qint64 positionMs)
{
    if (!m_durationSlider->isSliderDown())
        m_durationSlider->setValue(static_cast<int>(positionMs / 1000));

    m_positionLabel->setText(formatTime(positionMs));
}

void VideoPage::onDurationChanged(qint64 durationMs)
{
    m_durationMs = qMax<qint64>(0, durationMs);
    m_durationSlider->setRange(0, static_cast<int>(m_durationMs / 1000));
    m_durationLabel->setText(QStringLiteral("/ %1").arg(formatTime(m_durationMs)));
}

void VideoPage::onPlayerError(const QString &message)
{
    m_statusLabel->setText(message);
    m_playButton->setIcon(QIcon(QStringLiteral(":/video/play.png")));
    restoreQtDisplay();
}

QString VideoPage::formatTime(qint64 milliseconds) const
{
    const qint64 totalSeconds = qMax<qint64>(0, milliseconds / 1000);
    const qint64 minutes = totalSeconds / 60;
    const qint64 seconds = totalSeconds % 60;

    return QStringLiteral("%1:%2")
        .arg(minutes, 2, 10, QLatin1Char('0'))
        .arg(seconds, 2, 10, QLatin1Char('0'));
}

void VideoPage::restoreQtDisplay()
{
    QTimer::singleShot(100, this, [this]() {
        m_videoSurface->repaint();
        m_videoList->repaint();
        repaint();
    });
}