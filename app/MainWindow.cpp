#include "MainWindow.h"

#include "pages/board/BoardDevicePage.h"
#include "pages/camera/CameraPage.h"
#include "pages/home/HomePage.h"
#include "pages/media/AudioPage.h"
#include "pages/reverse/ReversePage.h"
#include "pages/sentinel/SentinelPage.h"
#include "pages/video/VideoPage.h"

#include <QStackedWidget>
#include <QApplication>
#include <QMessageBox>
#include <QPushButton>

MainWindow::MainWindow(const QString &musicDirectory,
                       const QString &videoDirectory,
                       const QString &photoDirectory,
                       QWidget *parent)
    : QMainWindow(parent),
      m_musicDirectory(musicDirectory),
      m_videoDirectory(videoDirectory),
      m_photoDirectory(photoDirectory),
      m_pages(new QStackedWidget(this)),
      m_homePage(new HomePage(this)),
      m_audioPage(nullptr),
      m_videoPage(nullptr),
      m_boardDevicePage(nullptr),
      m_cameraPage(nullptr),
      m_sentinelPage(nullptr),
      m_reversePage(nullptr)
{
    /* 设置无边框窗口 */
    setWindowFlags(Qt::FramelessWindowHint);

    /* 固定 LCD 分辨率 */
    setFixedSize(800, 480);

    /* 启动时只创建首页 */
    m_pages->addWidget(m_homePage);
    m_pages->setCurrentWidget(m_homePage);

    setCentralWidget(m_pages);

    connect(m_homePage, &HomePage::audioRequested, this, &MainWindow::showAudioPage);

    connect(m_homePage, &HomePage::videoRequested, this, &MainWindow::showVideoPage);

    connect(m_homePage, &HomePage::diagnosticRequested, this, &MainWindow::showBoardDevicePage);

    connect(m_homePage, &HomePage::cameraRequested, this, &MainWindow::showCameraPage);

    connect(m_homePage, &HomePage::sentinelRequested, this, &MainWindow::showSentinelPage);

    connect(m_homePage, &HomePage::reverseRequested, this, &MainWindow::showReversePage);

    connect(m_homePage, &HomePage::exitRequested, this, &MainWindow::handleExitRequested);
}

/*
 * 首页"退出"按钮的处理：先弹一次确认框再退出。
 *
 * 触摸屏上误触一次就直接杀掉界面会让演示中断，且本程序退出后不会自动
 * 拉起，因此这里保留一次确认；默认焦点给"取消"，避免回车直接退出。
 */
void MainWindow::handleExitRequested()
{
    QMessageBox box(this);

    box.setWindowTitle(QStringLiteral("退出程序"));
    box.setIcon(QMessageBox::Question);
    box.setText(QStringLiteral("确定要退出车载终端程序吗？"));
    box.setInformativeText(QStringLiteral("退出后界面会关闭，需要重新运行才能恢复。"));

    QPushButton *confirmButton =
        box.addButton(QStringLiteral("退出"), QMessageBox::AcceptRole);
    QPushButton *cancelButton =
        box.addButton(QStringLiteral("取消"), QMessageBox::RejectRole);

    box.setDefaultButton(cancelButton);
    box.exec();

    if (box.clickedButton() == confirmButton)
        qApp->quit();
}

void MainWindow::ensureAudioPage()
{
    if (m_audioPage)
        return;

    /* 用户第一次进入音乐页面时才创建音乐页面 */
    m_audioPage = new AudioPage(m_musicDirectory, this);
    m_pages->addWidget(m_audioPage);

    connect(m_audioPage, &AudioPage::backRequested, this, [this]() {
        m_pages->setCurrentWidget(m_homePage);
    });
}

void MainWindow::ensureVideoPage()
{
    if (m_videoPage)
        return;

    /* 用户第一次进入视频页面时才创建视频页面 */
    m_videoPage = new VideoPage(m_videoDirectory, this);
    m_pages->addWidget(m_videoPage);

    connect(m_videoPage, &VideoPage::backRequested, this, [this]() {
        m_pages->setCurrentWidget(m_homePage);
        m_homePage->repaint();
    });
}

void MainWindow::ensureBoardDevicePage()
{
    if (m_boardDevicePage)
        return;

    /* 用户第一次进入板载设备页面时才创建传感器页面 */
    m_boardDevicePage = new BoardDevicePage(this);
    m_pages->addWidget(m_boardDevicePage);

    connect(m_boardDevicePage, &BoardDevicePage::backRequested, this, [this]() {
        m_pages->setCurrentWidget(m_homePage);
        m_homePage->update();
        m_homePage->repaint();
    });
}

void MainWindow::ensureCameraPage()
{
    if (m_cameraPage)
        return;

    /* 用户第一次进入摄像头页面时才创建摄像头服务 */
    m_cameraPage = new CameraPage(m_photoDirectory, this);
    m_pages->addWidget(m_cameraPage);

    connect(m_cameraPage, &CameraPage::backRequested, this, [this]() {
        m_pages->setCurrentWidget(m_homePage);
    });
}

void MainWindow::ensureSentinelPage()
{
    if (m_sentinelPage)
        return;

    /* 用户第一次进入哨兵页面时才创建 MJPG-streamer 服务 */
    m_sentinelPage = new SentinelPage(this);
    m_pages->addWidget(m_sentinelPage);

    connect(m_sentinelPage, &SentinelPage::backRequested, this, [this]() {
        m_pages->setCurrentWidget(m_homePage);
    });
}

void MainWindow::ensureReversePage()
{
    if (m_reversePage)
        return;

    /* 倒车页面使用照片目录保存倒车抓拍图片 */
    m_reversePage = new ReversePage(m_photoDirectory, this);

    /* 将倒车页面加入页面栈 */
    m_pages->addWidget(m_reversePage);

    /* 倒车页面返回后切换到首页 */
    connect(m_reversePage, &ReversePage::backRequested, this, [this]() {
        m_pages->setCurrentWidget(m_homePage);
    });
}

void MainWindow::showAudioPage()
{
    ensureAudioPage();
    m_pages->setCurrentWidget(m_audioPage);
}

void MainWindow::showVideoPage()
{
    ensureVideoPage();
    m_pages->setCurrentWidget(m_videoPage);
}

void MainWindow::showBoardDevicePage()
{
    ensureBoardDevicePage();
    m_pages->setCurrentWidget(m_boardDevicePage);
}

void MainWindow::showCameraPage()
{
    ensureCameraPage();
    m_pages->setCurrentWidget(m_cameraPage);
}

void MainWindow::showSentinelPage()
{
    ensureSentinelPage();
    m_pages->setCurrentWidget(m_sentinelPage);
}

void MainWindow::showReversePage()
{
    ensureReversePage();
    m_pages->setCurrentWidget(m_reversePage);
}
