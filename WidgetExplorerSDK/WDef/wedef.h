/**
 * @file wedef.h
 * @brief 核心定义头文件，包含类的前置声明
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
#ifndef WEDEF_H
#define WEDEF_H

#define EX(a) a
#define _LS(a,b) a##b
#define LS(a,b) _LS(a,b)
#define _MS(a) #a
#define MS(a) _MS(a)
#define V2V(type,key) qvariant_cast<type>(var[key])

#define _WE_VERSION 1_1_0
#define WE_VERSION _WE_VERSION
#define _WE_VERSION_STRING 1.1.0
#define WE_VERSION_STRING MS(_WE_VERSION_STRING)

#define WE_NAMESPACE we
#if defined(WIDGETEXPLORERSDK_LIBRARY)
#  define WIDGETEXPLORERSDK_EXPORT Q_DECL_EXPORT
#else
#  define WIDGETEXPLORERSDK_EXPORT Q_DECL_IMPORT
#endif

#define WApp (WE::inst())

#define W_DATA_PLUGIN "Plugin"
#define W_DATA_WE "WE"
#define W_DATA_
#define W_DATA_COMMAND "Command"

#define W_PUBLIC_DATA_PMANAGER "WPluginManager.pluginManager"
#define W_PUBLIC_DATA_WMANAGER "WWidgetManager.widgetManager"
#define W_PUBLIC_DATA_CONFIG "WConfigDocument.configManager"
#define W_PUBLIC_DATA_PATH "WPath.pathManager"

#define W_PLUGIN_ATTR_VERSION "version"
#define W_PLUGIN_ATTR_NAME "name"
#define W_PLUGIN_ATTR_INIT "init"
#define W_PLUGIN_ATTR_PATH "path"
#define W_PLUGIN_ATTR_DATE "date"
#define W_PLUGIN_ATTR_AUTHOR "author"
#define W_PLUGIN_ATTR_DES "des"

inline namespace WE_NAMESPACE {
class WE;
class WBase;
class WEBase;
class WEBaseData;
class LightWidget;
class WidgetExplorer;
class WPlugin;
class WidgetDebugger;
class WPath;
class WPluginManager;
class WConfigDocument;
class WWidgetManager;
class WWidget;
class WEClass;
class WVirtualPlugin;
template<class T>class WConfig;
}

template<typename funcType>
void* getMemberAddr(funcType func){
    union {
        void* pv;
        funcType pfn;
    } u;
    u.pfn = func;
    return u.pv;
}

#endif // WEDEF_H
