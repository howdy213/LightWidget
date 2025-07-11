QT       -= gui

TARGET = widgetexplorersdk
TEMPLATE = lib

DEFINES += WIDGETEXPLORERSDK_LIBRARY

LIBS += -luser32
LIBS+=-lshell32
win32 {
    CONFIG(debug, debug|release) {
        DESTDIR  = ../LightWidget/debug
    } else {
        DESTDIR  = ../LightWidget/release
    }
}

#include(../Shared/Shared.pri)
# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
include(WidgetExplorerSDKInternal.pri)
HEADERS += \
    WDef/colordef.h \
    WDef/wedef.h \
    WE/we.h \
    wconfig.h \
    wdef.h \
    wfile.h \
    wplugin.h \

SOURCES += \
    WE/we.cpp \
