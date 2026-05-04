/**
 * @file lightmain.h
 * @brief Header file for a light MainWidget.
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
#ifndef LIGHTMAIN_H
#define LIGHTMAIN_H
#include <QAction>
#include <QMainWindow>
#include <QObject>
#include <QtPlugin>

#include "WECore/plugin/wplugininterface.h"

namespace we {
class LightMainPrivate;
/**
 * @class LightMain
 * @brief Main widget implementation for the lightweight WidgetExplorer.
 */
class LightMain : public QObject, public WPluginInterface {
public:
    Q_OBJECT
    Q_PLUGIN_METADATA(IID WPluginInterface_iid)
    Q_INTERFACES(WPluginInterface)
public:
    LightMain();
    ~LightMain();
    bool init(WMessage &msg) override;
    void recMsg(WMessage &msg) override;
    bool deinit(WMessage &msg) override;

public:
    void createTray();

private:
    LightMainPrivate *d = nullptr;
    void onMainWindowExtension(const WEvent &event);
    QVariant onGetMainWindowState(const WEvent &event);
    void addExtensionAction(QAction *action, const QString &menuPath);

    QList<QAction *> m_pluginActions;
    QList<QDockWidget *> m_pluginDocks;
};
} // namespace we

#endif // LIGHTMAIN_H