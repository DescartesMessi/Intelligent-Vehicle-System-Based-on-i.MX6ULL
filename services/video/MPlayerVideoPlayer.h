#ifndef MPLAYER_VIDEO_PLAYER_H
#define MPLAYER_VIDEO_PLAYER_H

#include <QByteArray>
#include <QObject>
#include <QProcess>
#include <QString>
#include <QTimer>

class MPlayerVideoPlayer : public QObject
{
    Q_OBJECT

public:
    explicit MPlayerVideoPlayer(QObject *parent = nullptr);
    ~MPlayerVideoPlayer();

    bool play(const QString &filePath);
    void pauseOrResume();
    void stop();
    void setVolume(int volume);
    void seekAbsolute(int seconds);
    void toggleFullscreen();

    bool isPlaying() const;
    bool isPaused() const;
    bool isFullscreen() const;

signals:
    void playbackStarted(const QString &filePath);
    void pausedChanged(bool paused);
    void playbackStopped();
    void playbackFinished();
    void fullscreenChanged(bool fullscreen);
    void positionChanged(qint64 positionMs);
    void durationChanged(qint64 durationMs);
    void errorOccurred(const QString &message);

private slots:
    void onProcessStarted();
    void onProcessOutput();
    void onProcessFinished(int exitCode, QProcess::ExitStatus status);
    void onProcessError(QProcess::ProcessError error);
    void queryPlaybackInformation();

private:
    void startMPlayer();
    bool loadFile(const QString &filePath);
    void sendCommand(const QString &command);
    QString escapeFilePath(const QString &filePath) const;

private:
    QProcess m_process;
    QTimer m_queryTimer;
    QByteArray m_outputBuffer;

    QString m_playerPath;
    QString m_pendingFile;
    QString m_currentFile;

    bool m_paused;
    bool m_hasMedia;
    bool m_fullscreen;
    bool m_ignoreNextEof;
    bool m_quitting;
    int m_volume;
};

#endif