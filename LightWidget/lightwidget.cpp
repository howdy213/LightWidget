/**
 * @file lightwidget.cpp
 * @brief LightWidget主类实现文件
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
#include"lightwidget.h"
#include"../WidgetExplorerSDK/WE/we.h"
#include"../WidgetExplorerSDK/WFile/wpath.h"
#include"../WidgetExplorerSDK/WConfig/wconfigdocument.h"
#include"../WidgetExplorerSDK/WPlugin/wwidgetmanager.h"
#include"../WidgetExplorerSDK/WPlugin/wplugin.h"


void LightWidget::initData(QSharedPointer<WEBaseData> data){
    QVariant config;
    config.setValue(new WConfigDocument);
    data->addData(W_PUBLIC_DATA_CONFIG,config);

    QVariant path;
    path.setValue(new WPath);
    data->addData(W_PUBLIC_DATA_PATH,path);

    QVariant pluginManager;
    pluginManager.setValue(new WPluginManager);
    data->addData(W_PUBLIC_DATA_PMANAGER,pluginManager);

    QVariant widgetManager;
    widgetManager.setValue(new WWidgetManager);
    data->addData(W_PUBLIC_DATA_WMANAGER,widgetManager);
};

LightWidget::LightWidget():WEBase() {
    auto data=WEBase::getWEBaseData();
    initData(data);
    return;
}
bool LightWidget::sendMsgs(QString widgetName,QMap<QString, QVariant> map){
    auto widgetId=WE::inst()->getWEClass()->pluginManager()->getPluginByName(widgetName);
    if(widgetId.isNull())return 0;
    WPluginInterface *app = WE::inst()->getWEClass()->pluginManager()->getPluginById(widgetId)->inst();

    if (app)
    {
        WMetaData data;
        data.from=W_DATA_WE;
        data.dest=widgetName;
        data.map=map;
        app->recMsg(data);
    }
    return 1;
}

