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
    services/video/MPlayerVideoPlayer.cpp \
    services/sensors/BoardDeviceService.cpp \
    app/pages/board/BoardDevicePage.cpp \
    app/pages/camera/CameraPage.cpp \
    services/camera/V4l2CameraService.cpp \
    services/sentinel/MjpgStreamerService.cpp \
    services/sentinel/SentinelService.cpp \
    app/pages/sentinel/SentinelPage.cpp \
    services/reverse/ReverseService.cpp \
    app/pages/reverse/ReversePage.cpp

HEADERS += \
    app/MetricsProbe.h \
    app/MainWindow.h \
    app/pages/home/HomePage.h \
    app/pages/media/AudioPage.h \
    app/pages/video/VideoPage.h \
    services/audio/MPlayerAudioPlayer.h \
    services/video/MPlayerVideoPlayer.h \
    services/sensors/BoardDeviceService.h \
    app/pages/board/BoardDevicePage.h \
    app/pages/camera/CameraPage.h \
    services/camera/V4l2CameraService.h \
    services/sentinel/MjpgStreamerService.h \
    services/sentinel/SentinelService.h \
    app/pages/sentinel/SentinelPage.h \
    services/reverse/ReverseService.h \
    app/pages/reverse/ReversePage.h

RESOURCES += \
    app/resources/app.qrc

INCLUDEPATH += \
    app \
    app/pages/home \
    app/pages/media \
    app/pages/video \
    services/audio \
    services/video \
    services/sensors \
    include/uapi \
    app/pages/board \
    app/pages/camera \
    services/camera \
    services/sentinel \
    app/pages/sentinel \
    services/reverse \
    app/pages/reverse

QMAKE_LFLAGS += -Wl,-rpath,/usr/local/qt5/lib
QMAKE_LFLAGS += -Wl,--enable-new-dtags

target.path = /usr/local/bin
INSTALLS += target
