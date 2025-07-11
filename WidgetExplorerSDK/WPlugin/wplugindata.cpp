#include "wplugindata.h"
///
/// \brief The PluginDataPrivate class
///
class PluginDataPrivate
{
public:
    WEBase* data=nullptr;
    WPlugin* plugin=nullptr;
};
PluginDataPrivate* PluginData::d=nullptr;
///
/// \brief PluginData::PluginData
///
PluginData::PluginData() {
}
///
/// \brief PluginData::~PluginData
///
PluginData::~PluginData(){
}
///
/// \brief PluginData::setData
/// \param data
///
void PluginData::setData(WEBase* data){
    if(!d)d=new PluginDataPrivate;
    d->data=data;
}
///
/// \brief PluginData::getData
/// \return
///
WEBase* PluginData::getData(){
    if(!d)d=new PluginDataPrivate;
    return d->data;
}
///
/// \brief PluginData::setPlugin
/// \param plugin
///
void PluginData::setPlugin(WPlugin* plugin){
    if(!d)d=new PluginDataPrivate;
    d->plugin=plugin;
}
///
/// \brief PluginData::getPlugin
/// \return
///
WPlugin* PluginData::getPlugin(){
    if(!d)d=new PluginDataPrivate;
    return d->plugin;
}
