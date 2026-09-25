/********************************************************************************
** Form generated from reading UI file 'aboutwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.11.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ABOUTWINDOW_H
#define UI_ABOUTWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_AboutWindow
{
public:
    QGridLayout *gridLayout;
    QSpacerItem *horizontalSpacer;
    QSpacerItem *horizontalSpacer_2;
    QLabel *labelTitle;
    QLabel *labelLink;
    QSpacerItem *verticalSpacer;

    void setupUi(QWidget *AboutWindow)
    {
        if (AboutWindow->objectName().isEmpty())
            AboutWindow->setObjectName("AboutWindow");
        AboutWindow->resize(300, 240);
        gridLayout = new QGridLayout(AboutWindow);
        gridLayout->setObjectName("gridLayout");
        gridLayout->setContentsMargins(-1, 30, -1, -1);
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        gridLayout->addItem(horizontalSpacer, 1, 0, 1, 1);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        gridLayout->addItem(horizontalSpacer_2, 1, 2, 1, 1);

        labelTitle = new QLabel(AboutWindow);
        labelTitle->setObjectName("labelTitle");
        labelTitle->setMaximumSize(QSize(88, 88));
        QFont font;
        font.setFamilies({QString::fromUtf8("\345\256\213\344\275\223")});
        labelTitle->setFont(font);
        labelTitle->setPixmap(QPixmap(QString::fromUtf8(":/icons/icon/we.png")));
        labelTitle->setScaledContents(true);
        labelTitle->setAlignment(Qt::AlignmentFlag::AlignCenter);
        labelTitle->setMargin(10);

        gridLayout->addWidget(labelTitle, 1, 1, 1, 1);

        labelLink = new QLabel(AboutWindow);
        labelLink->setObjectName("labelLink");
        labelLink->setAlignment(Qt::AlignmentFlag::AlignCenter);

        gridLayout->addWidget(labelLink, 2, 0, 1, 3);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        gridLayout->addItem(verticalSpacer, 4, 0, 1, 3);


        retranslateUi(AboutWindow);

        QMetaObject::connectSlotsByName(AboutWindow);
    } // setupUi

    void retranslateUi(QWidget *AboutWindow)
    {
        AboutWindow->setWindowTitle(QCoreApplication::translate("AboutWindow", "\345\205\263\344\272\216", nullptr));
        labelLink->setText(QCoreApplication::translate("AboutWindow", "<html><head/><body><p><span style=\" font-weight:700;\">LightWidget v2.0.0</span><br/>Copyright \302\251 2025-2026 howdy213. <br/>All rights reserved. <br/>\350\256\270\345\217\257\350\257\201:Apache-2.0 license <br/><a href=\"http://www.github.com/howdy213/WidgetExplorer\"><span style=\" text-decoration: underline; color:#1f9b5d;\">\350\256\277\351\227\256github\344\273\223\345\272\223</span></a></p></body></html>", nullptr));
    } // retranslateUi

};

namespace Ui {
    class AboutWindow: public Ui_AboutWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ABOUTWINDOW_H
