/**
 * @file webase.h
 * @brief WE基础类头文件
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
#ifndef WEBASE_H
#define WEBASE_H
#include"../WDef/wedef.h"
#include"../WE/webasedata.h"
#include"../WE/weclass.h"

#include<QObject>
#include<QSharedPointer>

class WEBasePrivate;

class WE_NAMESPACE::WEBase
{
public:
    WEBase();
    virtual ~WEBase();
    virtual QSharedPointer<WEClass> getWEClass();
    virtual void setWEClass(QSharedPointer<WEClass> weclass);
    virtual QSharedPointer<WEBaseData> getWEBaseData();
    virtual void setWEBaseData(QSharedPointer<WEBaseData> webasedata);
private:
    WEBasePrivate* d=0;
};

Q_DECLARE_METATYPE(WE_NAMESPACE::WEBase);
Q_DECLARE_METATYPE(WE_NAMESPACE::WEBase*);

#endif // WEBASE_H
