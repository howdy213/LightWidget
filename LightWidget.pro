QT += core gui widgets
QT += statemachine
CONFIG += c++20

TEMPLATE = lib
CONFIG += plugin
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
INCLUDEPATH +=../../deps/WECore/include
include(../../deps/WECore/WECore.pri)
INCLUDEPATH +=\
    ../../deps/
SOURCES +=  \
    aboutwindow.cpp \
    lightmain.cpp \
    mainwindow.cpp
HEADERS +=  \
    ILightMain.h \
    aboutwindow.h \
    lightmain.h \
    mainwindow.h
FORMS += \
    aboutwindow.ui \
    mainwindow.ui

DISTFILES += \
    metadata.json

# Run lupdate / lrelease from Qt Creator (Tools > External > Linguist) to
# regenerate the .ts and build the .qm from it.
TRANSLATIONS += \
    translations/LightMain_zh_CN.ts

# Build the .qm files and embed them into the plugin resources as
# ":/i18n/LightMain_<locale>.qm", so the plugin can load them at runtime.
CONFIG += lrelease embed_translations