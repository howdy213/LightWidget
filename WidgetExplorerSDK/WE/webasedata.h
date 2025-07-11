/**
 * @file webasedata.h
 * @brief WE基础数据头文件
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
#ifndef WEBASEDATA_H
#define WEBASEDATA_H
#include"../WDef/wedef.h"

#include<QString>
#include<QMap>
#include<QObject>
#include<QVariant>

class WEBaseDataPrivate{
public:
    QMap<QString,QVariant> data;
    QMap<QString,QString>map;
};

class WE_NAMESPACE::WEBaseData
{
public:
    WEBaseData();
    virtual ~WEBaseData();
    template<typename T>
    T getData(QString name);
    void addData(QString key,QVariant value);
    void addMapping(QString preimage,QString image);
private:
    WEBaseDataPrivate* d=nullptr;
};

///
/// \brief WEBaseData::getData
/// \param name
/// \return
///
template<typename T>
T WEBaseData::getData(QString name){
    name=d->map.contains(name)?d->map[name]:name;
    return qvariant_cast<T>(this->d->data[name]);
}

#endif // WEBASEDATA_H
