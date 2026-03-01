/**
 * @file aboutwindow.cpp
 * @brief 关于窗口实现文件
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
#include "aboutwindow.h"
#include "ui_aboutwindow.h"

///
/// \brief AboutWindow::AboutWindow
/// \param parent
///
AboutWindow::AboutWindow(QWidget *parent)
    : QWidget(parent), ui(new Ui::AboutWindow) {
    ui->setupUi(this);
    ui->labelLink->setOpenExternalLinks(true);
}
///
/// \brief AboutWindow::~AboutWindow
///
AboutWindow::~AboutWindow() { delete ui; }
