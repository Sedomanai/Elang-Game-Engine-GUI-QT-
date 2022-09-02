/********************************************************************************
** Form generated from reading UI file 'clips_widget.ui'
**
** Created by: Qt User Interface Compiler version 6.3.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CLIPS_WIDGET_H
#define UI_CLIPS_WIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollBar>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <elqt/extension/view.h>

QT_BEGIN_NAMESPACE

class Ui_ClipsWidgetUI
{
public:
    QVBoxLayout *verticalLayout_2;
    QVBoxLayout *verticalLayout;
    el::QElangViewSignaled *view;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label;
    QDoubleSpinBox *speedBox;
    QCheckBox *repeatBox;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *playButton;
    QSpacerItem *horizontalSpacer;
    el::QElangViewSignaled *reel;
    QScrollBar *scroll;

    void setupUi(QWidget *ClipsWidgetUI)
    {
        if (ClipsWidgetUI->objectName().isEmpty())
            ClipsWidgetUI->setObjectName(QString::fromUtf8("ClipsWidgetUI"));
        ClipsWidgetUI->resize(870, 595);
        verticalLayout_2 = new QVBoxLayout(ClipsWidgetUI);
        verticalLayout_2->setSpacing(0);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(2);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        view = new el::QElangViewSignaled(ClipsWidgetUI);
        view->setObjectName(QString::fromUtf8("view"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(view->sizePolicy().hasHeightForWidth());
        view->setSizePolicy(sizePolicy);

        verticalLayout->addWidget(view);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        label = new QLabel(ClipsWidgetUI);
        label->setObjectName(QString::fromUtf8("label"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Minimum);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
        label->setSizePolicy(sizePolicy1);
        label->setMaximumSize(QSize(16777215, 20));

        horizontalLayout_2->addWidget(label);

        speedBox = new QDoubleSpinBox(ClipsWidgetUI);
        speedBox->setObjectName(QString::fromUtf8("speedBox"));
        QSizePolicy sizePolicy2(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(speedBox->sizePolicy().hasHeightForWidth());
        speedBox->setSizePolicy(sizePolicy2);

        horizontalLayout_2->addWidget(speedBox);

        repeatBox = new QCheckBox(ClipsWidgetUI);
        repeatBox->setObjectName(QString::fromUtf8("repeatBox"));
        QSizePolicy sizePolicy3(QSizePolicy::Minimum, QSizePolicy::Maximum);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(repeatBox->sizePolicy().hasHeightForWidth());
        repeatBox->setSizePolicy(sizePolicy3);
        repeatBox->setMaximumSize(QSize(16777215, 20));

        horizontalLayout_2->addWidget(repeatBox);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Minimum, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_2);

        playButton = new QPushButton(ClipsWidgetUI);
        playButton->setObjectName(QString::fromUtf8("playButton"));

        horizontalLayout_2->addWidget(playButton);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);


        verticalLayout->addLayout(horizontalLayout_2);

        reel = new el::QElangViewSignaled(ClipsWidgetUI);
        reel->setObjectName(QString::fromUtf8("reel"));
        sizePolicy.setHeightForWidth(reel->sizePolicy().hasHeightForWidth());
        reel->setSizePolicy(sizePolicy);
        reel->setMaximumSize(QSize(16777215, 100));

        verticalLayout->addWidget(reel);

        scroll = new QScrollBar(ClipsWidgetUI);
        scroll->setObjectName(QString::fromUtf8("scroll"));
        scroll->setOrientation(Qt::Horizontal);

        verticalLayout->addWidget(scroll);


        verticalLayout_2->addLayout(verticalLayout);


        retranslateUi(ClipsWidgetUI);

        QMetaObject::connectSlotsByName(ClipsWidgetUI);
    } // setupUi

    void retranslateUi(QWidget *ClipsWidgetUI)
    {
        ClipsWidgetUI->setWindowTitle(QCoreApplication::translate("ClipsWidgetUI", "Clilps Widget", nullptr));
        label->setText(QCoreApplication::translate("ClipsWidgetUI", "  Speed ", nullptr));
        repeatBox->setText(QCoreApplication::translate("ClipsWidgetUI", "Repeat", nullptr));
        playButton->setText(QCoreApplication::translate("ClipsWidgetUI", "Play", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ClipsWidgetUI: public Ui_ClipsWidgetUI {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CLIPS_WIDGET_H
