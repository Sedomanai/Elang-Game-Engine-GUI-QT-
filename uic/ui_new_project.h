/********************************************************************************
** Form generated from reading UI file 'new_project.ui'
**
** Created by: Qt User Interface Compiler version 6.2.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_NEW_PROJECT_H
#define UI_NEW_PROJECT_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_NewProjectUI
{
public:
    QVBoxLayout *verticalLayout_2;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_3;
    QLabel *assetDir_3;
    QLineEdit *projectName;
    QHBoxLayout *horizontalLayout_2;
    QLabel *assetDir_2;
    QPushButton *proj;
    QLabel *projDir;
    QSpacerItem *horizontalSpacer_3;
    QFrame *line;
    QHBoxLayout *horizontalLayout_4;
    QLabel *assetDir_4;
    QPushButton *asset;
    QLabel *assetDir;
    QSpacerItem *horizontalSpacer_4;
    QHBoxLayout *horizontalLayout_5;
    QLabel *assetDir_5;
    QPushButton *src;
    QLabel *srcDir;
    QSpacerItem *horizontalSpacer_5;
    QHBoxLayout *horizontalLayout_8;
    QLabel *assetDir_6;
    QPushButton *dat;
    QLabel *datDir;
    QSpacerItem *horizontalSpacer_6;
    QHBoxLayout *horizontalLayout_7;
    QSpacerItem *horizontalSpacer_2;
    QCheckBox *internalBinary;
    QHBoxLayout *horizontalLayout_6;
    QSpacerItem *horizontalSpacer;
    QPushButton *ok;
    QPushButton *cancel;

    void setupUi(QWidget *NewProjectUI)
    {
        if (NewProjectUI->objectName().isEmpty())
            NewProjectUI->setObjectName(QString::fromUtf8("NewProjectUI"));
        NewProjectUI->setWindowModality(Qt::ApplicationModal);
        NewProjectUI->resize(420, 241);
        verticalLayout_2 = new QVBoxLayout(NewProjectUI);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        assetDir_3 = new QLabel(NewProjectUI);
        assetDir_3->setObjectName(QString::fromUtf8("assetDir_3"));
        assetDir_3->setMargin(3);

        horizontalLayout_3->addWidget(assetDir_3);

        projectName = new QLineEdit(NewProjectUI);
        projectName->setObjectName(QString::fromUtf8("projectName"));

        horizontalLayout_3->addWidget(projectName);


        verticalLayout->addLayout(horizontalLayout_3);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        assetDir_2 = new QLabel(NewProjectUI);
        assetDir_2->setObjectName(QString::fromUtf8("assetDir_2"));
        assetDir_2->setMargin(3);

        horizontalLayout_2->addWidget(assetDir_2);

        proj = new QPushButton(NewProjectUI);
        proj->setObjectName(QString::fromUtf8("proj"));
        proj->setMaximumSize(QSize(100, 16777215));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/essentials/icons/essentials/reload.png"), QSize(), QIcon::Normal, QIcon::Off);
        proj->setIcon(icon);
        proj->setFlat(false);

        horizontalLayout_2->addWidget(proj);

        projDir = new QLabel(NewProjectUI);
        projDir->setObjectName(QString::fromUtf8("projDir"));
        projDir->setMargin(3);

        horizontalLayout_2->addWidget(projDir);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_3);


        verticalLayout->addLayout(horizontalLayout_2);

        line = new QFrame(NewProjectUI);
        line->setObjectName(QString::fromUtf8("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        verticalLayout->addWidget(line);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        assetDir_4 = new QLabel(NewProjectUI);
        assetDir_4->setObjectName(QString::fromUtf8("assetDir_4"));
        assetDir_4->setMargin(3);

        horizontalLayout_4->addWidget(assetDir_4);

        asset = new QPushButton(NewProjectUI);
        asset->setObjectName(QString::fromUtf8("asset"));
        asset->setMaximumSize(QSize(100, 16777215));
        asset->setIcon(icon);
        asset->setFlat(false);

        horizontalLayout_4->addWidget(asset);

        assetDir = new QLabel(NewProjectUI);
        assetDir->setObjectName(QString::fromUtf8("assetDir"));
        assetDir->setMargin(3);

        horizontalLayout_4->addWidget(assetDir);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_4);


        verticalLayout->addLayout(horizontalLayout_4);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        assetDir_5 = new QLabel(NewProjectUI);
        assetDir_5->setObjectName(QString::fromUtf8("assetDir_5"));
        assetDir_5->setMargin(3);

        horizontalLayout_5->addWidget(assetDir_5);

        src = new QPushButton(NewProjectUI);
        src->setObjectName(QString::fromUtf8("src"));
        src->setMaximumSize(QSize(100, 16777215));
        src->setIcon(icon);
        src->setFlat(false);

        horizontalLayout_5->addWidget(src);

        srcDir = new QLabel(NewProjectUI);
        srcDir->setObjectName(QString::fromUtf8("srcDir"));
        srcDir->setMargin(3);

        horizontalLayout_5->addWidget(srcDir);

        horizontalSpacer_5 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer_5);


        verticalLayout->addLayout(horizontalLayout_5);

        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setObjectName(QString::fromUtf8("horizontalLayout_8"));
        assetDir_6 = new QLabel(NewProjectUI);
        assetDir_6->setObjectName(QString::fromUtf8("assetDir_6"));
        assetDir_6->setMargin(3);

        horizontalLayout_8->addWidget(assetDir_6);

        dat = new QPushButton(NewProjectUI);
        dat->setObjectName(QString::fromUtf8("dat"));
        dat->setMaximumSize(QSize(100, 16777215));
        dat->setIcon(icon);
        dat->setFlat(false);

        horizontalLayout_8->addWidget(dat);

        datDir = new QLabel(NewProjectUI);
        datDir->setObjectName(QString::fromUtf8("datDir"));
        datDir->setMargin(3);

        horizontalLayout_8->addWidget(datDir);

        horizontalSpacer_6 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_8->addItem(horizontalSpacer_6);


        verticalLayout->addLayout(horizontalLayout_8);

        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setObjectName(QString::fromUtf8("horizontalLayout_7"));
        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_7->addItem(horizontalSpacer_2);

        internalBinary = new QCheckBox(NewProjectUI);
        internalBinary->setObjectName(QString::fromUtf8("internalBinary"));

        horizontalLayout_7->addWidget(internalBinary);


        verticalLayout->addLayout(horizontalLayout_7);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_6->addItem(horizontalSpacer);

        ok = new QPushButton(NewProjectUI);
        ok->setObjectName(QString::fromUtf8("ok"));
        ok->setMaximumSize(QSize(100, 16777215));

        horizontalLayout_6->addWidget(ok);

        cancel = new QPushButton(NewProjectUI);
        cancel->setObjectName(QString::fromUtf8("cancel"));
        cancel->setMaximumSize(QSize(100, 16777215));

        horizontalLayout_6->addWidget(cancel);


        verticalLayout->addLayout(horizontalLayout_6);


        verticalLayout_2->addLayout(verticalLayout);


        retranslateUi(NewProjectUI);

        QMetaObject::connectSlotsByName(NewProjectUI);
    } // setupUi

    void retranslateUi(QWidget *NewProjectUI)
    {
        NewProjectUI->setWindowTitle(QCoreApplication::translate("NewProjectUI", "New Project", nullptr));
        assetDir_3->setText(QCoreApplication::translate("NewProjectUI", "Project Name     :", nullptr));
        assetDir_2->setText(QCoreApplication::translate("NewProjectUI", "Project Directory :", nullptr));
        proj->setText(QString());
        projDir->setText(QCoreApplication::translate("NewProjectUI", "label", nullptr));
        assetDir_4->setText(QCoreApplication::translate("NewProjectUI", "Asset Directory   :", nullptr));
        asset->setText(QString());
        assetDir->setText(QCoreApplication::translate("NewProjectUI", "label", nullptr));
        assetDir_5->setText(QCoreApplication::translate("NewProjectUI", "Source Directory :", nullptr));
        src->setText(QString());
        srcDir->setText(QCoreApplication::translate("NewProjectUI", "label", nullptr));
        assetDir_6->setText(QCoreApplication::translate("NewProjectUI", "Data Directory    :", nullptr));
        dat->setText(QString());
        datDir->setText(QCoreApplication::translate("NewProjectUI", "label", nullptr));
        internalBinary->setText(QCoreApplication::translate("NewProjectUI", "Internal Binary", nullptr));
        ok->setText(QCoreApplication::translate("NewProjectUI", "OK", nullptr));
#if QT_CONFIG(shortcut)
        ok->setShortcut(QCoreApplication::translate("NewProjectUI", "Return", nullptr));
#endif // QT_CONFIG(shortcut)
        cancel->setText(QCoreApplication::translate("NewProjectUI", "Cancel", nullptr));
#if QT_CONFIG(shortcut)
        cancel->setShortcut(QCoreApplication::translate("NewProjectUI", "Esc", nullptr));
#endif // QT_CONFIG(shortcut)
    } // retranslateUi

};

namespace Ui {
    class NewProjectUI: public Ui_NewProjectUI {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_NEW_PROJECT_H
