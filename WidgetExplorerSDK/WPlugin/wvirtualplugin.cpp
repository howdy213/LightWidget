/**
 * @file wvirtualplugin.cpp
 * @brief 虚拟插件类实现文件
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
#include"wvirtualplugin.h"
#include"../WFile/wpath.h"
#include"../WPlugin/wplugin.h"

///
/// \brief WVirtualPlugin::WVirtualPlugin
///
WVirtualPlugin::WVirtualPlugin() {
    return;
}
///
/// \brief WVirtualPlugin::~WVirtualPlugin
///
WVirtualPlugin::~WVirtualPlugin() {}
///
/// \brief WVirtualPlugin::init
/// \param data
///
void WVirtualPlugin::init(QMap<QString,QVariant> &data){
    auto plugin=qvariant_cast<WPlugin*>(data[W_DATA_PLUGIN]);
    WPath().ShellExe(filePath,"open",qvariant_cast<QString>(plugin->getMetaData(W_PLUGIN_ATTR_INIT)));
}
///
/// \brief WVirtualPlugin::recMsg
/// \param msg
///
void WVirtualPlugin::recMsg(WMetaData &msg){
    WPath().ShellExe(filePath,"open",qvariant_cast<QString>(msg.map[W_DATA_COMMAND]));
}
///
/// \brief WVirtualPlugin::connectManager
/// \param receiver
/// \param isConnect
/// \return
///
bool WVirtualPlugin::connectManager(WPluginManager *receiver, bool isConnect)const {
    Q_UNUSED(receiver)
    Q_UNUSED(isConnect)
    return false;
}
