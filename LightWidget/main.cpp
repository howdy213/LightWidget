/**
 * @file main.cpp
 * @brief 程序入口文件，实现了程序的初始化、主窗口创建、系统托盘设置等功能
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
#include"mainwindow.h"
#include"lightwidget.h"
#include"../WidgetExplorerSDK/we.h"
#include"../WidgetExplorerSDK/wplugin.h"
#include"../WidgetExplorerSDK/WConfig/wconfigdocument.h"
#include"../WidgetExplorerSDK/WFile/wpath.h"

#include<QApplication>
#include<QLockFile>
#include<QMenu>
#include<QSystemTrayIcon>
#include<QStyleFactory>

int main(int argc, char* argv[])
{
    //检测多开
    QLockFile* lockfile=new QLockFile(WPath().getModuleFolder() + ".lock");
    if (!lockfile->tryLock(0))return 1;

    //初始化
    auto lwptr=new LightWidget;
    if(!WE::init(lwptr,WE::WE_LW))return 0;
    lwptr->getWEClass()->configManager()->load(WPath().getModuleFolder()+"config/config.json",true);

    if (qEnvironmentVariableIsEmpty("QT_FONT_DPI"))
    {
        auto config=lwptr->getWEClass()->configManager();
        if(config->hasArg("font"))qputenv("QT_FONT_DPI", qvariant_cast<QByteArray>(lwptr->getWEClass()->configManager()->get("font")));
        if(config->hasArg("scale"))qputenv("QT_SCALE_FACTOR", qvariant_cast<QByteArray>(lwptr->getWEClass()->configManager()->get("scale")));
    }

    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/icons/icon/we.png"));
    a.setQuitOnLastWindowClosed(false);

    //主窗口
    QStringList param;
    for (int i = 1; i <= argc - 1; i++)param.push_back(argv[i]);

    MainWindow w(param,lwptr);
    w.setWindowTitle("LightWidget");
    w.setLockFile(lockfile);

    //创建系统托盘
    QSystemTrayIcon* pSystemTray = new QSystemTrayIcon(&w);
    if (NULL != pSystemTray) {
        pSystemTray->setIcon(QIcon(":/icons/icon/we.png"));
        pSystemTray->setToolTip("LightWidget");
        pSystemTray->show();
    }
    auto* trayMenu = new QMenu(&w);
    QObject::connect(pSystemTray, &QSystemTrayIcon::activated, &w, &MainWindow::tray);
    auto* showPanelAction = new QAction("显示", &w);
    QObject::connect(showPanelAction, &QAction::triggered, &w, &MainWindow::showPanel);
    trayMenu->addAction(showPanelAction);
    auto* quitAction = new QAction("退出", &w);
    QObject::connect(quitAction, &QAction::triggered, &w, &QApplication::quit);
    trayMenu->addAction(quitAction);
    pSystemTray->setContextMenu(trayMenu);
    pSystemTray->show();

    return a.exec();
}
