#include "MainWindow.h"

#include "pages/camera/CameraPage.h"
#include "pages/home/HomePage.h"
#include "pages/media/AudioPage.h"
#include "pages/video/VideoPage.h"
#include "pages/board/BoardDevicePage.h"
#include "pages/sentinel/SentinelPage.h"
#include "pages/reverse/ReversePage.h"

#include <QMessageBox>
#include <QStackedWidget>

MainWindow::MainWindow(const QString &musicDirectory,
                    const QString &videoDirectory,
                    const QString &photoDirectory,
                       QWidget *parent)
    : QMainWindow(parent),
      m_pages(new QStackedWidget(this)),
      m_homePage(new HomePage(this)),
      m_audioPage(new AudioPage(musicDirectory, this)),
      m_videoPage(new VideoPage(videoDirectory, this)),
      m_boardDevicePage(new BoardDevicePage(this)),
      m_cameraPage(new CameraPage(photoDirectory, this)),
      m_sentinelPage(new SentinelPage(this)),
      m_reversePage(new ReversePage(QStringLiteral("/photo"), this))
{
    setWindowFlags(Qt::FramelessWindowHint);
    setFixedSize(800, 480);

    m_pages->addWidget(m_homePage);
    m_pages->addWidget(m_audioPage);
    m_pages->addWidget(m_videoPage);
    m_pages->addWidget(m_boardDevicePage);
    m_pages->addWidget(m_cameraPage);
    m_pages->addWidget(m_sentinelPage);
    m_pages->addWidget(m_reversePage);


    setCentralWidget(m_pages);

    connect(m_homePage, &HomePage::audioRequested, this, [this]() {
        m_pages->setCurrentWidget(m_audioPage);});

    connect(m_homePage, &HomePage::videoRequested, this, [this]() {
        m_pages->setCurrentWidget(m_videoPage);});

    connect(m_homePage, &HomePage::diagnosticRequested, this, [this]() {
        m_pages->setCurrentWidget(m_boardDevicePage);});

    connect(m_homePage,&HomePage::cameraRequested,this,[this]() {
        m_pages->setCurrentWidget(m_cameraPage);});


    connect(m_audioPage, &AudioPage::backRequested, this, [this]() {
        m_pages->setCurrentWidget(m_homePage);});

    connect(m_videoPage, &VideoPage::backRequested, this, [this]() {
        m_pages->setCurrentWidget(m_homePage);
        m_homePage->repaint();});
    
    connect(m_cameraPage, &CameraPage::backRequested, this, [this]() {
        m_pages->setCurrentWidget(m_homePage);
    });

    connect(m_boardDevicePage,&BoardDevicePage::backRequested,this,[this]() {
                m_pages->setCurrentWidget(m_homePage);
                m_homePage->update();
                m_homePage->repaint();
            });

    connect(m_homePage,&HomePage::sentinelRequested,this,[this]() {
        m_pages->setCurrentWidget(m_sentinelPage);
    });

    connect(m_sentinelPage,&SentinelPage::backRequested,this,[this]() {
        m_pages->setCurrentWidget(m_homePage);
    });


    connect(m_reversePage,&ReversePage::backRequested,this,[this]() {
                m_pages->setCurrentWidget(m_homePage);
            });

    connect(m_homePage, &HomePage::reverseRequested, this,[this]() {
                m_pages->setCurrentWidget(m_reversePage);
            });



}