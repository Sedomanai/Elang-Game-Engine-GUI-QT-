/********************************************************************************
** Form generated from reading UI file 'q_ghost_dialog.ui'
**
** Created by: Qt User Interface Compiler version 6.4.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_Q_GHOST_DIALOG_H
#define UI_Q_GHOST_DIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_GhostDialogUI
{
public:
    QHBoxLayout *horizontalLayout_3;
    QVBoxLayout *verticalLayout_5;
    QGroupBox *typeBox;
    QVBoxLayout *verticalLayout_3;
    QGridLayout *gridLayout;
    QRadioButton *noneRadio;
    QRadioButton *prevRadio;
    QRadioButton *indexRadio;
    QRadioButton *externRadio;
    QGroupBox *posBox;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout;
    QRadioButton *backRadio;
    QRadioButton *frontRadio;
    QGroupBox *indexBox;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *texLayout;
    QPushButton *texButton;
    QLabel *texLabel;
    QHBoxLayout *atlasLayout;
    QPushButton *atlasButton;
    QLabel *atlasLabel;
    QHBoxLayout *cellLayout;
    QPushButton *cellButton;
    QLabel *cellLabel;
    QButtonGroup *posGroup;
    QButtonGroup *typeGroup;

    void setupUi(QWidget *GhostDialogUI)
    {
        if (GhostDialogUI->objectName().isEmpty())
            GhostDialogUI->setObjectName("GhostDialogUI");
        GhostDialogUI->resize(197, 231);
        horizontalLayout_3 = new QHBoxLayout(GhostDialogUI);
        horizontalLayout_3->setSpacing(0);
        horizontalLayout_3->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        horizontalLayout_3->setContentsMargins(0, 0, 0, 0);
        verticalLayout_5 = new QVBoxLayout();
        verticalLayout_5->setSpacing(4);
        verticalLayout_5->setObjectName("verticalLayout_5");
        verticalLayout_5->setContentsMargins(-1, 4, -1, -1);
        typeBox = new QGroupBox(GhostDialogUI);
        typeBox->setObjectName("typeBox");
        typeBox->setMaximumSize(QSize(16777215, 120));
        verticalLayout_3 = new QVBoxLayout(typeBox);
        verticalLayout_3->setSpacing(2);
        verticalLayout_3->setContentsMargins(11, 11, 11, 11);
        verticalLayout_3->setObjectName("verticalLayout_3");
        verticalLayout_3->setContentsMargins(4, 2, 4, 2);
        gridLayout = new QGridLayout();
        gridLayout->setSpacing(6);
        gridLayout->setObjectName("gridLayout");
        gridLayout->setHorizontalSpacing(6);
        gridLayout->setVerticalSpacing(0);
        gridLayout->setContentsMargins(4, 0, 4, -1);
        noneRadio = new QRadioButton(typeBox);
        typeGroup = new QButtonGroup(GhostDialogUI);
        typeGroup->setObjectName("typeGroup");
        typeGroup->addButton(noneRadio);
        noneRadio->setObjectName("noneRadio");
        noneRadio->setEnabled(true);
        noneRadio->setChecked(true);

        gridLayout->addWidget(noneRadio, 0, 0, 1, 1);

        prevRadio = new QRadioButton(typeBox);
        typeGroup->addButton(prevRadio);
        prevRadio->setObjectName("prevRadio");

        gridLayout->addWidget(prevRadio, 0, 1, 1, 1);

        indexRadio = new QRadioButton(typeBox);
        typeGroup->addButton(indexRadio);
        indexRadio->setObjectName("indexRadio");

        gridLayout->addWidget(indexRadio, 1, 0, 1, 1);

        externRadio = new QRadioButton(typeBox);
        typeGroup->addButton(externRadio);
        externRadio->setObjectName("externRadio");

        gridLayout->addWidget(externRadio, 1, 1, 1, 1);


        verticalLayout_3->addLayout(gridLayout);


        verticalLayout_5->addWidget(typeBox);

        posBox = new QGroupBox(GhostDialogUI);
        posBox->setObjectName("posBox");
        posBox->setMaximumSize(QSize(16777215, 50));
        verticalLayout_2 = new QVBoxLayout(posBox);
        verticalLayout_2->setSpacing(2);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName("verticalLayout_2");
        verticalLayout_2->setContentsMargins(4, 2, 4, 2);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(4, -1, 4, -1);
        backRadio = new QRadioButton(posBox);
        posGroup = new QButtonGroup(GhostDialogUI);
        posGroup->setObjectName("posGroup");
        posGroup->addButton(backRadio);
        backRadio->setObjectName("backRadio");
        backRadio->setChecked(true);

        horizontalLayout->addWidget(backRadio);

        frontRadio = new QRadioButton(posBox);
        posGroup->addButton(frontRadio);
        frontRadio->setObjectName("frontRadio");

        horizontalLayout->addWidget(frontRadio);


        verticalLayout_2->addLayout(horizontalLayout);


        verticalLayout_5->addWidget(posBox);

        indexBox = new QGroupBox(GhostDialogUI);
        indexBox->setObjectName("indexBox");
        verticalLayout = new QVBoxLayout(indexBox);
        verticalLayout->setSpacing(2);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(4, 2, 4, 8);
        texLayout = new QHBoxLayout();
        texLayout->setSpacing(4);
        texLayout->setObjectName("texLayout");
        texLayout->setContentsMargins(4, -1, 4, -1);
        texButton = new QPushButton(indexBox);
        texButton->setObjectName("texButton");
        texButton->setMaximumSize(QSize(52, 16777215));

        texLayout->addWidget(texButton);

        texLabel = new QLabel(indexBox);
        texLabel->setObjectName("texLabel");

        texLayout->addWidget(texLabel);


        verticalLayout->addLayout(texLayout);

        atlasLayout = new QHBoxLayout();
        atlasLayout->setSpacing(4);
        atlasLayout->setObjectName("atlasLayout");
        atlasLayout->setContentsMargins(4, -1, 4, -1);
        atlasButton = new QPushButton(indexBox);
        atlasButton->setObjectName("atlasButton");
        atlasButton->setMaximumSize(QSize(52, 16777215));

        atlasLayout->addWidget(atlasButton);

        atlasLabel = new QLabel(indexBox);
        atlasLabel->setObjectName("atlasLabel");

        atlasLayout->addWidget(atlasLabel);


        verticalLayout->addLayout(atlasLayout);

        cellLayout = new QHBoxLayout();
        cellLayout->setSpacing(4);
        cellLayout->setObjectName("cellLayout");
        cellLayout->setContentsMargins(4, -1, 4, -1);
        cellButton = new QPushButton(indexBox);
        cellButton->setObjectName("cellButton");
        cellButton->setMaximumSize(QSize(52, 16777215));

        cellLayout->addWidget(cellButton);

        cellLabel = new QLabel(indexBox);
        cellLabel->setObjectName("cellLabel");

        cellLayout->addWidget(cellLabel);


        verticalLayout->addLayout(cellLayout);


        verticalLayout_5->addWidget(indexBox);


        horizontalLayout_3->addLayout(verticalLayout_5);


        retranslateUi(GhostDialogUI);

        QMetaObject::connectSlotsByName(GhostDialogUI);
    } // setupUi

    void retranslateUi(QWidget *GhostDialogUI)
    {
        GhostDialogUI->setWindowTitle(QCoreApplication::translate("GhostDialogUI", "Set Ghost Dialog", nullptr));
        typeBox->setTitle(QCoreApplication::translate("GhostDialogUI", "Type", nullptr));
        noneRadio->setText(QCoreApplication::translate("GhostDialogUI", "None", nullptr));
        prevRadio->setText(QCoreApplication::translate("GhostDialogUI", "Previous", nullptr));
        indexRadio->setText(QCoreApplication::translate("GhostDialogUI", "Indexed", nullptr));
        externRadio->setText(QCoreApplication::translate("GhostDialogUI", "External", nullptr));
        posBox->setTitle(QCoreApplication::translate("GhostDialogUI", "Position", nullptr));
        backRadio->setText(QCoreApplication::translate("GhostDialogUI", "Back", nullptr));
        frontRadio->setText(QCoreApplication::translate("GhostDialogUI", "Front", nullptr));
        indexBox->setTitle(QCoreApplication::translate("GhostDialogUI", "Index", nullptr));
        texButton->setText(QCoreApplication::translate("GhostDialogUI", "Tex", nullptr));
        texLabel->setText(QString());
        atlasButton->setText(QCoreApplication::translate("GhostDialogUI", "Atlas", nullptr));
        atlasLabel->setText(QString());
        cellButton->setText(QCoreApplication::translate("GhostDialogUI", "Cell", nullptr));
        cellLabel->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class GhostDialogUI: public Ui_GhostDialogUI {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_Q_GHOST_DIALOG_H
