#ifndef AUDIO_PAGE_H
#define AUDIO_PAGE_H

#include <QWidget>

class QLabel;
class QListWidget;
class QListWidgetItem;
class QPushButton;
class QSlider;

class MPlayerAudioPlayer;

class AudioPage : public QWidget
{
    Q_OBJECT

public:
    explicit AudioPage(const QString &musicDirectory,QWidget *parent = nullptr);

signals:
    void backRequested();

private slots:
    void scanSongs();
    void playCurrentSong();
    void playSelectedItem(QListWidgetItem *item);
    void playPreviousSong();
    void playNextSong();
    void pauseOrResume();
    void stopPlayback();
    void seekPlayback();
    void changeVolume(int volume);

    void onPlaybackStarted(const QString &filePath);
    void onPausedChanged(bool paused);
    void onPlaybackStopped();
    void onPlaybackFinished();
    void onPositionChanged(qint64 positionMs);
    void onDurationChanged(qint64 durationMs);
    void onPlayerError(const QString &message);

private:
    QString selectedFilePath() const;
    QString formatTime(qint64 milliseconds) const;
    void selectRow(int row);
    void updateAlbumImage();

private:
    QString m_musicDirectory;
    qint64 m_durationMs;

    MPlayerAudioPlayer *m_player;

    QListWidget *m_songList;
    QLabel *m_titleLabel;
    QLabel *m_currentSongLabel;
    QLabel *m_albumLabel;
    QLabel *m_positionLabel;
    QLabel *m_durationLabel;
    QLabel *m_statusLabel;

    QSlider *m_durationSlider;
    QSlider *m_volumeSlider;

    QPushButton *m_backButton;
    QPushButton *m_refreshButton;
    QPushButton *m_previousButton;
    QPushButton *m_playButton;
    QPushButton *m_nextButton;
    QPushButton *m_stopButton;
};

#endif