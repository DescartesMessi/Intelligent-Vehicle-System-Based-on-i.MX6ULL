#include "MainWindow.h"

#include "pages/home/HomePage.h"
#include "pages/media/AudioPage.h"
#include "pages/video/VideoPage.h"

#include <QMessageBox>
#include <QStackedWidget>

MainWindow::MainWindow(const QString &musicDirectory,
                       const QString &videoDirectory,
                       QWidget *parent)
    : QMainWindow(parent),
      m_pages(new QStackedWidget(this)),
      m_homePage(new HomePage(this)),
      m_audioPage(new AudioPage(musicDirectory, this)),
      m_videoPage(new VideoPage(videoDirectory, this))
{
    setWindowFlags(Qt::FramelessWindowHint);
    setFixedSize(800, 480);

    m_pages->addWidget(m_homePage);
    m_pages->addWidget(m_audioPage);
    m_pages->addWidget(m_videoPage);
    m_pages->setCurrentWidget(m_homePage);

    setCentralWidget(m_pages);

    connect(m_homePage, &HomePage::audioRequested, this, [this]() {
        m_pages->setCurrentWidget(m_audioPage);
    });

    connect(m_homePage, &HomePage::videoRequested, this, [this]() {
        m_pages->setCurrentWidget(m_videoPage);
    });

    connect(m_audioPage, &AudioPage::backRequested, this, [this]() {
        m_pages->setCurrentWidget(m_homePage);
    });

    connect(m_videoPage, &VideoPage::backRequested, this, [this]() {
        m_pages->setCurrentWidget(m_homePage);
        m_homePage->repaint();
    });

    connect(m_homePage, &HomePage::diagnosticRequested, this, [this]() {
        QMessageBox::information(this, QStringLiteral("设备诊断"),
                                 QStringLiteral("设备诊断页面尚未实现"));
    });

    connect(m_homePage, &HomePage::sentinelRequested, this, [this]() {
        QMessageBox::information(this, QStringLiteral("哨兵监控"),
                                 QStringLiteral("哨兵页面尚未实现"));
    });

    connect(m_homePage, &HomePage::reverseRequested, this, [this]() {
        QMessageBox::information(this, QStringLiteral("倒车影像"),
                                 QStringLiteral("倒车页面尚未实现"));
    });
}