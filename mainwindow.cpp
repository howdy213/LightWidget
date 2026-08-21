/**
 * @file mainwindow.cpp
 * @brief Implementation file for the main window.
 * @author howdy213
 * @date 2026-08-20
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
#include "mainwindow.h"
#include "WECore/utils/flowlayout.h"
#include "aboutwindow.h"
#include "ui_mainwindow.h"

#include "WECore/config/WConfig.h"
#include "WECore/config/WConfigTemplate.h"
#include "WECore/config/WConfigWidget.h"
#include "WECore/def/wedef.h"
#include "WECore/file/wpath.h"
#include "WECore/file/wshellexecute.h"
#include "WECore/metadata/WMetaDocument.h"
#include "WECore/plugin/wplugindata.h"
#include "WECore/plugin/wpluginmanager.h"
#include "WECore/plugin/wpluginstatemachine.h"
#include "WECore/we/we.h"
#include "WECore/we/webase.h"
#include "WECore/widget/wwidgetmanager.h"

#include <QAction>
#include <QButtonGroup>
#include <QFile>
#include <QLockFile>
#include <QMessageBox>
#include <QPluginLoader>
#include <QToolBar>

using namespace we::Consts;
using namespace we::config;
using namespace we;

/**
 * @class MainWindowPrivate
 * @brief Private implementation class for MainWindow.
 */
class MainWindowPrivate {
public:
    Ui::MainWindow *ui = nullptr;
    AboutWindow *aboutWnd = nullptr;
};

/**
 * @brief Constructs the main window.
 * @param parent The parent widget.
 */
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    d = new MainWindowPrivate;
    d->ui = new Ui::MainWindow;
    d->ui->setupUi(this);
}

/**
 * @brief Destroys the main window.
 */
MainWindow::~MainWindow() {
    delete d->ui;
    d->ui = nullptr;
    delete d->aboutWnd;
    d->aboutWnd = nullptr;
}

/**
 * @brief Initializes the main window after construction.
 */
void MainWindow::init() {
    initWindow();
    initPlugin();
    initList();
    initTable();
    initWidgetTable();
    initMenu();
    createPluginMenu();
    PClass->widgetManager()->initWidget();
}

/**
 * @brief Adds an action to the plugin toolbar.
 * @param action The action to add.
 */
void MainWindow::addToolBarAction(QAction *action) {
    if (!action)
        return;
    QToolBar *toolbar = findChild<QToolBar *>();
    if (!toolbar) {
        toolbar = addToolBar("插件工具");
        toolbar->setObjectName("PluginToolbar");
    }
    toolbar->addAction(action);
}

/**
 * @brief Adds an extension dock widget.
 * @param dock The dock widget to add.
 */
void MainWindow::addExtensionDock(QDockWidget *dock) {
    if (!dock)
        return;
    addDockWidget(Qt::LeftDockWidgetArea, dock);
}

/**
 * @brief Gets the UI pointer for accessing UI elements.
 * @return Pointer to the UI structure.
 */
Ui::MainWindow *MainWindow::getUiPointer() { return d->ui; }

/**
 * @brief Creates the plugin management menu.
 */
void MainWindow::createPluginMenu() {
    // Plugin menu is already created in initMenu()
}

/**
 * @brief Shows the plugin manager dialog.
 */
void MainWindow::showPluginManager() {
    QDialog dialog(this);
    dialog.setWindowTitle("插件管理");
    dialog.setModal(true);
    dialog.resize(800, 500);

    QVBoxLayout *mainLayout = new QVBoxLayout(&dialog);

    QHBoxLayout *contentLayout = new QHBoxLayout();
    mainLayout->addLayout(contentLayout);

    // 左侧插件列表
    QListWidget *pluginList = new QListWidget(&dialog);
    pluginList->setFixedWidth(220);
    contentLayout->addWidget(pluginList);

    // 右侧元数据表格
    QTableWidget *metaTable = new QTableWidget(&dialog);
    metaTable->setColumnCount(2);
    metaTable->setHorizontalHeaderLabels(QStringList() << "属性" << "值");
    metaTable->horizontalHeader()->setSectionResizeMode(
        0, QHeaderView::ResizeToContents);
    metaTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    metaTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    metaTable->setEditTriggers(QAbstractItemView::DoubleClicked |
                               QAbstractItemView::EditKeyPressed);
    contentLayout->addWidget(metaTable);

    // 获取插件管理器
    auto manager = PClass->pluginManager();
    auto plugins = manager->allPluginsInst();

    // 获取插件显示文本
    auto pluginDisplayText = [](WPlugin *plugin) -> QString {
        QString name = plugin->name();
        if (name.isEmpty())
            name = plugin->uuid().toString();
        QString state = WPluginStateMachine::stateToString(plugin->getState());
        return QString("%1(%2)").arg(name, state);
    };

    // 填充左侧列表
    auto refreshList = [&]() {
        pluginList->clear();
        auto plugins = manager->allPluginsInst();
        for (WPlugin *plugin : std::as_const(plugins)) {
            QListWidgetItem *item =
                new QListWidgetItem(pluginDisplayText(plugin), pluginList);
            item->setData(Qt::UserRole,
                          QVariant::fromValue(reinterpret_cast<quintptr>(plugin)));
        }
        if (pluginList->count() > 0)
            pluginList->setCurrentRow(0);
    };

    refreshList();

    // 更新右侧表格
    auto updateMetaTable = [&](WPlugin *plugin) {
        metaTable->clearContents();
        metaTable->setRowCount(0);

        if (!plugin)
            return;

        QVariantMap metaMap = plugin->getMetaDocument().toMap();
        metaTable->setRowCount(metaMap.size());

        int row = 0;
        for (auto it = metaMap.constBegin(); it != metaMap.constEnd(); ++it) {
            QTableWidgetItem *keyItem = new QTableWidgetItem(it.key());
            keyItem->setFlags(keyItem->flags() & ~Qt::ItemIsEditable);
            QTableWidgetItem *valueItem = new QTableWidgetItem(it.value().toString());
            metaTable->setItem(row, 0, keyItem);
            metaTable->setItem(row, 1, valueItem);
            ++row;
        }
    };

    // 连接列表选择变化信号
    QObject::connect(pluginList, &QListWidget::currentItemChanged,
                     [&](QListWidgetItem *current, QListWidgetItem *previous) {
        Q_UNUSED(previous);
        if (!current)
            return;
        WPlugin *plugin = reinterpret_cast<WPlugin *>(
            current->data(Qt::UserRole).value<quintptr>());
        updateMetaTable(plugin);
    });

    // 默认选中第一个
    if (pluginList->count() > 0) {
        pluginList->setCurrentRow(0);
    }

    // 底部按钮布局
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *loadButton = new QPushButton("加载", &dialog);
    QPushButton *unloadButton = new QPushButton("卸载", &dialog);
    QPushButton *saveButton = new QPushButton("保存修改", &dialog);
    QPushButton *refreshButton = new QPushButton("刷新", &dialog);
    QPushButton *closeButton = new QPushButton("关闭", &dialog);

    buttonLayout->addWidget(loadButton);
    buttonLayout->addWidget(unloadButton);
    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(refreshButton);
    buttonLayout->addWidget(closeButton);
    buttonLayout->addStretch();
    mainLayout->addLayout(buttonLayout);

    // 加载按钮
    QObject::connect(loadButton, &QPushButton::clicked, [&]() {
        QListWidgetItem *current = pluginList->currentItem();
        if (!current)
            return;
        WPlugin *plugin = reinterpret_cast<WPlugin *>(
            current->data(Qt::UserRole).value<quintptr>());
        if (plugin && !plugin->available()) {
            manager->loadPlugin(plugin);
            manager->initPlugin(plugin);
            // 更新列表项显示状态
            current->setText(pluginDisplayText(plugin));
            updateMetaTable(plugin);
        }
    });

    // 卸载按钮
    QObject::connect(unloadButton, &QPushButton::clicked, [&]() {
        QListWidgetItem *current = pluginList->currentItem();
        if (!current)
            return;
        WPlugin *plugin = reinterpret_cast<WPlugin *>(
            current->data(Qt::UserRole).value<quintptr>());
        if (plugin && plugin->available()) {
            manager->hotUnloadPlugin(plugin);
            current->setText(pluginDisplayText(plugin));
            updateMetaTable(plugin);
        }
    });

    // 保存修改按钮
    QObject::connect(saveButton, &QPushButton::clicked, [&]() {
        QListWidgetItem *current = pluginList->currentItem();
        if (!current)
            return;
        WPlugin *plugin = reinterpret_cast<WPlugin *>(
            current->data(Qt::UserRole).value<quintptr>());
        if (!plugin)
            return;

        // 遍历表格，应用修改
        for (int row = 0; row < metaTable->rowCount(); ++row) {
            QTableWidgetItem *keyItem = metaTable->item(row, 0);
            QTableWidgetItem *valueItem = metaTable->item(row, 1);
            if (keyItem && valueItem) {
                QString key = keyItem->text();
                QString value = valueItem->text();
                plugin->setMetaData(key, value);
            }
        }

        // 保存到配置文件
        QString configPath = plugin->configPath();
        if (!configPath.isEmpty()) {
            plugin->getMetaDocument().save(configPath);
            QMessageBox::information(&dialog, "保存", "修改已保存。");
        } else {
            QMessageBox::warning(&dialog, "保存", "无法获取插件配置文件路径。");
        }
    });

    QObject::connect(refreshButton, &QPushButton::clicked, [&]() {
        refreshList();
    });

    QObject::connect(closeButton, &QPushButton::clicked, &dialog,
                     &QDialog::accept);

    dialog.exec();
}

void MainWindow::restartAsPluginManagerMode() {
    WShellExecute::asyncExecute(WPath().getModuleFolder() +
                                    "tools/WELauncher.exe",
                                "open", "-t 500 -cmd=\"--pluginmanager\"");
    QApplication::exit(0);
}

/**
 * @brief Updates the plugin table with current plugin information.
 * @param table The table widget to update.
 */
void MainWindow::updatePluginTable(QTableWidget *table) {
    if (!table)
        return;

    auto manager = PClass->pluginManager();
    auto plugins = manager->allPluginsInst();

    table->setRowCount(plugins.size());
    int row = 0;

    for (auto plugin : plugins) {
        // Plugin name
        QTableWidgetItem *nameItem =
            new QTableWidgetItem(plugin->name());
        table->setItem(row, 0, nameItem);

        // UUID
        QTableWidgetItem *uuidItem =
            new QTableWidgetItem(plugin->uuid().toString());
        table->setItem(row, 1, uuidItem);

        // Type
        QTableWidgetItem *typeItem =
            new QTableWidgetItem(plugin->type());
        table->setItem(row, 2, typeItem);

        // State
        QString stateStr = WPluginStateMachine::stateToString(plugin->getState());
        QTableWidgetItem *stateItem = new QTableWidgetItem(stateStr);
        table->setItem(row, 3, stateItem);

        // Path
        QTableWidgetItem *pathItem =
            new QTableWidgetItem(plugin->path());
        table->setItem(row, 4, pathItem);

        // Author
        QTableWidgetItem *authorItem =
            new QTableWidgetItem(plugin->author());
        table->setItem(row, 5, authorItem);

        // Add custom metadata columns
        for (int col = 6; col < table->columnCount(); ++col) {
            QString key = table->horizontalHeaderItem(col)->text();
            if (plugin->hasMetaData(key)) {
                QTableWidgetItem *metaItem =
                    new QTableWidgetItem(plugin->getMetaData(key).toString());
                table->setItem(row, col, metaItem);
            }
        }

        row++;
    }
}

/**
 * @brief Initializes the window properties.
 */
void MainWindow::initWindow() {
    setWindowTitle("WidgetExplorer");
    setMinimumSize(800, 600);
    this->resize(1200, 800);
    this->setMinimumSize(800, 566);

    QPalette pal(this->palette());
    pal.setColor(QPalette::ColorRole::Window, QColor(255, 255, 255));
    this->setAutoFillBackground(true);
    this->setPalette(pal);
}

/**
 * @brief Initializes the plugin system.
 */
void MainWindow::initPlugin() {
    auto man = PClass->pluginManager();
    auto insts = man->allPluginsInst();
    foreach (auto inst, insts) {
        if (!man->loadPlugin(inst))
            continue;
        man->initPlugin(inst);
    }
}

/**
 * @brief Initializes the list of buttons from the link file.
 */
void MainWindow::initList() {
    QStringList links = ReadLinkFile();
    QWidget *btnContainer = d->ui->btnWidget;
    if (!btnContainer) {
        qWarning() << "btnWidget is null!";
        return;
    }

    QLayout *oldLayout = btnContainer->layout();
    if (oldLayout) {
        QLayoutItem *child;
        while ((child = oldLayout->takeAt(0)) != nullptr) {
            if (child->widget())
                delete child->widget();
            delete child;
        }
        delete oldLayout;
    } else {
        QList<QPushButton *> btns = btnContainer->findChildren<QPushButton *>();
        for (QPushButton *btn : std::as_const(btns))
            delete btn;
    }

    FlowLayout *flowLayout = new FlowLayout(btnContainer, 10, 10, 10);
    btnContainer->setLayout(flowLayout);

    for (const QString &link : std::as_const(links)) {
        if (link.trimmed().isEmpty())
            continue;

        QPushButton *btn = new QPushButton(link, btnContainer);
        btn->setMaximumWidth(200);
        btn->setMinimumHeight(100);
        btn->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
        btn->setToolTip(link);
        connect(btn, &QPushButton::clicked, this,
                [this, link]() { this->d->ui->editCmd->setText(link); });

        flowLayout->addWidget(btn);
    }
}

/**
 * @brief Initializes the plugin information table.
 */
void MainWindow::initTable() {
    d->ui->tablePlugin->setEditTriggers(QAbstractItemView::NoEditTriggers);
    d->ui->tablePlugin->setSelectionMode(QAbstractItemView::NoSelection);

    d->ui->tablePlugin->clear();
    d->ui->tablePlugin->setColumnCount(6);
    QStringList headers;
    headers << "名称" << "UUID" << "类型" << "状态" << "路径" << "作者";
    d->ui->tablePlugin->setHorizontalHeaderLabels(headers);

    auto pluginList = PClass->pluginManager()->allPluginsInst();
    d->ui->tablePlugin->setRowCount(pluginList.size());

    for (int i = 0; i < pluginList.size(); ++i) {
        createRow(i, pluginList[i]);
    }
}

/**
 * @brief Initializes the widget catalog table.
 */
void MainWindow::initWidgetTable() {
    d->ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    d->ui->tableWidget->setSelectionMode(QAbstractItemView::NoSelection);

    QFont font;
    font.setPointSize(10);
    font.setFamily("黑体");
    QColor color(0, 0, 0);

    const int colCount = 2;
    d->ui->tableWidget->setColumnCount(colCount);

    QTableWidgetItem *headerName = new QTableWidgetItem("名称");
    headerName->setFont(font);
    headerName->setForeground(QBrush(color));
    headerName->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    d->ui->tableWidget->setHorizontalHeaderItem(0, headerName);
    d->ui->tableWidget->horizontalHeader()->setSectionResizeMode(
        0, QHeaderView::ResizeToContents);

    QTableWidgetItem *headerDesc = new QTableWidgetItem("描述");
    headerDesc->setFont(font);
    headerDesc->setForeground(QBrush(color));
    headerDesc->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    d->ui->tableWidget->setHorizontalHeaderItem(1, headerDesc);
    d->ui->tableWidget->horizontalHeader()->setSectionResizeMode(
        1, QHeaderView::ResizeToContents);

    auto widgets = PClass->widgetManager()->getWidgets();
    d->ui->tableWidget->setRowCount(widgets.size());

    for (int i = 0; i < widgets.size(); ++i) {
        QObject *widget = widgets[i];
        QString name =
            PClass->widgetManager()->getAttr(widget, Widget::Name).toString();
        QString desc =
            PClass->widgetManager()->getAttr(widget, Widget::Desc).toString();

        QTableWidgetItem *itemName = new QTableWidgetItem(name);
        itemName->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        d->ui->tableWidget->setItem(i, 0, itemName);

        QTableWidgetItem *itemDesc = new QTableWidgetItem(desc);
        itemDesc->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        d->ui->tableWidget->setItem(i, 1, itemDesc);
    }
}

/**
 * @brief Handles double-click on the widget table.
 * @param row Row index.
 * @param column Column index.
 */
void MainWindow::on_tableWidget_cellDoubleClicked(int row, int column) {
    QTableWidgetItem *item = d->ui->tableWidget->item(row, column);
    if (item) {
        d->ui->editCmd->setText(item->text());
    }
}

/**
 * @brief Initializes the menu bar.
 */
void MainWindow::initMenu() {
    QMenuBar *menuBar = this->menuBar();
    QMenu *menuPlugin = new QMenu("插件");
    QAction *actNew = new QAction("创建新插件");
    QAction *actImport = new QAction("导入现有插件");
    QAction *actExport = new QAction("导出插件");
    QAction *actShowPluginManager = new QAction("显示插件管理器");
    QAction *actConfigManager = new QAction("重启至插件配置编辑器");
    QMenu *menuOption = new QMenu("选项");
    QAction *actSetting = new QAction("设置");
    QAction *actReset = new QAction("重启");
    QMenu *menuAbout = new QMenu("关于");
    QAction *actAbout = new QAction("关于");

    menuBar->addMenu(menuPlugin);
    menuPlugin->addAction(actNew);
    menuPlugin->addAction(actImport);
    menuPlugin->addAction(actExport);
    menuPlugin->addAction(actConfigManager);
    menuPlugin->addAction(actShowPluginManager);
    menuBar->addMenu(menuOption);
    menuOption->addAction(actSetting);
    menuOption->addAction(actReset);
    menuBar->addMenu(menuAbout);
    menuAbout->addAction(actAbout);

    connect(actAbout, &QAction::triggered, this, &MainWindow::about);
    connect(actReset, &QAction::triggered, this, &MainWindow::restart);
    connect(actSetting, &QAction::triggered, this, &MainWindow::openSettings);
    connect(actShowPluginManager, &QAction::triggered, this,
            &MainWindow::showPluginManager);
    connect(actConfigManager, &QAction::triggered, this,
            &MainWindow::restartAsPluginManagerMode);
}

/**
 * @brief Creates a column header for the plugin table.
 * @param col Column index.
 * @param title Header text.
 * @param font Font for the header.
 * @param color Color for the header text.
 */
void MainWindow::createCol(int col, QString title, QFont font, QColor color) {
    QTableWidgetItem *item = new QTableWidgetItem(title);
    item->setFont(font);
    item->setForeground(QBrush(color));
    item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    d->ui->tablePlugin->setHorizontalHeaderItem(col, item);
    d->ui->tablePlugin->horizontalHeader()->setSectionResizeMode(
        col, QHeaderView::ResizeToContents);
}

/**
 * @brief Creates a row in the plugin table for a given plugin.
 * @param row Row index.
 * @param info Pointer to the plugin object.
 */
void MainWindow::createRow(int row, we::WPlugin *info) {
    if (!info)
        return;

    // Name
    QTableWidgetItem *nameItem =
        new QTableWidgetItem(info->name());
    d->ui->tablePlugin->setItem(row, 0, nameItem);

    // UUID
    QTableWidgetItem *uuidItem = new QTableWidgetItem(info->uuid().toString());
    d->ui->tablePlugin->setItem(row, 1, uuidItem);

    // Type
    QTableWidgetItem *typeItem =
        new QTableWidgetItem(info->type());
    d->ui->tablePlugin->setItem(row, 2, typeItem);

    // State
    QString stateStr = WPluginStateMachine::stateToString(info->getState());
    QTableWidgetItem *stateItem = new QTableWidgetItem(stateStr);
    d->ui->tablePlugin->setItem(row, 3, stateItem);

    // Path
    QTableWidgetItem *pathItem =
        new QTableWidgetItem(info->path());
    d->ui->tablePlugin->setItem(row, 4, pathItem);

    // Author
    QTableWidgetItem *authorItem =
        new QTableWidgetItem(info->author());
    d->ui->tablePlugin->setItem(row, 5, authorItem);
}

/**
 * @brief Reads the link file and returns a list of links.
 * @return QStringList containing the links.
 */
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

/**
 * @brief Handles the close event (hides the window instead of closing).
 * @param event The close event.
 */
void MainWindow::closeEvent(QCloseEvent *event) {
    event->ignore();
    setVisible(false);
}

/**
 * @brief Handles system tray icon activation.
 * @param reason The activation reason.
 */
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

/**
 * @brief Shows the main window (used from tray context menu).
 */
void MainWindow::showPanel() {
    show();
    activateWindow();
}

/**
 * @brief Shows the about dialog.
 */
void MainWindow::about() {
    if (d->aboutWnd == nullptr)
        d->aboutWnd = new AboutWindow;
    if (d->aboutWnd->isHidden())
        d->aboutWnd->show();
    d->aboutWnd->activateWindow();
}

/**
 * @brief Restarts the application.
 */
void MainWindow::restart() {
    WShellExecute::asyncExecute(
        WPath().getModuleFolder() + "tools/WELauncher.exe", "open", "-t 500");
    QApplication::exit(0);
}

/**
 * @brief MainWindow::openSettings
 */
void MainWindow::openSettings() {
    static WConfig *config = new WConfig;
    if (!config) {
        qWarning() << "Failed to get WConfig instance.";
        return;
    }
    WConfigTemplate configTemplate;
    QString mainWidget = WE::inst()
                             ->getWEClass()
                             ->configManager()
                             ->get(Config::DefaultMain)
                             .toString();
    configTemplate.addString(
        "", Config::DefaultMain,
        WConfigItemInfo().defaultValue(mainWidget).displayName("主控件本地Uuid"));
    configTemplate.addDouble(
        "", Config::Scale,
        WConfigItemInfo().defaultValue(1.0).decimalPlaces(1).displayName("缩放"));
    configTemplate.addInt(
        "", Config::Font,
        WConfigItemInfo().defaultValue(96).displayName("字体缩放"));
    configTemplate.setViewerMeta("", "设置", "设置根目录");
    config->initialize(WPath().getModuleFolder() + Config::ConfigPath,
                       &configTemplate);
    // Create a new config widget for the settings
    static WConfigWidget *settingsWidget = new WConfigWidget(config, this);
    settingsWidget->setWindowTitle("Settings");
    settingsWidget->resize(600, 400);
    settingsWidget->show();
    config->setParent(this);
}

/**
 * @brief Handles the plugin table cell double click event.
 * @param row Row index.
 * @param column Column index.
 */
void MainWindow::on_tablePlugin_cellDoubleClicked(int row, int column) {
    auto item = d->ui->tablePlugin->item(row, column);
    d->ui->editCmd->setText(item->text());
    /*
Q_UNUSED(column);
auto manager = PClass->pluginManager();
auto plugins = manager->allPluginsInst();

if (row >= 0 && row < plugins.size()) {
    WPlugin *plugin = plugins[row];
    if (plugin) {
        // Toggle plugin state
        if (plugin->available()) {
            manager->hotUnloadPlugin(plugin);
        } else {
            manager->loadPlugin(plugin);
        }
        initTable(); // Refresh the table
    }
}
    */
}

/**
 * @brief Handles the command button click event.
 */
void MainWindow::on_btnCmd_clicked() {
    QString str = d->ui->editCmd->text();
    QString topic;
    QString param;

    str = str.trimmed();
    if (str.isEmpty()) {
        return;
    }

    QChar firstChar = str[0];
    if (firstChar == '"') {
        int endIdx = -1;
        for (int i = 1; i < str.length(); ++i) {
            if (str[i] == firstChar && (i == 0 || str[i - 1] != '\\')) {
                endIdx = i;
                break;
            }
        }
        if (endIdx != -1) {
            topic = str.mid(1, endIdx - 1);
            QString remaining = str.mid(endIdx + 1).trimmed();
            param = remaining;
        } else {
            int spaceIdx = str.indexOf(' ');
            if (spaceIdx != -1) {
                topic = str.left(spaceIdx);
                param = str.mid(spaceIdx + 1);
            } else {
                topic = str;
                param = QString();
            }
        }
    } else {
        int spaceIdx = str.indexOf(' ');
        if (spaceIdx != -1) {
            topic = str.left(spaceIdx);
            param = str.mid(spaceIdx + 1);
        } else {
            topic = str;
            param = QString();
        }
    }

    WMessage msg;
    msg.from = WESender;
    msg.command = param;

    WEvent event;
    event.topic = topic;
    event.msg = msg;

    WApp->getWEClass()->widgetManager()->publish(event);
}

/**
 * @brief Clears the command line edit field.
 */
void MainWindow::on_btnClear_clicked() { d->ui->editCmd->clear(); }

/**
 * @brief Refreshes the table content when the tab bar is clicked.
 * @param index Index of the clicked tab.
 */
void MainWindow::on_tabWidget_tabBarClicked(int index) {
    switch (index) {
    case 0: {
        d->ui->tablePlugin->clearContents();
        auto pluginList = PClass->pluginManager()->allPluginsInst();
        d->ui->tablePlugin->setRowCount(pluginList.size());
        for (int i = 0; i < pluginList.size(); ++i) {
            createRow(i, pluginList[i]);
        }
        break;
    }
    case 1: {
        d->ui->tableWidget->clearContents();
        auto widgets = PClass->widgetManager()->getWidgets();
        d->ui->tableWidget->setRowCount(widgets.size());
        for (int i = 0; i < widgets.size(); ++i) {
            QString name =
                PClass->widgetManager()->getAttr(widgets[i], Widget::Name).toString();
            QString desc =
                PClass->widgetManager()->getAttr(widgets[i], Widget::Desc).toString();
            d->ui->tableWidget->setItem(i, 0, new QTableWidgetItem(name));
            d->ui->tableWidget->setItem(i, 1, new QTableWidgetItem(desc));
        }
        break;
    }
    default:
        break;
    }
}