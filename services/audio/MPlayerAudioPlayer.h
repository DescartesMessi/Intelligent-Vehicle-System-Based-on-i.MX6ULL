#ifndef MPLAYER_AUDIO_PLAYER_H
#define MPLAYER_AUDIO_PLAYER_H

#include <QByteArray>
#include <QObject>
#include <QProcess>
#include <QString>
#include <QTimer>

class MPlayerAudioPlayer : public QObject
{
    Q_OBJECT

public:
    explicit MPlayerAudioPlayer(QObject *parent = nullptr);
    ~MPlayerAudioPlayer();

    bool play(const QString &filePath);
    void pauseOrResume();
    void stop();
    void setVolume(int volume);
    void seekAbsolute(int seconds);

    bool isRunning() const;
    bool isPaused() const;

signals:
    void playbackStarted(const QString &filePath);
    void pausedChanged(bool paused);
    void playbackStopped();
    void playbackFinished();
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
    bool m_ignoreNextEof;
    bool m_quitting;
    int m_volume;
};

#endif