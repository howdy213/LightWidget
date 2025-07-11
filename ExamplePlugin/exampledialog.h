/**
 * @file exampledialog.h
 * @brief 示例对话框类头文件
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
#ifndef EXAMPLEDIALOG_H
#define EXAMPLEDIALOG_H
#include <QDialog>

namespace Ui {
class ExampleDialog;
}
class ExamplePlugin;
class ExampleDialog : public QWidget
{
    Q_OBJECT
public:
    explicit ExampleDialog(QWidget *parent = nullptr);
    ~ExampleDialog();
    void setCommand(QMap<QString,QVariant> map);
    void appCommand(QMap<QString,QVariant> map);
    void appCommand(QString str);
    ExamplePlugin* plugin=nullptr;
private:
    Ui::ExampleDialog *ui;
};

#endif // EXAMPLEDIALOG_H
