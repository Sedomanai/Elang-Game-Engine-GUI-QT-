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
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
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
    QWidget *centralWidget;
    QGridLayout *gridLayout;
    QFrame *frame;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuView;
    QMenu *menuDebug;
    QMenu *menuSettings;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *AtlasEditorUI)
    {
        if (AtlasEditorUI->objectName().isEmpty())
            AtlasEditorUI->setObjectName(QString::fromUtf8("AtlasEditorUI"));
        AtlasEditorUI->resize(900, 601);
        AtlasEditorUI->setMinimumSize(QSize(900, 600));
        actionCellMode = new QAction(AtlasEditorUI);
        actionCellMode->setObjectName(QString::fromUtf8("actionCellMode"));
        actionCellMode->setCheckable(true);
        actionOriginMode = new QAction(AtlasEditorUI);
        actionOriginMode->setObjectName(QString::fromUtf8("actionOriginMode"));
        actionOriginMode->setCheckable(true);
        actionClipMode = new QAction(AtlasEditorUI);
        actionClipMode->setObjectName(QString::fromUtf8("actionClipMode"));
        actionClipMode->setCheckable(true);
        actionNewProject = new QAction(AtlasEditorUI);
        actionNewProject->setObjectName(QString::fromUtf8("actionNewProject"));
        actionSaveProject = new QAction(AtlasEditorUI);
        actionSaveProject->setObjectName(QString::fromUtf8("actionSaveProject"));
        actionSaveProjectAs = new QAction(AtlasEditorUI);
        actionSaveProjectAs->setObjectName(QString::fromUtf8("actionSaveProjectAs"));
        actionLoadProject = new QAction(AtlasEditorUI);
        actionLoadProject->setObjectName(QString::fromUtf8("actionLoadProject"));
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
        menuBar = new QMenuBar(AtlasEditorUI);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 900, 22));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        menuView = new QMenu(menuBar);
        menuView->setObjectName(QString::fromUtf8("menuView"));
        menuDebug = new QMenu(menuBar);
        menuDebug->setObjectName(QString::fromUtf8("menuDebug"));
        menuSettings = new QMenu(menuBar);
        menuSettings->setObjectName(QString::fromUtf8("menuSettings"));
        AtlasEditorUI->setMenuBar(menuBar);
        mainToolBar = new QToolBar(AtlasEditorUI);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        AtlasEditorUI->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(AtlasEditorUI);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        AtlasEditorUI->setStatusBar(statusBar);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuView->menuAction());
        menuBar->addAction(menuSettings->menuAction());
        menuBar->addAction(menuDebug->menuAction());
        menuFile->addAction(actionNewProject);
        menuFile->addAction(actionSaveProject);
        menuFile->addAction(actionSaveProjectAs);
        menuFile->addAction(actionLoadProject);
        menuFile->addSeparator();
        menuView->addAction(actionCellMode);
        mainToolBar->addAction(actionCellMode);
        mainToolBar->addAction(actionOriginMode);
        mainToolBar->addAction(actionClipMode);

        retranslateUi(AtlasEditorUI);

        QMetaObject::connectSlotsByName(AtlasEditorUI);
    } // setupUi

    void retranslateUi(QMainWindow *AtlasEditorUI)
    {
        AtlasEditorUI->setWindowTitle(QCoreApplication::translate("AtlasEditorUI", "Atlas Editor", nullptr));
        actionCellMode->setText(QCoreApplication::translate("AtlasEditorUI", "Cell Mode", nullptr));
#if QT_CONFIG(tooltip)
        actionCellMode->setToolTip(QCoreApplication::translate("AtlasEditorUI", "Transition to Cell Edit Mode", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(shortcut)
        actionCellMode->setShortcut(QCoreApplication::translate("AtlasEditorUI", "1", nullptr));
#endif // QT_CONFIG(shortcut)
        actionOriginMode->setText(QCoreApplication::translate("AtlasEditorUI", "Origin Mode", nullptr));
#if QT_CONFIG(tooltip)
        actionOriginMode->setToolTip(QCoreApplication::translate("AtlasEditorUI", "Transition to Origin Edit Mode", nullptr));
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
        menuFile->setTitle(QCoreApplication::translate("AtlasEditorUI", "File", nullptr));
        menuView->setTitle(QCoreApplication::translate("AtlasEditorUI", "Modes", nullptr));
        menuDebug->setTitle(QCoreApplication::translate("AtlasEditorUI", "Debug", nullptr));
        menuSettings->setTitle(QCoreApplication::translate("AtlasEditorUI", "Settings", nullptr));
    } // retranslateUi

};

namespace Ui {
    class AtlasEditorUI: public Ui_AtlasEditorUI {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_EDITOR_H
