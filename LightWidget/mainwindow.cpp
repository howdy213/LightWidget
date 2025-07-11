/**
 * @file mainwindow.cpp
 * @brief 主窗口实现文件
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
#include"ui_mainwindow.h"
#include"aboutwindow.h"
#include"lightwidget.h"
#include"lightsystem.h"
#include"../WidgetExplorerSDK/WFile/wpath.h"
#include"../WidgetExplorerSDK/WE/we.h"
#include"../WidgetExplorerSDK/WE/webase.h"
#include"../WidgetExplorerSDK/WPlugin/wwidgetmanager.h"
#include"../WidgetExplorerSDK/WConfig/wconfigdocument.h"

#include<QButtonGroup>
#include<QPluginLoader>
#include<QLockFile>
#include<QFile>

class MainWindowPrivate{
public:
    LightWidget* ptr=nullptr;
    QLockFile* lock=nullptr;
    LightSystem* sys=nullptr;
    AboutWindow* aboutWnd=nullptr;
};

///
/// \brief MainWindow::MainWindow
/// \param param
/// \param ptr
/// \param parent
///
MainWindow::MainWindow(QStringList param,LightWidget* ptr, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    this->d=new MainWindowPrivate;
    d->ptr=ptr;
    d->sys=new LightSystem;
    auto initAutorun=[this,param,ptr]()->auto{if(param.length()>=1)if(param[0]=="autorun")ptr->getWEClass()->configManager()->set("autorun",true);
        if(!qvariant_cast<bool>(ptr->getWEClass()->configManager()->get("autorun")))show();
    };

    ui->setupUi(this);
    initWindow();
    initAutorun();
    initPlugin();
    initList();
    initTable();
    initMenu();
    d->ptr->getWEClass()->widgetManager()->initWidget();
}
///
/// \brief MainWindow::~MainWindow
///
MainWindow::~MainWindow()
{
    delete ui;
    ui=nullptr;
    delete d->ptr;
    d->ptr=nullptr;
    delete d->lock;
    d->lock=nullptr;
    delete d->sys;
    d->sys=nullptr;
    delete d->aboutWnd;
    d->aboutWnd=nullptr;
}
///
////// \brief MainWindow::initWindow
///
void MainWindow::initWindow(){
    this->resize(1200,800);
    this->setMinimumSize(800,566);

    QPalette pal(this->palette());
    pal.setColor(QPalette::ColorRole::Window, QColor(255,255,255));
    this->setAutoFillBackground(true);
    this->setPalette(pal);
}
///
/// \brief MainWindow::initPlugin
///
void MainWindow::initPlugin(){
    d->sys->loadAllPlugin();
    auto plugins=WE::inst()->getWEClass()->pluginManager()->allPluginsInst();
    foreach(auto plugin,plugins)
    {
        WPluginInterface *app = plugin->inst();
        if (app)
        {
            if(qvariant_cast<QString>(plugin->getMetaData("type"))=="exe"){
                if(qvariant_cast<QString>(plugin->getMetaData("init"))=="default"){
                    continue;
                }
            };
            QMap<QString,QVariant> data;
            QVariant var;
            QVariant var2;
            var.setValue((WEBase*)d->ptr);
            var2.setValue(plugin);
            data.insert("WE",var);
            data.insert("Plugin",var2);
            app->init(data);
        }
    }
}
///
/// \brief MainWindow::initList
///
void MainWindow::initList(){
    ui->listLink->addItems(ReadLinkFile());
}
///
/// \brief MainWindow::initTable
///
void MainWindow::initTable(){
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->setSelectionMode(QAbstractItemView::NoSelection);

    QFont font;
    font.setPointSize(10);
    font.setFamily("黑体");

    QColor color(0,0,0);

    int colCount=4;

    ui->tableWidget->setColumnCount(colCount);

    createCol(0,"插件名",font,color);
    createCol(1,"版本",font,color);
    createCol(2,"作者",font,color);
    createCol(3,"路径",font,color);

    auto list=WE::inst()->getWEClass()->pluginManager()->allPluginsInst();
    ui->tableWidget->setRowCount(list.length());
    auto it=list.begin();
    for(int i=0;i<=list.length()-1;i++){
        createRow(i,*it);
        it++;
    }
}
///
/// \brief MainWindow::initMenu
///
void MainWindow::initMenu(){
    QMenuBar* menuBar=this->menuBar();
    QMenu* menuPlugin=new QMenu("插件");
    QAction* actNew=new QAction("创建新插件");
    QAction* actImport=new QAction("导入现有插件");
    QAction* actExport=new QAction("导出");
    QMenu* menuOption=new QMenu("选项");
    QAction* actSetting=new QAction("设置");
    QAction* actReset=new QAction("重启");
    QMenu* menuAbout=new QMenu("关于");
    QAction* actAbout=new QAction("关于");

    menuBar->addMenu(menuPlugin);
    menuPlugin->addAction(actNew);
    menuPlugin->addAction(actImport);
    menuPlugin->addAction(actExport);
    menuBar->addMenu(menuOption);
    menuOption->addAction(actSetting);
    menuOption->addAction(actReset);
    menuBar->addMenu(menuAbout);
    menuAbout->addAction(actAbout);

    connect(actAbout,&QAction::triggered,this,&MainWindow::about);
    connect(actReset,&QAction::triggered,this,&MainWindow::restart);
    connect(actSetting,&QAction::triggered,this,[](){
        WPath().ShellExe(WPath().getModuleFolder()+"config/config.json");
    });
}
///
/// \brief MainWindow::createCol
/// \param col
/// \param title
/// \param font
/// \param color
///
void MainWindow::createCol(int col,QString title,QFont font,QColor color){
    QTableWidgetItem*item=new QTableWidgetItem(title);
    item->setFont(font);
    item->setForeground(QBrush(color));
    item->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    ui->tableWidget->setHorizontalHeaderItem(col,item);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(col,QHeaderView::ResizeToContents);
}
///
/// \brief MainWindow::createRow
/// \param row
/// \param info
///
void MainWindow::createRow(int row,  WPlugin* info){
    if(row>=ui->tableWidget->rowCount())return;
    QTableWidgetItem*item=nullptr;
    QString str;
    QStringList list={W_PLUGIN_ATTR_NAME,
                         W_PLUGIN_ATTR_VERSION,
                        W_PLUGIN_ATTR_AUTHOR,
                        W_PLUGIN_ATTR_PATH};
    for(int i=0;i<=list.length()-1;i++){
        str=qvariant_cast<QString>(info->getMetaData(list[i]));
        item=new QTableWidgetItem(str);
        item->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget->setItem(row,i,item);
    }
}
///
/// \brief MainWindow::ReadLinkFile
/// \return
///
QStringList MainWindow::ReadLinkFile(){
    QFile file(WPath().getModuleFolder()+"widget/link.txt");
    QStringList list;
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        QString str = in.readAll();
        list = str.split('\n');
        file.close();
    }
    return list;
}
void MainWindow::setLockFile(QLockFile* file){
    d->lock=file;
}
///
/// \brief MainWindow::closeEvent
/// \param event
///
void MainWindow::closeEvent(QCloseEvent *event)
{
    event->ignore();
    setVisible(false);
}
///
/// \brief MainWindow::tray
/// \param reason
///
void MainWindow::tray(QSystemTrayIcon::ActivationReason reason){
    switch (reason) {
    case QSystemTrayIcon::DoubleClick:
        if(!this->isVisible())this->showNormal();
        else this->activateWindow();
        break;
    default:
        break;
    }
}
///
/// \brief MainWindow::showPanel
///
void MainWindow::showPanel()
{
    this->show();
    this->raise();
}
///
/// \brief MainWindow::about
///
void MainWindow::about(){
    if(d->aboutWnd==nullptr)d->aboutWnd=new AboutWindow;
    if(d->aboutWnd->isHidden())d->aboutWnd->show();
    d->aboutWnd->activateWindow();
}
///
/// \brief MainWindow::reset
///
void MainWindow::restart(){
    d->lock->unlock();
    Sleep(10);
    WPath().ShellExe(WPath().getModuleFolder()+"LightWidgetLauncher.exe","open","-t 100 -r");
    ::TerminateProcess(::GetCurrentProcess(), 0);
}
///
/// \brief MainWindow::on_listLink_itemClicked
/// \param item
///
void MainWindow::on_listLink_itemClicked(QListWidgetItem *item)
{
    ui->editCmd->setText(item->text());

}
///
/// \brief MainWindow::on_tableWidget_cellDoubleClicked
/// \param row
/// \param column
///
void MainWindow::on_tableWidget_cellDoubleClicked(int row, int column)
{
    auto item=ui->tableWidget->item(row,column);
    ui->editCmd->setText(item->text());
}
///
/// \brief MainWindow::on_btnCmd_clicked
///
void MainWindow::on_btnCmd_clicked()
{
    QMap<QString,QVariant>map;
    QString str=ui->editCmd->text();
    int i=0;
    for(;i<=str.size()-1;i++){
        if(str[i]==' ')break;
    }
    map.insert(W_DATA_COMMAND,str.mid(i+1));
    d->ptr->sendMsgs(str.mid(0,i),map);
}
///
/// \brief MainWindow::on_btnClear_clicked
///
void MainWindow::on_btnClear_clicked()
{
    ui->editCmd->clear();
}
