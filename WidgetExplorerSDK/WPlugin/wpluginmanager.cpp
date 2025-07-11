/**
 * @file wpluginmanager.cpp
 * @brief 插件管理器实现文件
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
#include"../WPlugin/wpluginmanager.h"
#include"../WPlugin/wplugininterface.h"
#include"../WPlugin/wplugin.h"

#include<QDir>
#include<QCoreApplication>
#include<QJsonArray>
#include<QHash>
#include<QVariant>
#include<QPluginLoader>

///
/// \brief The WPluginManagerPrivate class
///
class WPluginManagerPrivate
{
public:
    QMap<QUuid,WPlugin*>plugins;
};
///
/// \brief WPluginManager::WPluginManager
///
WPluginManager::WPluginManager()
{
    d=new WPluginManagerPrivate;
}
///
/// \brief WPluginManager::~WPluginManager
///
WPluginManager::~WPluginManager()
{
    delete d;
    d=nullptr;
}
///
/// \brief WPluginManager::createPlugin
/// \param configPath
/// \param config
/// \return
///
WPlugin* WPluginManager::createPlugin(QString configPath,QString config){
    auto plugin=new WPlugin(this);
    plugin->readConfig(configPath,config);
    return plugin;
}
///
/// \brief WPluginManager::loadPlugin
/// \param plugin
/// \return
///
bool WPluginManager::loadPlugin(WPlugin* plugin){
    if(plugin->available()){
        QString name=qvariant_cast<QString>(plugin->getMetaData(W_PLUGIN_ATTR_NAME));
        while(getPluginById(getPluginByName(name))){
            name+="#";
        }
        plugin->setMetaData(W_PLUGIN_ATTR_NAME,name);
        d->plugins.insert(QUuid::createUuid(),plugin);
        return true;
    }
    return false;
}
///
/// \brief WPluginManager::unloadPlugin
/// \param plugin
/// \return
///
bool WPluginManager::unloadPlugin(WPlugin* plugin){
    auto it=d->plugins.begin();
    for(;it!=d->plugins.end();it++){
        if(*it==plugin)break;
    }
    if(it==d->plugins.end())return false;
    return unloadPlugin(it);
}
///
/// \brief WPluginManager::unloadAllPlugins
///
void WPluginManager::unloadAllPlugins()
{
    for(auto it=d->plugins.begin();it!=d->plugins.end();it++){
        unloadPlugin(it);
    }
}
///
/// \brief WPluginManager::unloadPlugin
/// \param it
/// \return
///
bool WPluginManager::unloadPlugin(QMap<QUuid,WPlugin*>::Iterator it){
    if(!(*it)->unload())return false;
    d->plugins.erase(it);
    return true;
}
///
/// \brief WPluginManager::getPluginByAttr
/// \param key
/// \param value
/// \return
///
QVector<QUuid> WPluginManager::getPluginByAttr(QString key,QVariant value){
    QVector<QUuid> res;
    for(auto it=d->plugins.begin();it!=d->plugins.end();it++){
        if(it.value()->getMetaData(key)==value)res.push_back(it.key());
    }
    return res;
}
///
/// \brief WPluginManager::getPluginByName
/// \param name
/// \return
///
QUuid WPluginManager::getPluginByName(QString name){
    auto list=getPluginByAttr(W_PLUGIN_ATTR_NAME,name);
    if(!list.isEmpty())return list[0];
    return QUuid();
}
//
/// \brief WPluginManager::getPlugin
/// \param id
/// \return
///
WPlugin* WPluginManager::getPluginById(QUuid id){
    if(d->plugins.contains(id))return d->plugins[id];
    return nullptr;
}
///
/// \brief WPluginManager::setPluginData
/// \param id
/// \param key
/// \param value
/// \return
///
QVariant WPluginManager::setPluginData(QUuid id,QString key,QVariant value){
    if(!d->plugins.contains(id))return value;
    auto plugin=d->plugins[id];
    if(key==W_PLUGIN_ATTR_NAME){
        QString name=qvariant_cast<QString>(value);
        while(getPluginById(getPluginByName(name))){
            if(getPluginByAttr(W_PLUGIN_ATTR_NAME,name).length()==1&&plugin->getMetaData(W_PLUGIN_ATTR_NAME)==name)return name;
            name+="#";
        }
        value=name;
    }
    return value;
}
///
/// \brief WPluginManager::getUuid
/// \param plugin
/// \return
///
QUuid WPluginManager::getUuid(WPlugin* plugin){
    for(auto it=d->plugins.begin();it!=d->plugins.end();it++){
        if(it.value()==plugin)return it.key();
    }
    return QUuid();
}
///
/// \brief WPluginManager::allPluginsId
/// \return
///
QVector<QUuid> WPluginManager::allPluginsId(){
    return d->plugins.keys();
}
///
/// \brief WPluginManager::allPluginsInst
/// \return
///
QVector<WPlugin*> WPluginManager::allPluginsInst(){
    return d->plugins.values();
}
///
/// \brief WPluginManager::sendMsg
/// \param msg
///
void WPluginManager::sendMsg(WMetaData &msg)
{
    auto loader =getPluginById(getPluginByName(msg.dest));
    if(loader)
    {
        auto plugin = loader->inst();
        if(plugin)plugin->recMsg(msg);
    }
}
