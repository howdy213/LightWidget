#ifndef WPLUGINDATA_H
#define WPLUGINDATA_H
#include "../WE/webase.h"
#define PData PluginData::getData()
#define PClass PData->getWEClass()
#define PPlugin PluginData::getPlugin()

class PluginDataPrivate;
class PluginData
{
public:
    PluginData();
    virtual ~PluginData();
    static void setData(WEBase* data);
    static WEBase* getData();
    static void setPlugin(WPlugin* plugin);
    static WPlugin* getPlugin();
private:
    static PluginDataPrivate* d;
};
#endif // WPLUGINDATA_H
