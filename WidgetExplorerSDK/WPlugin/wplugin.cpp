/**
 * @file wplugin.cpp
 * @brief 插件类实现文件
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
#include"wplugin.h"
#include"../WFile/wpath.h"
#include"../WPlugin/wpluginmanager.h"
#include"../WConfig/wconfigdocument.h"
#include"../WPlugin/wplugininterface.h"
#include"../WPlugin/wvirtualplugin.h"

#include<QRandomGenerator>

///
/// \brief The WPluginPrivate class
///
class WPluginPrivate{
public:
    WConfigDocument doc;
    WPluginManager* parent=nullptr;
    QPluginLoader* loader=nullptr;
    WPluginInterface* wif=nullptr;
    bool loaded=false;
    enum LoadFileType{TYPE_DLL,TYPE_EXE,TYPE_OTHER,TYPE_NONE}type=TYPE_NONE;
};

///
/// \brief WPlugin::WPlugin
/// \param parent
///
WPlugin::WPlugin(WPluginManager* parent) {
    d=new WPluginPrivate;
    d->parent=parent;
    d->doc.setDefaultValue("default");
    d->doc.setDefault(W_PLUGIN_ATTR_AUTHOR,"");
    d->doc.setDefault(W_PLUGIN_ATTR_DATE,"2025-1-1");
    d->doc.setDefault(W_PLUGIN_ATTR_DES,"");
    d->doc.setDefault(W_PLUGIN_ATTR_VERSION,WE_VERSION_STRING);
    d->doc.setDefault(W_PLUGIN_ATTR_NAME,"Plugin-"+QString::number((long long int)QRandomGenerator::system()->generate()));
    d->doc.setDefault(W_PLUGIN_ATTR_PATH,"");
}
///
/// \brief WPlugin::~WPlugin
///
WPlugin::~WPlugin(){
    delete d;
    d=nullptr;
}
///
/// \brief WPlugin::readConfig
/// \param filePath
/// \param config
/// \return
///
bool WPlugin::readConfig(QString filePath,QString config){
    if(!d->doc.load(config,false))return false;
    QString filepath=WPath().transPath(filePath,qvariant_cast<QString>(d->doc.get(W_PLUGIN_ATTR_PATH)));
    d->doc.set(W_PLUGIN_ATTR_PATH,filepath);
    return true;
}
///
/// \brief WPlugin::load
/// \return
///
bool WPlugin::load(){
    if(d->parent==nullptr)return false;
    QString filepath=qvariant_cast<QString>(this->getMetaData(W_PLUGIN_ATTR_PATH));
    if(QLibrary::isLibrary(filepath)){
        return loadDll(filepath);
    }
    else if(filepath.endsWith(".exe")||filepath.endsWith(".bat")){
        return loadExe(filepath);
    }
    return false;
}
///
/// \brief WPlugin::unload
/// \return
///
bool WPlugin::unload(){
    if(d->parent){
        d->parent=nullptr;
        return d->parent->unloadPlugin(this);
    }
    return d->loader?d->loader->unload():0;
}
///
/// \brief WPlugin::available
/// \return
///
bool WPlugin::available(){
    return d->loaded;
}
///
/// \brief WPlugin::getMetaData
/// \param key
/// \return
///
QVariant WPlugin::getMetaData(QString key){
    return d->doc.get(key);
}
///
/// \brief WPlugin::getId
/// \return
///
QUuid WPlugin::getId(){
    QUuid id;
    if(d->parent){
        id=d->parent->getUuid(this);
    }
    return id;
}
///
/// \brief WPlugin::setMetaData
/// \param key
/// \param value
///
void WPlugin::setMetaData(QString key,QVariant value){
    if(d->parent!=nullptr){
        value=d->parent->setPluginData(d->parent->getUuid(this),key,value);
    }
    d->doc.set(key,value);
}
///
/// \brief WPlugin::loadDll
/// \param dllPath
/// \return
///
bool WPlugin::loadDll(QString dllPath){
    QString filepath=dllPath;
    if(!QLibrary::isLibrary(filepath)){
        return false;
    }
    QPluginLoader *loader = new QPluginLoader(filepath);
    if(loader->load())
    {
        WPluginInterface *pluginIf = qobject_cast<WPluginInterface *>(loader->instance());
        if(pluginIf)
        {
            pluginIf->connectManager(d->parent, true);
            d->loader=loader;
            d->loaded=true;
            setMetaData("type","dll");
        }
        else
        {
            delete loader;
            loader = nullptr;
        }
    }
    d->type=WPluginPrivate::TYPE_DLL;
    return d->loaded;
}
///
/// \brief WPlugin::loadExe
/// \param exePath
/// \return
///
bool WPlugin::loadExe(QString exePath){
    d->loader=nullptr;
    d->loaded=true;
    d->type=WPluginPrivate::TYPE_EXE;
    auto vp=new WVirtualPlugin;
    vp->setFile(exePath);
    setMetaData("type","exe");
    QString name=qvariant_cast<QString>(getMetaData(W_PLUGIN_ATTR_PATH)).split("/").last();
    if(!name.isEmpty())setMetaData(W_PLUGIN_ATTR_NAME,name.split("/").last().split(".")[0]);
    d->wif=vp;
    return true;
}
///
/// \brief WPlugin::inst
/// \return
///
WPluginInterface* WPlugin::inst(){
    switch(d->type){
    case WPluginPrivate::TYPE_DLL:
        if(!d->loader)return nullptr;
        return qobject_cast<WPluginInterface*>(d->loader->instance());
        break;
    case WPluginPrivate::TYPE_EXE:
        return d->wif;
        break;
    case WPluginPrivate::TYPE_OTHER:
        break;
    case WPluginPrivate::TYPE_NONE:
        break;
    }
    return nullptr;
}
