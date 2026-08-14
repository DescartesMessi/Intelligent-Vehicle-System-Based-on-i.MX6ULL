#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>

class AudioPage;
class HomePage;
class QStackedWidget;
class VideoPage;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(const QString &musicDirectory,
            const QString &videoDirectory,
            QWidget *parent = nullptr);

private:
    QStackedWidget *m_pages;
    HomePage *m_homePage;
    AudioPage *m_audioPage;
    VideoPage *m_videoPage;
};

#endif