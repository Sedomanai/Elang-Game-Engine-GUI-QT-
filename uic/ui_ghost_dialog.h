/********************************************************************************
** Form generated from reading UI file 'ghost_dialog.ui'
**
** Created by: Qt User Interface Compiler version 6.2.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GHOST_DIALOG_H
#define UI_GHOST_DIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
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
    QRadioButton *customRadio;
    QRadioButton *externRadio;
    QGroupBox *posBox;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout;
    QRadioButton *backRadio;
    QRadioButton *frontRadio;
    QGroupBox *customBox;
    QVBoxLayout *verticalLayout_4;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *textureLayout;
    QLabel *texLabel;
    QComboBox *texture;
    QHBoxLayout *paletteLayout;
    QPushButton *cellButton;
    QLabel *cellLabel;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *confirmButton;
    QPushButton *cancelButton;
    QButtonGroup *posGroup;
    QButtonGroup *typeGroup;

    void setupUi(QWidget *GhostDialogUI)
    {
        if (GhostDialogUI->objectName().isEmpty())
            GhostDialogUI->setObjectName(QString::fromUtf8("GhostDialogUI"));
        GhostDialogUI->resize(267, 251);
        horizontalLayout_3 = new QHBoxLayout(GhostDialogUI);
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        verticalLayout_5 = new QVBoxLayout();
        verticalLayout_5->setSpacing(2);
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        typeBox = new QGroupBox(GhostDialogUI);
        typeBox->setObjectName(QString::fromUtf8("typeBox"));
        typeBox->setMaximumSize(QSize(16777215, 120));
        verticalLayout_3 = new QVBoxLayout(typeBox);
        verticalLayout_3->setSpacing(2);
        verticalLayout_3->setContentsMargins(11, 11, 11, 11);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        verticalLayout_3->setContentsMargins(4, 4, 4, 4);
        gridLayout = new QGridLayout();
        gridLayout->setSpacing(0);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        noneRadio = new QRadioButton(typeBox);
        typeGroup = new QButtonGroup(GhostDialogUI);
        typeGroup->setObjectName(QString::fromUtf8("typeGroup"));
        typeGroup->addButton(noneRadio);
        noneRadio->setObjectName(QString::fromUtf8("noneRadio"));
        noneRadio->setEnabled(true);
        noneRadio->setChecked(true);

        gridLayout->addWidget(noneRadio, 0, 0, 1, 1);

        prevRadio = new QRadioButton(typeBox);
        typeGroup->addButton(prevRadio);
        prevRadio->setObjectName(QString::fromUtf8("prevRadio"));

        gridLayout->addWidget(prevRadio, 0, 1, 1, 1);

        customRadio = new QRadioButton(typeBox);
        typeGroup->addButton(customRadio);
        customRadio->setObjectName(QString::fromUtf8("customRadio"));

        gridLayout->addWidget(customRadio, 1, 0, 1, 1);

        externRadio = new QRadioButton(typeBox);
        typeGroup->addButton(externRadio);
        externRadio->setObjectName(QString::fromUtf8("externRadio"));

        gridLayout->addWidget(externRadio, 1, 1, 1, 1);


        verticalLayout_3->addLayout(gridLayout);


        verticalLayout_5->addWidget(typeBox);

        posBox = new QGroupBox(GhostDialogUI);
        posBox->setObjectName(QString::fromUtf8("posBox"));
        posBox->setMaximumSize(QSize(16777215, 50));
        verticalLayout_2 = new QVBoxLayout(posBox);
        verticalLayout_2->setSpacing(2);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(4, 4, 4, 4);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        backRadio = new QRadioButton(posBox);
        posGroup = new QButtonGroup(GhostDialogUI);
        posGroup->setObjectName(QString::fromUtf8("posGroup"));
        posGroup->addButton(backRadio);
        backRadio->setObjectName(QString::fromUtf8("backRadio"));
        backRadio->setChecked(true);

        horizontalLayout->addWidget(backRadio);

        frontRadio = new QRadioButton(posBox);
        posGroup->addButton(frontRadio);
        frontRadio->setObjectName(QString::fromUtf8("frontRadio"));

        horizontalLayout->addWidget(frontRadio);


        verticalLayout_2->addLayout(horizontalLayout);


        verticalLayout_5->addWidget(posBox);

        customBox = new QGroupBox(GhostDialogUI);
        customBox->setObjectName(QString::fromUtf8("customBox"));
        verticalLayout_4 = new QVBoxLayout(customBox);
        verticalLayout_4->setSpacing(6);
        verticalLayout_4->setContentsMargins(11, 11, 11, 11);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        verticalLayout_4->setContentsMargins(4, 4, 4, 4);
        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(6);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        textureLayout = new QHBoxLayout();
        textureLayout->setSpacing(6);
        textureLayout->setObjectName(QString::fromUtf8("textureLayout"));
        textureLayout->setContentsMargins(-1, 0, -1, -1);
        texLabel = new QLabel(customBox);
        texLabel->setObjectName(QString::fromUtf8("texLabel"));
        texLabel->setMaximumSize(QSize(50, 16777215));

        textureLayout->addWidget(texLabel);

        texture = new QComboBox(customBox);
        texture->setObjectName(QString::fromUtf8("texture"));

        textureLayout->addWidget(texture);


        verticalLayout->addLayout(textureLayout);

        paletteLayout = new QHBoxLayout();
        paletteLayout->setSpacing(6);
        paletteLayout->setObjectName(QString::fromUtf8("paletteLayout"));
        cellButton = new QPushButton(customBox);
        cellButton->setObjectName(QString::fromUtf8("cellButton"));
        cellButton->setMaximumSize(QSize(52, 16777215));

        paletteLayout->addWidget(cellButton);

        cellLabel = new QLabel(customBox);
        cellLabel->setObjectName(QString::fromUtf8("cellLabel"));

        paletteLayout->addWidget(cellLabel);


        verticalLayout->addLayout(paletteLayout);


        verticalLayout_4->addLayout(verticalLayout);


        verticalLayout_5->addWidget(customBox);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        confirmButton = new QPushButton(GhostDialogUI);
        confirmButton->setObjectName(QString::fromUtf8("confirmButton"));

        horizontalLayout_2->addWidget(confirmButton);

        cancelButton = new QPushButton(GhostDialogUI);
        cancelButton->setObjectName(QString::fromUtf8("cancelButton"));

        horizontalLayout_2->addWidget(cancelButton);


        verticalLayout_5->addLayout(horizontalLayout_2);


        horizontalLayout_3->addLayout(verticalLayout_5);


        retranslateUi(GhostDialogUI);

        QMetaObject::connectSlotsByName(GhostDialogUI);
    } // setupUi

    void retranslateUi(QWidget *GhostDialogUI)
    {
        GhostDialogUI->setWindowTitle(QCoreApplication::translate("GhostDialogUI", "GhostDialog", nullptr));
        typeBox->setTitle(QCoreApplication::translate("GhostDialogUI", "Type", nullptr));
        noneRadio->setText(QCoreApplication::translate("GhostDialogUI", "None", nullptr));
        prevRadio->setText(QCoreApplication::translate("GhostDialogUI", "Previous", nullptr));
        customRadio->setText(QCoreApplication::translate("GhostDialogUI", "Custom", nullptr));
        externRadio->setText(QCoreApplication::translate("GhostDialogUI", "External", nullptr));
        posBox->setTitle(QCoreApplication::translate("GhostDialogUI", "Position", nullptr));
        backRadio->setText(QCoreApplication::translate("GhostDialogUI", "Back", nullptr));
        frontRadio->setText(QCoreApplication::translate("GhostDialogUI", "Front", nullptr));
        customBox->setTitle(QCoreApplication::translate("GhostDialogUI", "Custom", nullptr));
        texLabel->setText(QCoreApplication::translate("GhostDialogUI", " Texture:", nullptr));
        cellButton->setText(QCoreApplication::translate("GhostDialogUI", "Cell", nullptr));
        cellLabel->setText(QString());
        confirmButton->setText(QCoreApplication::translate("GhostDialogUI", "Confirm", nullptr));
        cancelButton->setText(QCoreApplication::translate("GhostDialogUI", "Cancel", nullptr));
    } // retranslateUi

};

namespace Ui {
    class GhostDialogUI: public Ui_GhostDialogUI {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GHOST_DIALOG_H
