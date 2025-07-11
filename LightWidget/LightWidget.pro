QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
RC_ICONS = icon/we_tp.ico
# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/release/ -lwidgetexplorersdk
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/debug -lwidgetexplorersdk
include(../WidgetExplorerSDK/WidgetExplorerSDK.pri)
SOURCES += \
    aboutwindow.cpp \
    lightsystem.cpp \
    lightwidget.cpp \
    main.cpp \
    mainwindow.cpp \

HEADERS += \
    aboutwindow.h \
    lightsystem.h \
    lightwidget.h \
    mainwindow.h \

FORMS += \
    aboutwindow.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES +=

RESOURCES += \
    resourse.qrc
