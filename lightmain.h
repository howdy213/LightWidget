/**
 * @file lightmain.h
 * @brief 轻量MainWidget头文件
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
#ifndef EXAMPLEPLUGIN_H
#define EXAMPLEPLUGIN_H
#include "WECore/WPlugin/wplugininterface.h"

#include <QObject>
#include <QtPlugin>

class LightMainPrivate;
class WE_NAMESPACE::LightMain : public QObject, public WPluginInterface {
public:
    Q_OBJECT
    Q_PLUGIN_METADATA(IID WPluginInterfaceIID)
    Q_INTERFACES(WPluginInterface)
    W_DECLARE_PLUGIN(LightMain);
signals:
    void sendMsg(WMetaData &);

public:
    LightMain();
    ~LightMain();
    bool init(WMetaData &msg) override;
    void recMsg(WMetaData &msg) override;
    bool deinit(WMetaData &msg) override;

public:
    void createTray();

private:
    LightMainPrivate *d = nullptr;
};

#endif // EXAMPLEPLUGIN_H
