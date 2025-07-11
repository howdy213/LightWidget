QT += core gui
QT += widgets

TEMPLATE = lib
CONFIG += plugin

CONFIG += c++17
TARGET          = ExamplePlugin       #生成插件的名称

LIBS += -luser32
LIBS+=-lshell32
win32 {
    CONFIG(debug, debug|release) {
        DESTDIR  = ../LightWidget/debug/widget/Example
    } else {
        DESTDIR  = ../LightWidget/release/widget/Example
    }
}#生成插件的目录
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../LightWidget/release/ -lwidgetexplorersdk
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../LightWidget/debug -lwidgetexplorersdk
# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
include(../WidgetExplorerSDK/WidgetExplorerSDK.pri)
SOURCES += \
    exampledialog.cpp \
    exampleplugin.cpp \
    examplewidget.cpp

HEADERS += \
    exampledialog.h \
    exampleplugin.h \
    examplewidget.h

DISTFILES +=

# Default rules for deployment.
unix {
    target.path = $$[QT_INSTALL_PLUGINS]/generic
}
!isEmpty(target.path): INSTALLS += target

FORMS += \
    exampledialog.ui

