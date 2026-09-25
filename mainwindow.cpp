/**
 * @file mainwindow.cpp
 * @brief Implementation file for the main window.
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
#include "mainwindow.h"
#include "WECore/metadata/wmetadocument.h"
#include "WECore/plugin/wpluginmanager.h"
#include "WECore/utils/flowlayout.h"
#include "aboutwindow.h"
#include "ui_mainwindow.h"

#include "WECore/config/WConfig.h"
#include "WECore/config/WConfigCustomType.h"
#include "WECore/config/WConfigTemplate.h"
#include "WECore/config/WConfigItemWidget.h"
#include "WECore/config/WConfigLayout.h"
#include "WECore/config/WConfigWidget.h"
#include "WECore/def/wedef.h"
#include "WECore/utils/wpath.h"
#include "WECore/utils/wshellexecute.h"
#include "WECore/metadata/wmetadocument.h"
#include "WECore/plugin/wplugindata.h"
#include "WECore/plugin/wpluginmanager.h"
#include "WECore/plugin/wpluginstatemachine.h"
#include "WECore/style/wstyle.h"
#include "WECore/we/we.h"
#include "WECore/we/webase.h"
#include "WECore/widget/wwidgetmanager.h"

#include <QButtonGroup>
#include <QFile>
#include <QHBoxLayout>
#include <QLabel>
#include <QLockFile>
#include <QMenu>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPluginLoader>
#include <QSlider>
#include <QTimer>
#include <QToolBar>
#include <QVBoxLayout>

using namespace we::Consts;
using namespace we::config;
using namespace we;

// Registers the "range" custom type on first use; returns whether it is available.
static bool ensureRangeTypeRegistered() {
    auto &reg = we::config::WConfigCustomTypeRegistry::instance();
    if (reg.hasType("range"))
        return true;
    we::config::WConfigCustomType range;
    range.typeName = QStringLiteral("range");
    range.serialize = [](const QVariant &v) { return v.toInt(); };
    range.deserialize = [](const QVariant &v) { return v.toInt(); };
    range.defaultValue = 50;
    range.displayString = [](const QVariant &v) {
        return QString("%1%").arg(v.toInt());
    };
    range.editorFactory = [](QWidget *parent, we::config::WCustomEditorInterface **out) {
        auto *editor = new RangeSliderEditor(parent);
        if (out)
            *out = editor;
        return static_cast<QWidget *>(editor);
    };
    return reg.registerType(range);
}

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
        toolbar = addToolBar(tr("Plugin Toolbar"));
    }
    toolbar->addAction(action);
}

/**
 * @brief Adds a dock widget as an extension.
 * @param dock The dock widget to add.
 */
void MainWindow::addExtensionDock(QDockWidget *dock) {
    if (!dock)
        return;
    addDockWidget(Qt::RightDockWidgetArea, dock);
}

/**
 * @brief Returns a pointer to the UI object.
 * @return Pointer to the main window's UI.
 */
Ui::MainWindow *MainWindow::getUiPointer() { return d->ui; }

/**
 * @brief Initializes window properties (size, background, etc.).
 */
void MainWindow::initWindow() {
    this->resize(1200, 800);
    this->setMinimumSize(800, 566);

    QPalette pal(this->palette());
    pal.setColor(QPalette::ColorRole::Window, QColor(255, 255, 255));
    this->setAutoFillBackground(true);
    this->setPalette(pal);
}

/**
 * @brief Loads and initializes all plugins.
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

    QFont font;
    font.setPointSize(10);
    font.setFamily(QStringLiteral("SimHei"));

    QColor color(0, 0, 0);

    int colCount = 4;

    d->ui->tablePlugin->setColumnCount(colCount);

    createCol(0, tr("Name"), font, color);
    createCol(1, tr("Version"), font, color);
    createCol(2, tr("Author"), font, color);
    createCol(3, tr("Path"), font, color);

    auto list = WE::inst()->getWEClass()->pluginManager()->allPluginsInst();
    d->ui->tablePlugin->setRowCount(list.length());
    auto it = list.begin();
    for (int i = 0; i <= list.length() - 1; i++) {
        createRow(i, *it);
        it++;
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
    font.setFamily(QStringLiteral("SimHei"));
    QColor color(0, 0, 0);

    const int colCount = 2;
    d->ui->tableWidget->setColumnCount(colCount);

    QTableWidgetItem *headerName = new QTableWidgetItem(tr("Name"));
    headerName->setFont(font);
    headerName->setForeground(QBrush(color));
    headerName->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    d->ui->tableWidget->setHorizontalHeaderItem(0, headerName);
    d->ui->tableWidget->horizontalHeader()->setSectionResizeMode(
        0, QHeaderView::ResizeToContents);

    QTableWidgetItem *headerDesc = new QTableWidgetItem(tr("Description"));
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
    QMenu *menuPlugin = new QMenu(tr("Plugin"));
    QAction *actNew = new QAction(tr("Create Plugin"));
    QAction *actImport = new QAction(tr("Import Plugin"));
    QAction *actExport = new QAction(tr("Export Plugin"));
    QAction *actShowPluginManager = new QAction(tr("Show Plugin Manager"));
    QAction *actConfigManager = new QAction(tr("Restart as Plugin Config Editor"));
    QMenu *menuOption = new QMenu(tr("Options"));
    QAction *actSetting = new QAction(tr("Settings"));
    QAction *actReset = new QAction(tr("Restart"));
    QMenu *menuAbout = new QMenu(tr("About"));
    QAction *actAbout = new QAction(tr("About"));

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
void MainWindow::createRow(int row, WPlugin *info) {
    if (row >= d->ui->tablePlugin->rowCount())
        return;
    QTableWidgetItem *item = nullptr;
    QString str;
    QStringList list = {Plugin::Name, Plugin::Version, Plugin::Author,
                        Plugin::Path};
    for (int i = 0; i <= list.length() - 1; i++) {
        str = qvariant_cast<QString>(info->getMetaData(list[i]));
        item = new QTableWidgetItem(str);
        item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        d->ui->tablePlugin->setItem(row, i, item);
    }
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
    this->show();
    this->raise();
}

/**
 * @brief Opens the About dialog.
 */
void MainWindow::about() {
    if (d->aboutWnd == nullptr)
        d->aboutWnd = new AboutWindow;
    if (d->aboutWnd->isHidden())
        d->aboutWnd->show();
    d->aboutWnd->activateWindow();
}

/**
 * @brief Restarts the application via launcher.
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
    // Currently open settings dialog: an Action callback writes values back into
    // the data and then has to refresh what the dialog shows.
    static WConfigWidget *s_settingsWidget = nullptr;
    if (!config) {
        qWarning() << "Failed to get WConfig instance.";
        return;
    }

    // Register the "range" custom type (a std::function editor, so no subclassing
    // is needed); "path" is registered by the framework itself.
    ensureRangeTypeRegistered();

    // The host template only describes what this dialog adds on top of the file:
    // the "Test" demonstration page and the root meta data. The application-level
    // entries of config.json are declared by the sub-config created further down.
    WConfigTemplate configTemplate;

    // ---- Feature demonstrations, all hosted by the "Test" sub-page ----

    // Members of a merged config group (they share a single title inside the sub
    // viewer). Brightness is a scalar int rendered Inline, with the title placed
    // to the left of the editor.
    configTemplate.addInt("Test", "Brightness",
                          WConfigItemInfo().defaultValue(128).displayName(
                              tr("Brightness")));
    // Theme: a Select combo box, also Inline.
    configTemplate.addSelect(
        "Test", "Theme",
        WConfigItemInfo().defaultValue("light").displayName(tr("Theme"))
            .options(QStringList() << "light" << "dark"));
    // Custom type "path" (the built-in path browser), Inline.
    configTemplate.addCustom(
        "Test", "DataDir",
        WConfigItemInfo().defaultValue(QString()).displayName(
            tr("Data directory"))
            .description(
                tr("Directory browsing through the built-in \"path\" custom type"))
            .defaultItem("path"));
    // Custom type "range" (registered above through std::function), Inline and
    // marked as requiring a restart.
    configTemplate.addCustom(
        "Test", "Range",
        WConfigItemInfo().defaultValue(50).displayName(tr("Volume"))
            .description(
                tr("Slider custom type registered through std::function"))
            .property(Property::RestartRequired).defaultItem("range"));
    // Action member rendered ButtonOnly: the button is shown, its title is not.
    configTemplate.addAction(
        "Test", "Reset",
        WConfigItemInfo().displayName(tr("Restore display defaults"))
            .description(tr("Demonstrates ButtonOnly: button only, no title"))
            .callback([]() {
                // Invoked from the settings dialog: writes the group values back
                // to their defaults.
                if (config) {
                    config->setTemporaryValue("Test/Brightness", 128);
                    config->setTemporaryValue("Test/Theme", QLatin1String("light"));
                    config->setTemporaryValue("Test/Range", 50);
                    if (s_settingsWidget)
                        s_settingsWidget->refreshDisplay();
                }
            }));

    // Standalone items outside any group: they keep the raw Array / Object
    // rendering (Full).
    configTemplate.addArray(
        "Test", "Presets",
        WConfigItemInfo().defaultValue(QVariantList() << "default" << "custom")
            .displayName(tr("Presets"))
            .description(tr("Array item keeping the raw rendering")));
    WConfigDataObject *mapping = configTemplate.addObject(
        "Test", "Mapping",
        WConfigItemInfo().displayName(tr("Mapping"))
            .description(tr("Object item keeping the raw rendering")));
    if (mapping) {
        configTemplate.addObjectChild(
            mapping, we::config::createDataByType(
                         DataType::String, "title",
                         QLatin1String("WindowTitle"),
                         WConfigItemInfo().displayName(tr("Title"))));
    }
    // Standalone item without a description: in Full mode the description label
    // stays hidden.
    configTemplate.addString(
        "Test", "NoDesc",
        WConfigItemInfo().defaultValue("hello").displayName(
            tr("Item without description")));

    configTemplate.setViewerMeta("", tr("Settings"),
                                 tr("Settings root directory"));
    configTemplate.setViewerMeta("Test", tr("Extended Test"),
                                 tr("Config feature demonstrations"));

    // config/config.json is described by a sub-config of its own: the
    // application-level entries and the Style section belong to the file, not to
    // this dialog. The sub-config has no storage backend; it is mounted at the
    // root of the host config, so its directories and items become direct members
    // of the root and the file keeps the flat layout the application reads at
    // startup. The template has to outlive the sub-config, because the document
    // only keeps a pointer to it.
    static WConfig *jsonConfig = nullptr;
    static WConfigTemplate *jsonTemplate = nullptr;
    if (!jsonConfig) {
        const QString mainWidget = WE::inst()
                                       ->getWEClass()
                                       ->configManager()
                                       ->get(Config::DefaultMain)
                                       .toString();
        jsonTemplate = new WConfigTemplate;
        jsonTemplate->addString(
            "", Config::DefaultMain,
            WConfigItemInfo().defaultValue(mainWidget).displayName(
                tr("Local id of the main widget")));
        // The application reads this entry at startup and installs the matching
        // translation, so it only takes effect after a restart.
        jsonTemplate->addSelect(
            "", Config::Language,
            WConfigItemInfo().defaultValue("zh_CN").displayName(tr("UI language"))
                .description(tr("Takes effect after a restart"))
                .options(QStringList() << "zh_CN" << "en_US")
                .property(Property::RestartRequired));
        jsonTemplate->addDouble(
            "", Config::Scale,
            WConfigItemInfo().defaultValue(1.0).decimalPlaces(1).displayName(
                tr("Scale")));
        jsonTemplate->addInt(
            "", Config::Font, WConfigItemInfo().defaultValue(96).displayName(
                tr("Font scaling")));
        // WStyle supplies the "Style" section template; it holds no config itself.
        we::style::WStyle::buildTemplate(*jsonTemplate, Config::Style);
        jsonConfig = new WConfig;
        jsonConfig->applyTemplate(jsonTemplate);
    }
    if (!jsonConfig->isMounted())
        config->mountSubConfig(QString(), jsonConfig); // must precede initialize()
    config->initialize(WPath().getModuleFolder() + Config::ConfigPath,
                       &configTemplate);

    // Register the merged config group on the "Test" sub viewer: one shared title
    // plus a render mode per member.
    auto *root = config->document()->root();
    WConfigViewer *testViewer = root->findChildViewer("Test");
    WConfigGroupSpec display;
    display.groupId = "display";
    display.displayName = tr("Display settings");
    display.description =
        tr("Merged config demo: several items share a single title.");
    display.members = {
        {"Brightness", WConfigItemMode::Inline},
        {"Theme", WConfigItemMode::Inline},
        {"DataDir", WConfigItemMode::Inline},
        {"Range", WConfigItemMode::Inline},
        {"Reset", WConfigItemMode::ButtonOnly},
        {"Presets", WConfigItemMode::Full}
    };
    testViewer->addGroup(display);

    // The root page uses no page layout descriptor. An untitled group carries
    // "Scale" and "Font" (an empty displayName hides the group title while each
    // member keeps its own) and supplies a horizontal layout through its
    // layoutBuilder at creation time, so the two items sit side by side.
    WConfigGroupSpec rootRow;
    rootRow.groupId = "rootRow";
    rootRow.displayName = QString();
    rootRow.description =
        tr("Untitled group plus a creation-time layout builder: two items side by side");
    rootRow.members = {
        {"Scale", WConfigItemMode::Inline},
        {"Font", WConfigItemMode::Inline}
    };
    // Full group-level custom layout: called once when the group is created; the
    // group takes over the returned layout.
    rootRow.layoutBuilder = [](we::config::WConfigGroupWidget *group) -> QLayout * {
        auto *row = new QHBoxLayout;
        row->setContentsMargins(0, 0, 0, 0);
        row->setSpacing(we::config::Layout::RowSpacing);
        for (const QString &key : {QStringLiteral("Scale"),
                                   QStringLiteral("Font")}) {
            if (auto *item = group->memberWidget(key))
                row->addWidget(item, 1); // equal widths, side by side
        }
        return row;
    };
    root->addGroup(rootRow);

    auto *settingsWidget = new WConfigWidget(config, this);
    s_settingsWidget = settingsWidget;
    // Destroyed on close, which also clears the static pointer so it never dangles.
    connect(settingsWidget, &QObject::destroyed, this,
            []() { s_settingsWidget = nullptr; });

    // Item-level layout adjuster: the framework creates the ItemWidget and then
    // calls this back, so a single item can be tuned in place. It applies to group
    // members as well, which is why "Brightness" is used here.
    settingsWidget->setItemAdjuster(
        "Test", "Brightness",
        [](we::config::WConfigItemWidget *item) {
            // The item already exists and is bound to its data; only its vertical
            // margins are changed here.
            if (QLayout *layout = item->layout())
                layout->setContentsMargins(0, 10, 0, 10);
        });
    settingsWidget->setWindowTitle(tr("Settings"));
    settingsWidget->resize(760, 560);
    // Delete on close: destroying the widget releases the root viewer lock,
    // otherwise the lock count would grow every time the dialog is opened.
    settingsWidget->setAttribute(Qt::WA_DeleteOnClose);
    // A successful save may have changed the appearance: re-read the Style
    // section (contributed to the host root by the sub-config above) and apply it
    // right away.
    connect(settingsWidget, &QDialog::accepted, settingsWidget, []() {
        if (auto *style = we::style::WStyle::active())
            style->applyFromValues(config->document()
                                       ->toVariant()
                                       .toMap()
                                       .value(Config::Style)
                                       .toMap());
    });
    settingsWidget->show();
    config->setParent(this);

    // Both timers act on the "Test" sub-page: they demonstrate the dynamic
    // read-only flag and a manual display refresh. After 2s Brightness becomes
    // read-only; after 4s Range is changed from the outside and the page is
    // refreshed by hand (nothing is written to disk until Save is pressed).
    QTimer::singleShot(2000, settingsWidget, [settingsWidget]() {
        config->setItemReadOnly("Test/Brightness", true); // disable the editor now
    });
    QTimer::singleShot(4000, settingsWidget, [settingsWidget]() {
        config->setTemporaryValue("Test/Range", 70); // change the data from outside
        settingsWidget->refreshDisplay();            // refresh the display by hand
    });
}

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
    dialog.setWindowTitle(tr("Plugin Manager"));
    dialog.setModal(true);
    dialog.resize(800, 500);

    QVBoxLayout *mainLayout = new QVBoxLayout(&dialog);

    QHBoxLayout *contentLayout = new QHBoxLayout();
    mainLayout->addLayout(contentLayout);

    // Plugin list on the left.
    QListWidget *pluginList = new QListWidget(&dialog);
    pluginList->setFixedWidth(220);
    contentLayout->addWidget(pluginList);

    // Metadata table on the right.
    QTableWidget *metaTable = new QTableWidget(&dialog);
    metaTable->setColumnCount(2);
    metaTable->setHorizontalHeaderLabels(QStringList() << tr("Property")
                                                       << tr("Value"));
    metaTable->horizontalHeader()->setSectionResizeMode(
        0, QHeaderView::ResizeToContents);
    metaTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    metaTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    metaTable->setEditTriggers(QAbstractItemView::DoubleClicked |
                               QAbstractItemView::EditKeyPressed);
    contentLayout->addWidget(metaTable);

    // Plugin manager.
    auto manager = PClass->pluginManager();
    auto plugins = manager->allPluginsInst();

    // Display text of a plugin: its name (or uuid when it has none) plus state.
    auto pluginDisplayText = [](WPlugin *plugin) -> QString {
        QString name = plugin->name();
        if (name.isEmpty())
            name = plugin->uuid().toString();
        QString state = WPluginStateMachine::stateToString(plugin->getState());
        return QString("%1(%2)").arg(name, state);
    };

    // Fill the list.
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

    // Fill the metadata table.
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

    // Repopulate the table whenever the selection changes.
    QObject::connect(pluginList, &QListWidget::currentItemChanged,
                     [&](QListWidgetItem *current, QListWidgetItem *previous) {
        Q_UNUSED(previous);
        if (!current)
            return;
        WPlugin *plugin = reinterpret_cast<WPlugin *>(
            current->data(Qt::UserRole).value<quintptr>());
        updateMetaTable(plugin);
    });

    // Select the first row by default.
    if (pluginList->count() > 0) {
        pluginList->setCurrentRow(0);
    }

    // Bottom button row.
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *loadButton = new QPushButton(tr("Load"), &dialog);
    QPushButton *unloadButton = new QPushButton(tr("Unload"), &dialog);
    QPushButton *saveButton = new QPushButton(tr("Save Changes"), &dialog);
    QPushButton *refreshButton = new QPushButton(tr("Refresh"), &dialog);
    QPushButton *closeButton = new QPushButton(tr("Close"), &dialog);

    buttonLayout->addWidget(loadButton);
    buttonLayout->addWidget(unloadButton);
    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(refreshButton);
    buttonLayout->addWidget(closeButton);
    buttonLayout->addStretch();
    mainLayout->addLayout(buttonLayout);

    // Load
    QObject::connect(loadButton, &QPushButton::clicked, [&]() {
        QListWidgetItem *current = pluginList->currentItem();
        if (!current)
            return;
        WPlugin *plugin = reinterpret_cast<WPlugin *>(
            current->data(Qt::UserRole).value<quintptr>());
        if (plugin && !plugin->available()) {
            manager->loadPlugin(plugin);
            manager->initPlugin(plugin);
            // Refresh the row after the state changed.
            current->setText(pluginDisplayText(plugin));
            updateMetaTable(plugin);
        }
    });

    // Unload
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

    // Save changes
    QObject::connect(saveButton, &QPushButton::clicked, [&]() {
        QListWidgetItem *current = pluginList->currentItem();
        if (!current)
            return;
        WPlugin *plugin = reinterpret_cast<WPlugin *>(
            current->data(Qt::UserRole).value<quintptr>());
        if (!plugin)
            return;

        // Apply every edited cell to the plugin.
        for (int row = 0; row < metaTable->rowCount(); ++row) {
            QTableWidgetItem *keyItem = metaTable->item(row, 0);
            QTableWidgetItem *valueItem = metaTable->item(row, 1);
            if (keyItem && valueItem) {
                QString key = keyItem->text();
                QString value = valueItem->text();
                plugin->setMetaData(key, value);
            }
        }

        // Persist the metadata to the plugin config file.
        QString configPath = plugin->configPath();
        if (!configPath.isEmpty()) {
            plugin->getMetaDocument().save(configPath);
            QMessageBox::information(&dialog, tr("Save"),
                                     tr("The changes have been saved."));
        } else {
            QMessageBox::warning(
                &dialog, tr("Save"),
                tr("The plugin configuration file path is not available."));
        }
    });

    QObject::connect(refreshButton, &QPushButton::clicked, [&]() {
        refreshList();
    });

    QObject::connect(closeButton, &QPushButton::clicked, &dialog,
                     &QDialog::accept);

    dialog.exec();
}

/**
 * @brief Restarts the application in plugin manager mode.
 */
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

    for (auto plugin : std::as_const(plugins)) {
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
 * @brief Handles double-click on plugin table: sets clicked text to command
 * line.
 * @param row Row index.
 * @param column Column index.
 */
void MainWindow::on_tablePlugin_cellDoubleClicked(int row, int column) {
    auto item = d->ui->tablePlugin->item(row, column);
    d->ui->editCmd->setText(item->text());
}

/**
 * @brief Executes the command entered in the edit field.
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