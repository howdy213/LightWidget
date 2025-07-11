/**
 * @file weclass.cpp
 * @brief WE类管理实现文件
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
#include "weclass.h"
#include"../WE/webase.h"
#include"../WidgetExplorerSDK/WFile/wpath.h"
#include"../WidgetExplorerSDK/WConfig/wconfigdocument.h"
#include"../WidgetExplorerSDK/WPlugin/wpluginmanager.h"
#include"../WidgetExplorerSDK/WPlugin/wwidgetmanager.h"

///
/// \brief The WEClassPrivate class
///
class WEClassPrivate{
public:
    WEBase* base=0;
};
///
/// \brief WEClass::WEClass
/// \param base
///
WEClass::WEClass(WEBase* base) {
    d=new WEClassPrivate;
    d->base=base;
}
WEClass::~WEClass(){
    delete d;
    d=nullptr;
}
///
/// \brief WEClass::configManager
/// \return
///
WConfigDocument* WEClass::configManager(){
    return d->base->getWEBaseData()->getData<WConfigDocument*>(W_PUBLIC_DATA_CONFIG);
}
///
/// \brief WEClass::pathManager
/// \return
///
WPath* WEClass::pathManager(){
    return d->base->getWEBaseData()->getData<WPath*>(W_PUBLIC_DATA_PATH);
}
///
/// \brief WEClass::pluginManager
/// \return
///
WPluginManager* WEClass::pluginManager(){
    return d->base->getWEBaseData()->getData<WPluginManager*>(W_PUBLIC_DATA_PMANAGER);
}
///
/// \brief WEClass::widgetManager
/// \return
///
WWidgetManager* WEClass::widgetManager(){
    return d->base->getWEBaseData()->getData<WWidgetManager*>(W_PUBLIC_DATA_WMANAGER);
}
