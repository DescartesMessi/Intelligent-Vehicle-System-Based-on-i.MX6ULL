#ifndef VIDEO_PAGE_H
#define VIDEO_PAGE_H

#include <QWidget>

class QLabel;
class QListWidget;
class QListWidgetItem;
class QPushButton;
class QSlider;

class MPlayerVideoPlayer;

class VideoPage : public QWidget
{
    Q_OBJECT

public:
    explicit VideoPage(const QString &videoDirectory,
                       QWidget *parent = nullptr);

signals:
    void backRequested();

private slots:
    void scanVideoFiles();
    void playCurrentVideo();
    void playSelectedItem(QListWidgetItem *item);
    void playNextVideo();
    void pauseOrResume();
    void stopPlayback();
    void seekPlayback();
    void decreaseVolume();
    void increaseVolume();
    void changeVolume(int volume);
    void toggleFullscreen();
    void returnToHome();

    void onPlaybackStarted(const QString &filePath);
    void onPausedChanged(bool paused);
    void onPlaybackStopped();
    void onPlaybackFinished();
    void onFullscreenChanged(bool fullscreen);
    void onPositionChanged(qint64 positionMs);
    void onDurationChanged(qint64 durationMs);
    void onPlayerError(const QString &message);

private:
    QString selectedFilePath() const;
    QString formatTime(qint64 milliseconds) const;
    void selectRow(int row);
    void restoreQtDisplay();

private:
    QString m_videoDirectory;
    qint64 m_durationMs;

    MPlayerVideoPlayer *m_player;

    QLabel *m_titleLabel;
    QLabel *m_videoSurface;
    QLabel *m_currentVideoLabel;
    QLabel *m_positionLabel;
    QLabel *m_durationLabel;
    QLabel *m_statusLabel;

    QListWidget *m_videoList;
    QSlider *m_durationSlider;
    QSlider *m_volumeSlider;

    QPushButton *m_backButton;
    QPushButton *m_refreshButton;
    QPushButton *m_playButton;
    QPushButton *m_nextButton;
    QPushButton *m_volumeDownButton;
    QPushButton *m_volumeUpButton;
    QPushButton *m_fullscreenButton;
    QPushButton *m_stopButton;
};

#endif