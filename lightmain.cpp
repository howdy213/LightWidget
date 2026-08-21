/**
 * @file lightmain.cpp
 * @brief Implementation file for a light MainWidget.
 * @author howdy213
 * @date 2026-05-04
 * @version 2.0.0
 *
 * @copyright Copyright 2025-2026 howdy213
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
#include "ILightMain.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QApplication>
#include <QDockWidget>
#include <QMenu>
#include <QMenuBar>
#include <QWidget>
#include <Windows.h>

#include "WECore/metadata/WMetaDocument.h"
#include "WECore/plugin/wplugin.h"
#include "WECore/plugin/wplugindata.h"
#include "WECore/plugin/wpluginmessage.h"
#include "WECore/plugin/wpluginmanager.h"
#include "WECore/service/wserviceregistry.h"
#include "WECore/widget/wwidgetmanager.h"

using namespace LightWidget::Consts;
using namespace we::Consts;
namespace we {
/**
 * @class LightMainPrivate
 * @brief Private implementation class for LightMain.
 */
class LightMainPrivate {
public:
    MainWindow *w = nullptr;
    bool inited = false;
    QSystemTrayIcon *tray = nullptr;
};

/**
 * @brief Constructs a LightMain object.
 */
LightMain::LightMain() { d = new LightMainPrivate; }

/**
 * @brief Destroys a LightMain object.
 */
LightMain::~LightMain() {
    if (d->w) {
        delete d->w;
        d->w = nullptr;
    }
    d->tray->hide();
    delete d;
    d = nullptr;
}

/**
 * @brief Initializes the plugin.
 * @param msg Message containing initialization data.
 * @return True if initialization succeeded, false otherwise.
 */
bool LightMain::init(WMessage &msg) {
    if (d->inited)
        return false;
    d->inited = true;
    qApp->setQuitOnLastWindowClosed(false);
    auto we = qvariant_cast<WEBase *>(msg.map[Data::WEBase]);
    PluginData::setData(we);
    auto plugin = qvariant_cast<WPlugin *>(msg.map[Data::Plugin]);
    PluginData::setPlugin(plugin);
    auto params = qvariant_cast<QStringList>(msg.map[Data::Params]);

    auto config = PClass->configManager();
    if (params.length() >= 1)
        if (params[0] == Plugin::Autorun)
            config->set(Plugin::Autorun, true);

    auto wmgr = PClass->widgetManager();
    auto registry = PData->getWEBaseData()->getData<WServiceRegistry *>(
        Public::ServiceRegistry);

    // Subscribe to extension topics
    SubscribeFunc func = [this](const WEvent &ev) { onMainWindowExtension(ev); };
    wmgr->subscribe("ui.mainwindow.*", this, func);

    // Provide state query service
    wmgr->subscribe(
        "ui.mainwindow.getState", this,
        (SubscribeFunc)[this, wmgr](const WEvent &ev) {
            QVariant state = onGetMainWindowState(ev);
            WMessage msg;
            msg.map[Data::Data] = state;
            WEvent reply("_reply_" + ev.correlationId, msg, "LightMain");
            wmgr->publish(reply);
        });

    d->w = new MainWindow;
    d->w->init();

    if (!qvariant_cast<bool>(config->get(Plugin::Autorun))) {
        if (!PluginData::getPlugin()
                 ->initArg()
                 .split(' ')
                 .contains("-hide"))
            d->w->show();
    }
    d->w->setWindowTitle("LightWidget");
    createTray();

    return true;
}

/**
 * @brief Receives messages sent to the plugin.
 * @param msg The message to process.
 */
void LightMain::recMsg(WMessage &msg) { Q_UNUSED(msg); }

/**
 * @brief Deinitializes the plugin.
 * @param msg Deinitialization message.
 * @return True if deinitialization succeeded, false otherwise.
 */
bool LightMain::deinit(WMessage &msg) {
    Q_UNUSED(msg);
    d->tray->hide();
    return true;
}

/**
 * @brief Creates and configures the system tray icon.
 */
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

/**
 * @brief Handles main window extension events.
 * @param event The event containing extension data.
 */
void LightMain::onMainWindowExtension(const WEvent &event) {
    const QVariantMap &map = event.msg.map;
    QObject *object = event.msg.object;

    if (event.topic.endsWith(Event::MenuAction)) {
        QAction *action = qobject_cast<QAction *>(object);
        QString menuPath = map.value(Key::MenuPath).toString();
        QString menuOpType = map.value(Key::MenuOpType).toString();
        addExtensionAction(menuOpType, action, "功能/" + menuPath);
    } else if (event.topic.endsWith(Event::ToolAction)) {
        QAction *action = qobject_cast<QAction *>(object);
        if (action && d->w) {
            d->w->addToolBarAction(action);
            m_pluginActions.append(action);
        }
    } else {
        QWidget *widget = qobject_cast<QWidget *>(object);
        if (!widget)
            return;
        if (event.topic.endsWith(Event::Tab)) {
            QString title = map.value(Key::TabTitle).toString();
            if (d->w) {
                d->w->getUiPointer()->tabWidget->addTab(widget, title);
            }
        }
    }
}

/**
 * @brief Adds an action to a specified menu path.
 * @param action The action to add.
 * @param menuPath The menu path (e.g., "功能/SubMenu").
 */
void LightMain::addExtensionAction(QString menuOpType, QAction *action, const QString &menuPath) {
    if (!d->w || !action)
        return;
    if (menuOpType != Key::MenuOpCreate && menuOpType != Key::MenuOpDelete) menuOpType = Key::MenuOpCreate;
    
    // Find or create the menu path
    QStringList parts = menuPath.split('/', Qt::SkipEmptyParts);
    QMenu *targetMenu = nullptr;
    QMenuBar *bar = d->w->menuBar();
    if (menuOpType == Key::MenuOpDelete) {
        // Navigate through the menu path to find the target menu
        for (const QString &part : std::as_const(parts)) {
            QMenu *found = bar->findChild<QMenu *>(part);
            if (found) {
                targetMenu = found;
            } else {
                // Menu path not found, can't delete
                return;
            }
        }
        
        // Remove the action from the target menu
        if (targetMenu) {
            targetMenu->removeAction(action);
        }
        
        // Remove the action from the plugin actions list
        m_pluginActions.removeAll(action);
        
        // Optionally: Clean up empty menus (optional, can be removed if not needed)
        if (targetMenu && targetMenu->actions().isEmpty()) {
            // Remove empty menus from the hierarchy
            QMenu *parentMenu = nullptr;
            QWidget *parentWidget = targetMenu->parentWidget();
            if (parentWidget) {
                parentMenu = qobject_cast<QMenu*>(parentWidget);
            }
            
            if (parentMenu) {
                bar->removeAction(targetMenu->menuAction());
                targetMenu->deleteLater();
            }
        }
    }
    else if (menuOpType == Key::MenuOpCreate)
    {
        for (const QString &part : std::as_const(parts)) {
            QMenu *found = bar->findChild<QMenu *>(part);
            if (found) {
                targetMenu = found;
            } else {
                if (targetMenu) {
                    targetMenu = targetMenu->addMenu(part);
                } else {
                    targetMenu = bar->addMenu(part);
                }
                targetMenu->setObjectName(part);
            }
        }
        if (targetMenu) {
            targetMenu->addAction(action);
            m_pluginActions.append(action);
        }
    }
}

/**
 * @brief Retrieves the current state of the main window.
 * @param event The requesting event.
 * @return A QVariant containing the window state.
 */
QVariant LightMain::onGetMainWindowState(const WEvent &event) {
    Q_UNUSED(event);
    QVariantMap state;
    if (d->w) {
        //no data
    }
    return state;
}
} // namespace we