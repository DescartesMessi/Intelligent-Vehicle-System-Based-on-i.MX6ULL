TEMPLATE = app
TARGET = VehicleSystem

CONFIG += c++11 release
CONFIG -= debug

QT += core gui widgets

SOURCES += \
    app/main.cpp \
    app/MainWindow.cpp \
    app/pages/home/HomePage.cpp \
    app/pages/media/AudioPage.cpp \
    app/pages/video/VideoPage.cpp \
    services/audio/MPlayerAudioPlayer.cpp \
    services/video/MPlayerVideoPlayer.cpp

HEADERS += \
    app/MainWindow.h \
    app/pages/home/HomePage.h \
    app/pages/media/AudioPage.h \
    app/pages/video/VideoPage.h \
    services/audio/MPlayerAudioPlayer.h \
    services/video/MPlayerVideoPlayer.h

RESOURCES += \
    app/resources/app.qrc

INCLUDEPATH += \
    app \
    app/pages/home \
    app/pages/media \
    app/pages/video \
    services/audio \
    services/video

QMAKE_LFLAGS += -Wl,-rpath,/usr/local/qt5/lib
QMAKE_LFLAGS += -Wl,--enable-new-dtags

target.path = /usr/local/bin
INSTALLS += target