TEMPLATE = app
TARGET = openhbbtvbrowser
QT += webenginewidgets
CONFIG += c++11 link_pkgconfig
PKGCONFIG += gstreamer-1.0

HEADERS += \
    browsercontrol.h \
    browserwindow.h \
    gstplayer.h \
    webpage.h \
    webview.h

SOURCES += \
    main.cpp \
    browsercontrol.cpp \
    browserwindow.cpp \
    gstplayer.cpp \
    webpage.cpp \
    webview.cpp

RESOURCES += openhbbtvbrowser.qrc
