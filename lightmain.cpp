/**
 * @file lightmain.cpp
 * @brief WidgetExplorer轻量MainWidget
 * @author howdy213
 * @date 2026-1-30
 * @version 1.1.0
 *
 * Copyright 2025-2026 howdy213
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "lightmain.h"
#include "mainwindow.h"

#include "WConfig/wconfigdocument.h"
#include "WECore/WPlugin/wplugin.h"
#include "WECore/WPlugin/wplugindata.h"

#include <QApplication>
#include <QMenu>
#include <QWidget>
#include <Windows.h>
using namespace we::Consts;

class LightMainPrivate {
public:
    MainWindow *w = nullptr;
    bool inited = false;
    QSystemTrayIcon *tray = nullptr;
};

///
/// \brief LightMain::LightMain
///
LightMain::LightMain() { d = new LightMainPrivate; }
///
/// \brief LightMain::~LightMain
///
LightMain::~LightMain() {
    if (d->w) {
    delete d->w;
    d->w = nullptr;
    }
    d->tray->hide();
    delete d;
    d = nullptr;
}
///
/// \brief LightMain::init
/// \param data
///
bool LightMain::init(WMetaData &msg) {
    if (d->inited)
        return false;
    d->inited = true;

    auto we = qvariant_cast<WEBase *>(msg.map[Data::WEBase]);
    PluginData::setData(we);
    auto plugin = qvariant_cast<WPlugin *>(msg.map[Data::Plugin]);
    PluginData::setPlugin(plugin);
    auto params = qvariant_cast<QStringList>(msg.map[Data::Params]);
    plugin->setMetaData(Plugin::Name, "LightMain");

    auto config = PClass->configManager();
    if (params.length() >= 1)
        if (params[0] == Plugin::Autorun)
            config->set(Plugin::Autorun, true);

    d->w = new MainWindow;

    if (!qvariant_cast<bool>(config->get(Plugin::Autorun))) {
        if (!PluginData::getPlugin()
                 ->getMetaData(Plugin::Init)
                 .toString()
                 .split(' ')
                 .contains("-hide"))
            d->w->show();
    }
    d->w->setWindowTitle("LightWidget");
    createTray();
    return true;
}
///
/// \brief LightMain::recMsg
/// \param msg
///
void LightMain::recMsg(WMetaData &msg) { Q_UNUSED(msg); }
///
/// \brief LightMain::deinit
/// \param msg
/// \return
///
bool LightMain::deinit(WMetaData &msg) {
    Q_UNUSED(msg);
    d->tray->hide();
    return true;
}
///
/// \brief LightMain::createTray
///
void LightMain::createTray() {
    d->tray = new QSystemTrayIcon(d->w);
    if (NULL != d->tray) {
        d->tray->setIcon(QIcon(":/icons/icon/we.png"));
        d->tray->setToolTip("LightWidget");
        d->tray->show();
    }
    auto *trayMenu = new QMenu(d->w);
    QObject::connect(d->tray, &QSystemTrayIcon::activated, d->w,
                     &MainWindow::tray);
    auto *showPanelAction = new QAction("显示", d->w);
    QObject::connect(showPanelAction, &QAction::triggered, d->w,
                     &MainWindow::showPanel);
    trayMenu->addAction(showPanelAction);
    auto *quitAction = new QAction("退出", d->w);
    QObject::connect(quitAction, &QAction::triggered, d->w, &QApplication::quit);
    trayMenu->addAction(quitAction);
    d->tray->setContextMenu(trayMenu);
    d->tray->show();
}
