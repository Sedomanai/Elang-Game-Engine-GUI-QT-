/********************************************************************************
** Form generated from reading UI file 'q_elang_logo.ui'
**
** Created by: Qt User Interface Compiler version 6.4.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_Q_ELANG_LOGO_H
#define UI_Q_ELANG_LOGO_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QWidget>
#include <elqt/extension/view.h>

QT_BEGIN_NAMESPACE

class Ui_QElangLogoClass
{
public:
    QGridLayout *gridLayout;
    el::QElangViewSignaled *openGLWidget;

    void setupUi(QWidget *QElangLogoClass)
    {
        if (QElangLogoClass->objectName().isEmpty())
            QElangLogoClass->setObjectName("QElangLogoClass");
        QElangLogoClass->resize(1108, 659);
        gridLayout = new QGridLayout(QElangLogoClass);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName("gridLayout");
        gridLayout->setContentsMargins(2, 2, 2, 2);
        openGLWidget = new el::QElangViewSignaled(QElangLogoClass);
        openGLWidget->setObjectName("openGLWidget");

        gridLayout->addWidget(openGLWidget, 1, 0, 1, 1);


        retranslateUi(QElangLogoClass);

        QMetaObject::connectSlotsByName(QElangLogoClass);
    } // setupUi

    void retranslateUi(QWidget *QElangLogoClass)
    {
        QElangLogoClass->setWindowTitle(QCoreApplication::translate("QElangLogoClass", "QElangLogo", nullptr));
    } // retranslateUi

};

namespace Ui {
    class QElangLogoClass: public Ui_QElangLogoClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_Q_ELANG_LOGO_H
