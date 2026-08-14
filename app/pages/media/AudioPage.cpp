#include "AudioPage.h"

#include "MPlayerAudioPlayer.h"

#include <QDir>
#include <QFileInfo>
#include <QFileInfoList>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPixmap>
#include <QPushButton>
#include <QSlider>
#include <QVBoxLayout>
#include <QtGlobal>

AudioPage::AudioPage(const QString &musicDirectory, QWidget *parent)
    : QWidget(parent),
      m_musicDirectory(musicDirectory),
      m_durationMs(0),
      m_player(new MPlayerAudioPlayer(this)),
      m_songList(new QListWidget(this)),
      m_titleLabel(new QLabel(QStringLiteral("本地音乐"), this)),
      m_currentSongLabel(new QLabel(QStringLiteral("请选择歌曲"), this)),
      m_albumLabel(new QLabel(this)),
      m_positionLabel(new QLabel(QStringLiteral("00:00"), this)),
      m_durationLabel(new QLabel(QStringLiteral("00:00"), this)),
      m_statusLabel(new QLabel(QStringLiteral("就绪"), this)),
      m_durationSlider(new QSlider(Qt::Horizontal, this)),
      m_volumeSlider(new QSlider(Qt::Horizontal, this)),
      m_backButton(new QPushButton(QStringLiteral("返回"), this)),
      m_refreshButton(new QPushButton(QStringLiteral("刷新"), this)),
      m_previousButton(new QPushButton(QStringLiteral("上一曲"), this)),
      m_playButton(new QPushButton(QStringLiteral("播放"), this)),
      m_nextButton(new QPushButton(QStringLiteral("下一曲"), this)),
      m_stopButton(new QPushButton(QStringLiteral("停止"), this))
{
    setObjectName(QStringLiteral("audioPage"));
    setFixedSize(800, 480);

    m_backButton->setObjectName(QStringLiteral("audioBackButton"));
    m_refreshButton->setObjectName(QStringLiteral("audioRefreshButton"));
    m_previousButton->setObjectName(QStringLiteral("audioPreviousButton"));
    m_playButton->setObjectName(QStringLiteral("audioPlayButton"));
    m_nextButton->setObjectName(QStringLiteral("audioNextButton"));
    m_stopButton->setObjectName(QStringLiteral("audioStopButton"));

    m_titleLabel->setObjectName(QStringLiteral("audioTitleLabel"));
    m_currentSongLabel->setObjectName(QStringLiteral("currentSongLabel"));
    m_statusLabel->setObjectName(QStringLiteral("audioStatusLabel"));
    m_songList->setObjectName(QStringLiteral("audioSongList"));
    m_albumLabel->setObjectName(QStringLiteral("audioAlbumLabel"));
    m_durationSlider->setObjectName(QStringLiteral("audioDurationSlider"));
    m_volumeSlider->setObjectName(QStringLiteral("audioVolumeSlider"));

    m_backButton->setFixedSize(80, 40);
    m_refreshButton->setFixedSize(80, 40);

    m_titleLabel->setAlignment(Qt::AlignCenter);
    m_currentSongLabel->setAlignment(Qt::AlignCenter);
    m_statusLabel->setAlignment(Qt::AlignCenter);

    m_songList->setFixedWidth(310);
    m_songList->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_songList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    m_albumLabel->setFixedSize(220, 220);
    m_albumLabel->setAlignment(Qt::AlignCenter);
    m_albumLabel->setScaledContents(false);

    m_durationSlider->setRange(0, 0);

    m_volumeSlider->setRange(0, 100);
    m_volumeSlider->setValue(80);
    m_volumeSlider->setFixedWidth(150);

    QHBoxLayout *headerLayout = new QHBoxLayout;
    headerLayout->setContentsMargins(0, 0, 0, 0);
    headerLayout->addWidget(m_backButton);
    headerLayout->addStretch();
    headerLayout->addWidget(m_titleLabel);
    headerLayout->addStretch();
    headerLayout->addWidget(m_refreshButton);

    QVBoxLayout *leftLayout = new QVBoxLayout;
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->setSpacing(8);
    leftLayout->addWidget(m_songList);
    leftLayout->addWidget(m_statusLabel);

    QWidget *leftPanel = new QWidget(this);
    leftPanel->setObjectName(QStringLiteral("audioLeftPanel"));
    leftPanel->setLayout(leftLayout);

    QHBoxLayout *timeLayout = new QHBoxLayout;
    timeLayout->setContentsMargins(0, 0, 0, 0);
    timeLayout->addWidget(m_positionLabel);
    timeLayout->addStretch();
    timeLayout->addWidget(m_durationLabel);

    QHBoxLayout *controlLayout = new QHBoxLayout;
    controlLayout->setContentsMargins(0, 0, 0, 0);
    controlLayout->setSpacing(8);
    controlLayout->addWidget(m_previousButton);
    controlLayout->addWidget(m_playButton);
    controlLayout->addWidget(m_nextButton);
    controlLayout->addWidget(m_stopButton);

    QHBoxLayout *volumeLayout = new QHBoxLayout;
    volumeLayout->setContentsMargins(0, 0, 0, 0);
    volumeLayout->addWidget(new QLabel(QStringLiteral("音量"), this));
    volumeLayout->addWidget(m_volumeSlider);
    volumeLayout->addStretch();

    QVBoxLayout *rightLayout = new QVBoxLayout;
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(7);
    rightLayout->addWidget(m_currentSongLabel);
    rightLayout->addWidget(m_albumLabel, 0, Qt::AlignHCenter);
    rightLayout->addWidget(m_durationSlider);
    rightLayout->addLayout(timeLayout);
    rightLayout->addLayout(controlLayout);
    rightLayout->addLayout(volumeLayout);

    QWidget *rightPanel = new QWidget(this);
    rightPanel->setObjectName(QStringLiteral("audioRightPanel"));
    rightPanel->setLayout(rightLayout);

    QHBoxLayout *contentLayout = new QHBoxLayout;
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(14);
    contentLayout->addWidget(leftPanel);
    contentLayout->addWidget(rightPanel, 1);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(16, 12, 16, 12);
    mainLayout->setSpacing(10);
    mainLayout->addLayout(headerLayout);
    mainLayout->addLayout(contentLayout, 1);

    connect(m_backButton, &QPushButton::clicked,
            this, &AudioPage::backRequested);

    connect(m_refreshButton, &QPushButton::clicked,
            this, &AudioPage::scanSongs);

    connect(m_previousButton, &QPushButton::clicked,
            this, &AudioPage::playPreviousSong);

    connect(m_playButton, &QPushButton::clicked,
            this, &AudioPage::pauseOrResume);

    connect(m_nextButton, &QPushButton::clicked,
            this, &AudioPage::playNextSong);

    connect(m_stopButton, &QPushButton::clicked,
            this, &AudioPage::stopPlayback);

    connect(m_songList, &QListWidget::itemDoubleClicked,
            this, &AudioPage::playSelectedItem);

    connect(m_durationSlider, &QSlider::sliderReleased,
            this, &AudioPage::seekPlayback);

    connect(m_volumeSlider, &QSlider::valueChanged,
            this, &AudioPage::changeVolume);

    connect(m_player, &MPlayerAudioPlayer::playbackStarted,
            this, &AudioPage::onPlaybackStarted);

    connect(m_player, &MPlayerAudioPlayer::pausedChanged,
            this, &AudioPage::onPausedChanged);

    connect(m_player, &MPlayerAudioPlayer::playbackStopped,
            this, &AudioPage::onPlaybackStopped);

    connect(m_player, &MPlayerAudioPlayer::playbackFinished,
            this, &AudioPage::onPlaybackFinished);

    connect(m_player, &MPlayerAudioPlayer::positionChanged,
            this, &AudioPage::onPositionChanged);

    connect(m_player, &MPlayerAudioPlayer::durationChanged,
            this, &AudioPage::onDurationChanged);

    connect(m_player, &MPlayerAudioPlayer::errorOccurred,
            this, &AudioPage::onPlayerError);

    updateAlbumImage();
    scanSongs();
}

void AudioPage::scanSongs()
{
    m_songList->clear();

    QDir musicDir(m_musicDirectory);

    if (!musicDir.exists()) {
        m_statusLabel->setText(
            QStringLiteral("音乐目录不存在：%1").arg(m_musicDirectory));
        return;
    }

    QStringList filters;
    filters << QStringLiteral("*.mp3")
            << QStringLiteral("*.MP3")
            << QStringLiteral("*.wav")
            << QStringLiteral("*.WAV")
            << QStringLiteral("*.ogg")
            << QStringLiteral("*.flac")
            << QStringLiteral("*.aac")
            << QStringLiteral("*.m4a");

    const QFileInfoList files = musicDir.entryInfoList(
        filters, QDir::Files | QDir::Readable, QDir::Name);

    for (const QFileInfo &fileInfo : files) {
        QListWidgetItem *item =
            new QListWidgetItem(fileInfo.completeBaseName());

        item->setData(Qt::UserRole, fileInfo.absoluteFilePath());
        item->setToolTip(fileInfo.absoluteFilePath());
        m_songList->addItem(item);
    }

    if (m_songList->count() > 0)
        m_songList->setCurrentRow(0);

    m_statusLabel->setText(
        QStringLiteral("共找到 %1 首歌曲").arg(m_songList->count()));
}

QString AudioPage::selectedFilePath() const
{
    QListWidgetItem *item = m_songList->currentItem();

    if (!item)
        return QString();

    return item->data(Qt::UserRole).toString();
}

void AudioPage::playCurrentSong()
{
    QString filePath = selectedFilePath();

    if (filePath.isEmpty()) {
        m_statusLabel->setText(QStringLiteral("请先选择歌曲"));
        return;
    }

    m_player->play(filePath);
}

void AudioPage::playSelectedItem(QListWidgetItem *item)
{
    if (!item)
        return;

    const QString filePath = item->data(Qt::UserRole).toString();
    m_player->play(filePath);
}

void AudioPage::selectRow(int row)
{
    const int count = m_songList->count();

    if (count <= 0)
        return;

    if (row < 0)
        row = count - 1;

    if (row >= count)
        row = 0;

    m_songList->setCurrentRow(row);
    playCurrentSong();
}

void AudioPage::playPreviousSong()
{
    selectRow(m_songList->currentRow() - 1);
}

void AudioPage::playNextSong()
{
    selectRow(m_songList->currentRow() + 1);
}

void AudioPage::pauseOrResume()
{
    if (!m_player->isRunning()) {
        playCurrentSong();
        return;
    }

    m_player->pauseOrResume();
}

void AudioPage::stopPlayback()
{
    m_player->stop();
}

void AudioPage::seekPlayback()
{
    m_player->seekAbsolute(m_durationSlider->value());
}

void AudioPage::changeVolume(int volume)
{
    m_player->setVolume(volume);
}

void AudioPage::onPlaybackStarted(const QString &filePath)
{
    m_currentSongLabel->setText(QFileInfo(filePath).completeBaseName());
    m_statusLabel->setText(QStringLiteral("正在播放"));
    m_playButton->setText(QStringLiteral("暂停"));
}

void AudioPage::onPausedChanged(bool paused)
{
    if (!m_player->isRunning() && !paused)
        return;

    m_playButton->setText(
        paused ? QStringLiteral("继续") : QStringLiteral("暂停"));

    m_statusLabel->setText(
        paused ? QStringLiteral("已暂停") : QStringLiteral("正在播放"));
}

void AudioPage::onPlaybackStopped()
{
    m_statusLabel->setText(QStringLiteral("已停止"));
    m_playButton->setText(QStringLiteral("播放"));
    m_durationSlider->setValue(0);
    m_positionLabel->setText(QStringLiteral("00:00"));
}

void AudioPage::onPlaybackFinished()
{
    m_statusLabel->setText(QStringLiteral("播放完成"));
    playNextSong();
}

void AudioPage::onPositionChanged(qint64 positionMs)
{
    if (!m_durationSlider->isSliderDown())
        m_durationSlider->setValue(static_cast<int>(positionMs / 1000));

    m_positionLabel->setText(formatTime(positionMs));
}

void AudioPage::onDurationChanged(qint64 durationMs)
{
    m_durationMs = qMax<qint64>(0, durationMs);
    m_durationSlider->setRange(0, static_cast<int>(m_durationMs / 1000));
    m_durationLabel->setText(formatTime(m_durationMs));
}

void AudioPage::onPlayerError(const QString &message)
{
    m_statusLabel->setText(message);
    m_playButton->setText(QStringLiteral("播放"));
}

QString AudioPage::formatTime(qint64 milliseconds) const
{
    const qint64 totalSeconds = qMax<qint64>(0, milliseconds / 1000);
    const qint64 minutes = totalSeconds / 60;
    const qint64 seconds = totalSeconds % 60;

    return QStringLiteral("%1:%2")
        .arg(minutes, 2, 10, QLatin1Char('0'))
        .arg(seconds, 2, 10, QLatin1Char('0'));
}

void AudioPage::updateAlbumImage()
{
    QPixmap albumPixmap(QStringLiteral(":/icons/music.png"));

    if (albumPixmap.isNull()) {
        m_albumLabel->setText(QStringLiteral("MUSIC"));
        return;
    }

    m_albumLabel->setPixmap(
        albumPixmap.scaled(m_albumLabel->size(),Qt::KeepAspectRatio,
                    Qt::SmoothTransformation));
}