QT += core gui widgets

TEMPLATE = lib
CONFIG += plugin
CONFIG += c++20
TARGET = LightMain

LIBS += -luser32 -lshell32

win32 {
    CONFIG(debug, debug|release) {
        DESTDIR  = ../../src/debug/plugins/WidgetExplorer
    } else {
        DESTDIR  = ../../src/release/plugins/WidgetExplorer
    }
}
win32:CONFIG(release, debug|release): LIBS += -L../../src/release -lwecore
else:win32:CONFIG(debug, debug|release): LIBS += -L../../src/debug/ -lwecore
unix {
    target.path = $$[QT_INSTALL_PLUGINS]/generic
}
!isEmpty(target.path): INSTALLS += target

include(../../deps/WECore/WECore.pri)
INCLUDEPATH +=\
    ../../deps/
SOURCES +=  \
    aboutwindow.cpp \
    lightmain.cpp \
    mainwindow.cpp
HEADERS +=  \
    aboutwindow.h \
    lightmain.h \
    mainwindow.h
FORMS += \
    aboutwindow.ui \
    mainwindow.ui

