/**
 * @file exampleplugin.cpp
 * @brief 示例插件实现文件
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
#include"examplewidget.h"
#include"exampledialog.h"
#include"exampleplugin.h"
#include"../WidgetExplorerSDK/we.h"
#include"../WidgetExplorerSDK/wfile.h"
#include"../WidgetExplorerSDK/WPlugin/wplugin.h"
#include"../WidgetExplorerSDK/WPlugin/wwidgetmanager.h"

#include<QWidget>
#include<windows.h>

ExamplePlugin::ExamplePlugin() {}

ExamplePlugin::~ExamplePlugin()
{

}

void ExamplePlugin::init(QMap<QString,QVariant> &data) {
    PluginData::setData(qvariant_cast<WEBase*>(data[W_DATA_WE]));
    PluginData::setPlugin(qvariant_cast<WPlugin*>(data[W_DATA_PLUGIN]));

    auto widgetManager=PClass->widgetManager();
    widget=new ExampleWidget(PData);
    widgetManager->addWidget(QUuid::createUuid(),widget);

    WPlugin* plugin=qvariant_cast<WPlugin*>(data[W_DATA_PLUGIN]);
    plugin->setMetaData(W_PLUGIN_ATTR_NAME,"Example");
}

void ExamplePlugin::recMsg(WMetaData &msg) {
    widget->sendMessageCallback(msg);
}
