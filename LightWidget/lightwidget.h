/**
 * @file lightwidget.h
 * @brief LightWidget类头文件
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
#ifndef LIGHTWIDGET_H
#define LIGHTWIDGET_H
#include"../WidgetExplorerSDK/WE/webase.h"

#include<QMap>
#include<QString>

class WE_NAMESPACE::LightWidget : public WEBase
{
public:
    LightWidget();
    void initData(QSharedPointer<WEBaseData> data);
    bool sendMsgs(QString widgetName, QMap<QString, QVariant> map);
private:
};

#endif // LIGHTWIDGET_H
