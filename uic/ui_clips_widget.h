/********************************************************************************
** Form generated from reading UI file 'clips_widget.ui'
**
** Created by: Qt User Interface Compiler version 6.2.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CLIPS_WIDGET_H
#define UI_CLIPS_WIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QScrollBar>
#include <QtWidgets/QWidget>
#include <elqt/extension/view.h>

QT_BEGIN_NAMESPACE

class Ui_ClipsWidgetUI
{
public:
    QGridLayout *gridLayout;
    el::QElangViewSignaled *view;
    el::QElangViewSignaled *reel;
    QScrollBar *scroll;

    void setupUi(QWidget *ClipsWidgetUI)
    {
        if (ClipsWidgetUI->objectName().isEmpty())
            ClipsWidgetUI->setObjectName(QString::fromUtf8("ClipsWidgetUI"));
        ClipsWidgetUI->resize(822, 561);
        gridLayout = new QGridLayout(ClipsWidgetUI);
        gridLayout->setSpacing(2);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        view = new el::QElangViewSignaled(ClipsWidgetUI);
        view->setObjectName(QString::fromUtf8("view"));

        gridLayout->addWidget(view, 0, 0, 1, 1);

        reel = new el::QElangViewSignaled(ClipsWidgetUI);
        reel->setObjectName(QString::fromUtf8("reel"));
        reel->setMaximumSize(QSize(16777215, 100));

        gridLayout->addWidget(reel, 2, 0, 1, 1);

        scroll = new QScrollBar(ClipsWidgetUI);
        scroll->setObjectName(QString::fromUtf8("scroll"));
        scroll->setOrientation(Qt::Horizontal);

        gridLayout->addWidget(scroll, 1, 0, 1, 1);


        retranslateUi(ClipsWidgetUI);

        QMetaObject::connectSlotsByName(ClipsWidgetUI);
    } // setupUi

    void retranslateUi(QWidget *ClipsWidgetUI)
    {
        ClipsWidgetUI->setWindowTitle(QCoreApplication::translate("ClipsWidgetUI", "Clilps Widget", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ClipsWidgetUI: public Ui_ClipsWidgetUI {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CLIPS_WIDGET_H
