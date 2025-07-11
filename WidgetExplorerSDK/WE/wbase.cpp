/**
 * @file wbase.cpp
 * @brief 基础类实现文件
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
#include "wbase.h"

class WEBasePrivate
{
public:
    WEBase* base=0;
};

///
/// \brief WBase::WBase
///
WBase::WBase(){
    d=new WEBasePrivate;
}
///
/// \brief WBase::WBase
/// \param base
///
WBase::WBase(WEBase* base):WBase(){
    setBase(base);
}
///
/// \brief WBase::~WBase
///
WBase::~WBase(){
    delete d;
    d=nullptr;
}
///
/// \brief WBase::setBase
/// \param base
///
void WBase::setBase(WEBase* base){
    d->base=base;
}
///
/// \brief WBase::getBase
/// \return
///
WEBase* WBase::getBase(){
    return d->base;
}
