/********************************************************************************
** Form generated from reading UI file 'editor.ui'
**
** Created by: Qt User Interface Compiler version 6.2.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_EDITOR_H
#define UI_EDITOR_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_AtlasEditorUI
{
public:
    QAction *actionCellMode;
    QAction *actionOriginMode;
    QAction *actionClipMode;
    QAction *actionNewProject;
    QAction *actionSaveProject;
    QAction *actionSaveProjectAs;
    QAction *actionLoadProject;
    QAction *actionExportAtlas;
    QAction *actionImportAtlas;
    QAction *actionReloadProject;
    QWidget *centralWidget;
    QGridLayout *gridLayout;
    QFrame *frame;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *AtlasEditorUI)
    {
        if (AtlasEditorUI->objectName().isEmpty())
            AtlasEditorUI->setObjectName(QString::fromUtf8("AtlasEditorUI"));
        AtlasEditorUI->resize(900, 600);
        AtlasEditorUI->setMinimumSize(QSize(900, 600));
        actionCellMode = new QAction(AtlasEditorUI);
        actionCellMode->setObjectName(QString::fromUtf8("actionCellMode"));
        actionCellMode->setCheckable(true);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/atlas/icons/atlas/cellmode.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionCellMode->setIcon(icon);
        actionOriginMode = new QAction(AtlasEditorUI);
        actionOriginMode->setObjectName(QString::fromUtf8("actionOriginMode"));
        actionOriginMode->setCheckable(true);
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/atlas/icons/atlas/originmode.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionOriginMode->setIcon(icon1);
        actionClipMode = new QAction(AtlasEditorUI);
        actionClipMode->setObjectName(QString::fromUtf8("actionClipMode"));
        actionClipMode->setCheckable(true);
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/atlas/icons/atlas/clipmode.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionClipMode->setIcon(icon2);
        actionNewProject = new QAction(AtlasEditorUI);
        actionNewProject->setObjectName(QString::fromUtf8("actionNewProject"));
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/essentials/icons/essentials/new.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionNewProject->setIcon(icon3);
        actionSaveProject = new QAction(AtlasEditorUI);
        actionSaveProject->setObjectName(QString::fromUtf8("actionSaveProject"));
        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/essentials/icons/essentials/save.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSaveProject->setIcon(icon4);
        actionSaveProjectAs = new QAction(AtlasEditorUI);
        actionSaveProjectAs->setObjectName(QString::fromUtf8("actionSaveProjectAs"));
        QIcon icon5;
        icon5.addFile(QString::fromUtf8(":/essentials/icons/essentials/save_as.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSaveProjectAs->setIcon(icon5);
        actionLoadProject = new QAction(AtlasEditorUI);
        actionLoadProject->setObjectName(QString::fromUtf8("actionLoadProject"));
        QIcon icon6;
        icon6.addFile(QString::fromUtf8(":/essentials/icons/essentials/load.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionLoadProject->setIcon(icon6);
        actionExportAtlas = new QAction(AtlasEditorUI);
        actionExportAtlas->setObjectName(QString::fromUtf8("actionExportAtlas"));
        QIcon icon7;
        icon7.addFile(QString::fromUtf8(":/essentials/icons/essentials/export.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionExportAtlas->setIcon(icon7);
        actionImportAtlas = new QAction(AtlasEditorUI);
        actionImportAtlas->setObjectName(QString::fromUtf8("actionImportAtlas"));
        QIcon icon8;
        icon8.addFile(QString::fromUtf8(":/essentials/icons/essentials/import.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionImportAtlas->setIcon(icon8);
        actionReloadProject = new QAction(AtlasEditorUI);
        actionReloadProject->setObjectName(QString::fromUtf8("actionReloadProject"));
        QIcon icon9;
        icon9.addFile(QString::fromUtf8(":/essentials/icons/essentials/reload.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionReloadProject->setIcon(icon9);
        centralWidget = new QWidget(AtlasEditorUI);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        gridLayout = new QGridLayout(centralWidget);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setContentsMargins(6, 6, 6, 6);
        frame = new QFrame(centralWidget);
        frame->setObjectName(QString::fromUtf8("frame"));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);

        gridLayout->addWidget(frame, 0, 0, 1, 1);

        AtlasEditorUI->setCentralWidget(centralWidget);
        mainToolBar = new QToolBar(AtlasEditorUI);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        AtlasEditorUI->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(AtlasEditorUI);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        AtlasEditorUI->setStatusBar(statusBar);

        mainToolBar->addAction(actionNewProject);
        mainToolBar->addAction(actionSaveProject);
        mainToolBar->addAction(actionSaveProjectAs);
        mainToolBar->addAction(actionLoadProject);
        mainToolBar->addAction(actionReloadProject);
        mainToolBar->addSeparator();
        mainToolBar->addAction(actionCellMode);
        mainToolBar->addAction(actionOriginMode);
        mainToolBar->addAction(actionClipMode);
        mainToolBar->addSeparator();
        mainToolBar->addAction(actionImportAtlas);
        mainToolBar->addAction(actionExportAtlas);

        retranslateUi(AtlasEditorUI);

        QMetaObject::connectSlotsByName(AtlasEditorUI);
    } // setupUi

    void retranslateUi(QMainWindow *AtlasEditorUI)
    {
        AtlasEditorUI->setWindowTitle(QCoreApplication::translate("AtlasEditorUI", "Atlas Editor", nullptr));
        actionCellMode->setText(QCoreApplication::translate("AtlasEditorUI", "Cell Mode", nullptr));
#if QT_CONFIG(tooltip)
        actionCellMode->setToolTip(QCoreApplication::translate("AtlasEditorUI", "Cell Mode", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(shortcut)
        actionCellMode->setShortcut(QCoreApplication::translate("AtlasEditorUI", "1", nullptr));
#endif // QT_CONFIG(shortcut)
        actionOriginMode->setText(QCoreApplication::translate("AtlasEditorUI", "Origin Mode", nullptr));
#if QT_CONFIG(tooltip)
        actionOriginMode->setToolTip(QCoreApplication::translate("AtlasEditorUI", "Origin Mode", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(shortcut)
        actionOriginMode->setShortcut(QCoreApplication::translate("AtlasEditorUI", "2", nullptr));
#endif // QT_CONFIG(shortcut)
        actionClipMode->setText(QCoreApplication::translate("AtlasEditorUI", "Clip Mode", nullptr));
#if QT_CONFIG(shortcut)
        actionClipMode->setShortcut(QCoreApplication::translate("AtlasEditorUI", "3", nullptr));
#endif // QT_CONFIG(shortcut)
        actionNewProject->setText(QCoreApplication::translate("AtlasEditorUI", "New Project", nullptr));
#if QT_CONFIG(shortcut)
        actionNewProject->setShortcut(QCoreApplication::translate("AtlasEditorUI", "Ctrl+N", nullptr));
#endif // QT_CONFIG(shortcut)
        actionSaveProject->setText(QCoreApplication::translate("AtlasEditorUI", "Save Project", nullptr));
#if QT_CONFIG(shortcut)
        actionSaveProject->setShortcut(QCoreApplication::translate("AtlasEditorUI", "Ctrl+S", nullptr));
#endif // QT_CONFIG(shortcut)
        actionSaveProjectAs->setText(QCoreApplication::translate("AtlasEditorUI", "Save Project As...", nullptr));
        actionLoadProject->setText(QCoreApplication::translate("AtlasEditorUI", "Load Project", nullptr));
#if QT_CONFIG(shortcut)
        actionLoadProject->setShortcut(QCoreApplication::translate("AtlasEditorUI", "Ctrl+O", nullptr));
#endif // QT_CONFIG(shortcut)
        actionExportAtlas->setText(QCoreApplication::translate("AtlasEditorUI", "Export Atlas", nullptr));
        actionImportAtlas->setText(QCoreApplication::translate("AtlasEditorUI", "Import Atlas", nullptr));
        actionReloadProject->setText(QCoreApplication::translate("AtlasEditorUI", "Reload Project", nullptr));
#if QT_CONFIG(tooltip)
        actionReloadProject->setToolTip(QCoreApplication::translate("AtlasEditorUI", "Reload Project", nullptr));
#endif // QT_CONFIG(tooltip)
    } // retranslateUi

};

namespace Ui {
    class AtlasEditorUI: public Ui_AtlasEditorUI {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_EDITOR_H
