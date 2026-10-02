#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>

class AudioPage;
class BoardDevicePage;
class CameraPage;
class HomePage;
class QStackedWidget;
class ReversePage;
class SentinelPage;
class VideoPage;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(const QString &musicDirectory,const QString &videoDirectory,const QString &photoDirectory, QWidget *parent = nullptr);

private:
    void showAudioPage();
    void showVideoPage();
    void showBoardDevicePage();
    void showCameraPage();
    void showSentinelPage();
    void showReversePage();
    void handleExitRequested();

    void ensureAudioPage();
    void ensureVideoPage();
    void ensureBoardDevicePage();
    void ensureCameraPage();
    void ensureSentinelPage();
    void ensureReversePage();

private:
    QString m_musicDirectory;
    QString m_videoDirectory;
    QString m_photoDirectory;

    QStackedWidget *m_pages;
    HomePage *m_homePage;
    AudioPage *m_audioPage;
    VideoPage *m_videoPage;
    BoardDevicePage *m_boardDevicePage;
    CameraPage *m_cameraPage;
    SentinelPage *m_sentinelPage;
    ReversePage *m_reversePage;
};

#endif
