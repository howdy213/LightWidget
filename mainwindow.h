/**
 * @file mainwindow.h
 * @brief Header file for the main window class.
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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "WECore/def/wedef.h"
#include "WECore/plugin/wplugin.h"

#include <QCloseEvent>
#include <QListWidget>
#include <QLockFile>
#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QPushButton>
#include <QHeaderView>

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
    void createPluginMenu();

private:
    void initWindow();
    void initPlugin();
    void initList();
    void initTable();
    void initMenu();

private:
    void initWidgetTable();
    void createCol(int col, QString title, QFont font, QColor color);
    void createRow(int row, we::WPlugin *info);
    QStringList ReadLinkFile();
    void updatePluginTable(QTableWidget *table);

public:
    void closeEvent(QCloseEvent *event) override;
    void tray(QSystemTrayIcon::ActivationReason reason);
    void recMsgs(we::WMessage &msg);
public slots:
    void showPanel();
private slots:
    void about();
    void restart();
    void openSettings();
    void showPluginManager();
    void restartAsPluginManagerMode();
    void on_tablePlugin_cellDoubleClicked(int row, int column);
    void on_tableWidget_cellDoubleClicked(int row, int column);
    void on_btnCmd_clicked();
    void on_btnClear_clicked();
    void on_tabWidget_tabBarClicked(int index);

private:
    MainWindowPrivate *d = nullptr;
};

#include "WECore/config/WConfigCustomType.h"
#include <QLabel>

// Range slider editor: a QSlider plus a value label, implementing the
// WCustomEditorInterface contract (the framework requires the valueEdited signal).
class RangeSliderEditor : public QWidget, public we::config::WCustomEditorInterface {
    Q_OBJECT
    QSlider *m_slider;
    QLabel *m_valueLabel;
public:
    explicit RangeSliderEditor(QWidget *parent = nullptr) : QWidget(parent) {
        QHBoxLayout *layout = new QHBoxLayout(this);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(6);
        // Force the horizontal orientation so the slider is never stretched
        // vertically, and widen the range so demo values above 100 fit.
        m_slider = new QSlider(Qt::Horizontal, this);
        m_slider->setRange(0, 200);
        m_valueLabel = new QLabel(this);
        m_valueLabel->setMinimumWidth(36);
        m_valueLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        layout->addWidget(m_slider, 1);
        layout->addWidget(m_valueLabel);
        connect(m_slider, &QSlider::valueChanged, this,
                [this](int v) {
                    m_valueLabel->setText(QString("%1%").arg(v));
                    emit valueEdited();
                });
    }
    QVariant editValue() const override { return m_slider->value(); }
    void setEditValue(const QVariant &v) override {
        QSignalBlocker blocker(m_slider);
        m_slider->setValue(v.toInt());
        m_valueLabel->setText(QString("%1%").arg(v.toInt()));
    }
    QSize sizeHint() const override { return QSize(200, 28); }
signals:
    void valueEdited();
};

#endif // MAINWINDOW_H