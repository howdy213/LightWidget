/**
 * @file exampledialog.cpp
 * @brief 示例对话框实现文件
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
#include"exampledialog.h"
#include"ui_exampledialog.h"
#include<windows.h>

ExampleDialog::ExampleDialog(QWidget *parent)
    : ui(new Ui::ExampleDialog)
{
    Q_UNUSED(parent);
    ui->setupUi(this);
}

ExampleDialog::~ExampleDialog()
{
    delete ui;
}

void ExampleDialog::setCommand(QMap<QString,QVariant> map){
    ui->textEdit->clear();
    QString str="";
    for(auto it=map.begin();it!=map.end();it++){
        str+=it.key();
        str+=":";
        str+=it->toString();
        str+="\n";
    }
    ui->textEdit->setText(str);
}
void ExampleDialog::appCommand(QMap<QString,QVariant> map){
    QString str="";
    for(auto it=map.begin();it!=map.end();it++){
        str+=it.key();
        str+=":";
        str+=it->toString();
        str+="\n";
    }
    appCommand(str);
}
void ExampleDialog::appCommand(QString str){
    QString org=ui->textEdit->toPlainText();
    str=org+str;
    ui->textEdit->setText(str);
}
