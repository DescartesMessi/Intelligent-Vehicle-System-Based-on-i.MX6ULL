#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>

class AudioPage;
class HomePage;
class QStackedWidget;
class VideoPage;
class BoardDevicePage;
class CameraPage;
class SentinelPage;
class ReversePage;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(const QString &musicDirectory,
                        const QString &videoDirectory,
                        const QString &photoDirectory,
                        QWidget *parent = nullptr);

private:
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