/**
 * @file mainwindow.cpp
 * @brief 主窗口实现文件
 * @author howdy213
 * @date 2026-1-30
 * @version 1.1.0
 *
 * Copyright 2025-2026 howdy213
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
#include "mainwindow.h"
#include "aboutwindow.h"
#include "ui_mainwindow.h"

#include "WECore/WDef/wedef.h"
#include "WECore/WE/we.h"
#include "WECore/WE/webase.h"
#include "WECore/WFile/wpath.h"
#include "WECore/WFile/wshellexecute.h"
#include "WECore/WPlugin/wwidgetmanager.h"
#include "WPlugin/wplugindata.h"

#include <QButtonGroup>
#include <QFile>
#include <QLockFile>
#include <QPluginLoader>
using namespace we::Consts;

class MainWindowPrivate {
public:
    Ui::MainWindow *ui = nullptr;
    AboutWindow *aboutWnd = nullptr;
};

///
/// \brief MainWindow::MainWindow
/// \param param
/// \param ptr
/// \param parent
///
///
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    d = new MainWindowPrivate;
    d->ui = new Ui::MainWindow;
    d->ui->setupUi(this);
    initWindow();
    initPlugin();
    initList();
    initTable();
    initMenu();
    PClass->widgetManager()->initWidget();
}
///
/// \brief MainWindow::~MainWindow
///
MainWindow::~MainWindow() {
    delete d->ui;
    d->ui = nullptr;
    delete d->aboutWnd;
    d->aboutWnd = nullptr;
}
///
////// \brief MainWindow::initWindow
///
void MainWindow::initWindow() {
    this->resize(1200, 800);
    this->setMinimumSize(800, 566);

    QPalette pal(this->palette());
    pal.setColor(QPalette::ColorRole::Window, QColor(255, 255, 255));
    this->setAutoFillBackground(true);
    this->setPalette(pal);
}
///
/// \brief MainWindow::initPlugin
///
void MainWindow::initPlugin() {
    auto man = PClass->pluginManager();
    auto insts = man->allPluginsInst();
    foreach (auto inst, insts) {
        if (!man->loadPlugin(inst))
            continue;
        man->initPlugin(inst);
    }
}
///
/// \brief MainWindow::initList
///
void MainWindow::initList() { d->ui->listLink->addItems(ReadLinkFile()); }
///
/// \brief MainWindow::initTable
///
void MainWindow::initTable() {
    d->ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    d->ui->tableWidget->setSelectionMode(QAbstractItemView::NoSelection);

    QFont font;
    font.setPointSize(10);
    font.setFamily("黑体");

    QColor color(0, 0, 0);

    int colCount = 4;

    d->ui->tableWidget->setColumnCount(colCount);

    createCol(0, "插件名", font, color);
    createCol(1, "版本", font, color);
    createCol(2, "作者", font, color);
    createCol(3, "路径", font, color);

    auto list = WE::inst()->getWEClass()->pluginManager()->allPluginsInst();
    d->ui->tableWidget->setRowCount(list.length());
    auto it = list.begin();
    for (int i = 0; i <= list.length() - 1; i++) {
        createRow(i, *it);
        it++;
    }
}
///
/// \brief MainWindow::initMenu
///
void MainWindow::initMenu() {
    QMenuBar *menuBar = this->menuBar();
    QMenu *menuPlugin = new QMenu("插件");
    QAction *actNew = new QAction("创建新插件");
    QAction *actImport = new QAction("导入现有插件");
    QAction *actExport = new QAction("导出");
    QMenu *menuOption = new QMenu("选项");
    QAction *actSetting = new QAction("设置");
    QAction *actReset = new QAction("重启");
    QMenu *menuAbout = new QMenu("关于");
    QAction *actAbout = new QAction("关于");

    menuBar->addMenu(menuPlugin);
    menuPlugin->addAction(actNew);
    menuPlugin->addAction(actImport);
    menuPlugin->addAction(actExport);
    menuBar->addMenu(menuOption);
    menuOption->addAction(actSetting);
    menuOption->addAction(actReset);
    menuBar->addMenu(menuAbout);
    menuAbout->addAction(actAbout);

    connect(actAbout, &QAction::triggered, this, &MainWindow::about);
    connect(actReset, &QAction::triggered, this, &MainWindow::restart);
    connect(actSetting, &QAction::triggered, this, []() {
        WShellExecute::syncExecute(WPath().getModuleFolder() + Config::ConfigPath);
    });
}
///
/// \brief MainWindow::createCol
/// \param col
/// \param title
/// \param font
/// \param color
///
void MainWindow::createCol(int col, QString title, QFont font, QColor color) {
    QTableWidgetItem *item = new QTableWidgetItem(title);
    item->setFont(font);
    item->setForeground(QBrush(color));
    item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    d->ui->tableWidget->setHorizontalHeaderItem(col, item);
    d->ui->tableWidget->horizontalHeader()->setSectionResizeMode(
        col, QHeaderView::ResizeToContents);
}
///
/// \brief MainWindow::createRow
/// \param row
/// \param info
///
void MainWindow::createRow(int row, WPlugin *info) {
    if (row >= d->ui->tableWidget->rowCount())
        return;
    QTableWidgetItem *item = nullptr;
    QString str;
    QStringList list = {Plugin::Name, Plugin::Version, Plugin::Author,
                        Plugin::Path};
    for (int i = 0; i <= list.length() - 1; i++) {
        str = qvariant_cast<QString>(info->getMetaData(list[i]));
        item = new QTableWidgetItem(str);
        item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        d->ui->tableWidget->setItem(row, i, item);
    }
}
///
/// \brief MainWindow::ReadLinkFile
/// \return
///
QStringList MainWindow::ReadLinkFile() {
    QFile file(WPath().getModuleFolder() + Plugins::ConfigFolder + "link.txt");
    QStringList list;
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        QString str = in.readAll();
        list = str.split('\n');
        file.close();
    }
    return list;
}
///
/// \brief MainWindow::closeEvent
/// \param event
///
void MainWindow::closeEvent(QCloseEvent *event) {
    event->ignore();
    setVisible(false);
}
///
/// \brief MainWindow::tray
/// \param reason
///
void MainWindow::tray(QSystemTrayIcon::ActivationReason reason) {
    switch (reason) {
    case QSystemTrayIcon::DoubleClick:
        if (!this->isVisible())
            this->showNormal();
        else
            this->activateWindow();
        break;
    default:
        break;
    }
}
///
/// \brief MainWindow::sendMsgs
/// \param widgetName
/// \param map
/// \return
///
bool MainWindow::sendMsgs(QString widgetName, QMap<QString, QVariant> map) {
    WMetaData data;
    data.from = WESender;
    data.dest = widgetName;
    data.map = map;
    return PClass->pluginManager()->sendMsg(data);
}
///
/// \brief MainWindow::recMsgs
/// \param msg
///
void MainWindow::recMsgs(WMetaData &msg) { Q_UNUSED(msg); }
///
/// \brief MainWindow::showPanel
///
void MainWindow::showPanel() {
    this->show();
    this->raise();
}
///
/// \brief MainWindow::about
///
void MainWindow::about() {
    if (d->aboutWnd == nullptr)
        d->aboutWnd = new AboutWindow;
    if (d->aboutWnd->isHidden())
        d->aboutWnd->show();
    d->aboutWnd->activateWindow();
}
///
/// \brief MainWindow::restart
///
void MainWindow::restart() {
    WShellExecute::asyncExecute(WPath().getModuleFolder()+"tools/WELauncher.exe","open","-t 500");
    QApplication::exit(0);
}
///
/// \brief MainWindow::on_listLink_itemClicked
/// \param item
///
void MainWindow::on_listLink_itemClicked(QListWidgetItem *item) {
    d->ui->editCmd->setText(item->text());
}
///
/// \brief MainWindow::on_tableWidget_cellDoubleClicked
/// \param row
/// \param column
///
void MainWindow::on_tableWidget_cellDoubleClicked(int row, int column) {
    auto item = d->ui->tableWidget->item(row, column);
    d->ui->editCmd->setText(item->text());
}
///
/// \brief MainWindow::on_btnCmd_clicked
///
void MainWindow::on_btnCmd_clicked() {
    QMap<QString, QVariant> map;
    QString str = d->ui->editCmd->text();
    int i = 0;
    for (; i <= str.size() - 1; i++) {
        if (str[i] == ' ')
            break;
    }
    map.insert(Data::Command, str.mid(i + 1));
    sendMsgs(str.mid(0, i), map);
}
///
/// \brief MainWindow::on_btnClear_clicked
///
void MainWindow::on_btnClear_clicked() { d->ui->editCmd->clear(); }
