/**
 * @file we.cpp
 * @brief WE类实现文件
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
#include"we.h"
#include"webase.h"

WEBase* WE::m_instance=nullptr;
WE::WETYPE WE::t=WE_NONE;
///
/// \brief WE::init
/// \param base
/// \param type
/// \return
///
bool WE::init(WEBase* base, WETYPE type)
{
    if (m_instance == nullptr) {
        t = type;
        m_instance = base;
    }
    else return false;
    return true;
}
///
/// \brief WE::inst
/// \return
///
WEBase* WE::inst(){
    return m_instance;
}
