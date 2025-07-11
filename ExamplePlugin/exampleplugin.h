/**
 * @file exampleplugin.h
 * @brief 示例插件类头文件
 * @author howdy213
 * @date 2025-07-11
 * @version 1.0.0
 *
 * Copyright 2025 howdy213
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
#include"exampledialog.h"
#include"examplewidget.h"
#include"../WidgetExplorerSDK/WPlugin/wplugininterface.h"
#include"../WidgetExplorerSDK/WPlugin/wpluginmanager.h"

#include<QObject>
#include<QtPlugin>

class ExamplePlugin :public QObject, public WPluginInterface
{
public:
    Q_OBJECT
    Q_PLUGIN_METADATA(IID PluginInterface_iid)
    Q_INTERFACES(WPluginInterface)
    W_DECLARE_PLUGIN(ExamplePlugin);
signals:
    void sendMsg(WMetaData&);
public:
    ExamplePlugin();
    ~ExamplePlugin();
    void init(QMap<QString,QVariant> &data) override;
    void recMsg(WMetaData &msg) override;
private:
    ExampleWidget* widget=nullptr;
};

#endif // EXAMPLEPLUGIN_H
