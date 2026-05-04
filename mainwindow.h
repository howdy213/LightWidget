/**
 * @file mainwindow.h
 * @brief Header file for the main window class.
 * @author howdy213
 * @date 2026-05-04
 * @version 2.0.0
 *
 * @copyright Copyright 2025-2026 howdy213
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
#include "WECore/def/wedef.h"
#include "WECore/plugin/wplugin.h"

#include <QCloseEvent>
#include <QListWidget>
#include <QLockFile>
#include <QMainWindow>
#include <QSystemTrayIcon>

namespace Ui {
class MainWindow;
}

class MainWindowPrivate;
/**
 * @class MainWindow
 * @brief The main application window.
 */
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void init();
    void addToolBarAction(QAction *action);
    void addExtensionDock(QDockWidget *dock);
    Ui::MainWindow *getUiPointer();

private:
    void initWindow();
    void initPlugin();
    void initList();
    void initTable();
    void initMenu();

private:
    void createCol(int col, QString title, QFont font, QColor color);
    void createRow(int row, we::WPlugin *info);
    QStringList ReadLinkFile();

public:
    void closeEvent(QCloseEvent *event) override;
    void tray(QSystemTrayIcon::ActivationReason reason);
    void recMsgs(we::WMessage &msg);
public slots:
    void showPanel();
private slots:
    void about();
    void restart();
    void on_tablePlugin_cellDoubleClicked(int row, int column);
    void on_tableWidget_cellDoubleClicked(int row, int column);
    void on_btnCmd_clicked();
    void on_btnClear_clicked();
    void on_tabWidget_tabBarClicked(int index);

private:
    MainWindowPrivate *d = nullptr;
    void initWidgetTable();
};
#endif // MAINWINDOW_H