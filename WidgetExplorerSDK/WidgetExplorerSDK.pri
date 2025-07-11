
LIBS+=-luser32
LIBS+=-lshell32
HEADERS += \
    $$PWD/WConfig/wconfig.h \
    $$PWD/WConfig/wconfigdocument.h \
    $$PWD/WDef/colordef.h \
    $$PWD/WDef/wedef.h \
    $$PWD/WE/wbase.h \
    $$PWD/WE/webase.h \
    $$PWD/WE/webasedata.h \
    $$PWD/WE/weclass.h \
    $$PWD/WFile/wpath.h \
    $$PWD/WPlugin/wplugin.h \
    $$PWD/WPlugin/wplugindata.h \
    $$PWD/WPlugin/wplugininterface.h \
    $$PWD/WPlugin/wpluginmanager.h \
    $$PWD/WPlugin/wpluginmetadata.h \
    $$PWD/WPlugin/wpluginproxy.h \
    $$PWD/WPlugin/wvirtualplugin.h \
    $$PWD/WPlugin/wwidget.h \
    $$PWD/WPlugin/wwidgetmanager.h \
    $$PWD/we.h \
    $$PWD/wconfig.h \
    $$PWD/wdef.h \
    $$PWD/wfile.h \
    $$PWD/wplugin.h

SOURCES += \
    $$PWD/WConfig/wconfigdocument.cpp \
    $$PWD/WE/wbase.cpp \
    $$PWD/WE/webase.cpp \
    $$PWD/WE/webasedata.cpp \
    $$PWD/WE/weclass.cpp \
    $$PWD/WFile/wpath.cpp \
    $$PWD/WPlugin/wplugin.cpp \
    $$PWD/WPlugin/wplugindata.cpp \
    $$PWD/WPlugin/wpluginmanager.cpp \
    $$PWD/WPlugin/wpluginproxy.cpp \
    $$PWD/WPlugin/wvirtualplugin.cpp \
    $$PWD/WPlugin/wwidget.cpp \
    $$PWD/WPlugin/wwidgetmanager.cpp

