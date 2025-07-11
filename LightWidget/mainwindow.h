/**
 * @file mainwindow.h
 * @brief 主窗口类头文件
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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include"../WidgetExplorerSDK/WDef/wedef.h"
#include"../WidgetExplorerSDK/WPlugin/wplugin.h"

#include<QMainWindow>
#include<QListWidget>
#include<QCloseEvent>
#include<QSystemTrayIcon>
#include<QLockFile>

namespace Ui { class MainWindow; }

class MainWindowPrivate;
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QStringList param, LightWidget *ptr, QWidget *parent = nullptr);
    ~MainWindow();
private:
    void initWindow();
    void initPlugin();
    void initList();
    void initTable();
    void initMenu();
private:
    void createCol(int col,QString title,QFont font,QColor color);
    void createRow(int row,  WPlugin *info);
    QStringList ReadLinkFile();
public:
    void setLockFile(QLockFile* file);
    void closeEvent(QCloseEvent *event);
    void tray(QSystemTrayIcon::ActivationReason reason);
public slots:
    void showPanel();
private slots:
    void about();
    void restart();
private slots:
    void on_listLink_itemClicked(QListWidgetItem *item);
    void on_tableWidget_cellDoubleClicked(int row, int column);
    void on_btnCmd_clicked();
    void on_btnClear_clicked();
private:
    Ui::MainWindow *ui=nullptr;
    MainWindowPrivate* d=nullptr;
};
#endif // MAINWINDOW_H
