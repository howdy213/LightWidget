/**
 * @file wpath.h
 * @brief 路径处理类头文件
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
#ifndef WPATH_H
#define WPATH_H
#include"../WDef/wedef.h"
#include"../WE/wbase.h"

#include<Windows.h>
#include<QString>
#include<QtGlobal>
#include<QObject>

class WPathPrivate;

class WE_NAMESPACE::WPath:public WBase
{
public:
    WPath(WEBase* we=nullptr);
    virtual~WPath();
    void setWEBase(WEBase* base);
    QString getModulePath();
    QString getModuleFolder();
    QString getModulePath(QUuid moduleId);
    QString getModuleFolder(QUuid moduleId);
    QString getModulePath(WPlugin* plugin);
    QString getModuleFolder(WPlugin* plugin);
    QString splitPath(QString path);
    QString transPath(QString cur,QString des);
    void ShellExe(QString lpFile,QString lpOperation="open",QString lpParameters="",QString lpDirectory="default");
private:
    WPathPrivate* d=nullptr;
};

Q_DECLARE_METATYPE(WE_NAMESPACE::WPath);
Q_DECLARE_METATYPE(WE_NAMESPACE::WPath*);
#endif // WPATH_H
