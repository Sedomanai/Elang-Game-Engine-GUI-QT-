/********************************************************************************
** Form generated from reading UI file 'q_elang_atlas_editor.ui'
**
** Created by: Qt User Interface Compiler version 6.3.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_Q_ELANG_ATLAS_EDITOR_H
#define UI_Q_ELANG_ATLAS_EDITOR_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
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
    QAction *actionNewAtlas;
    QAction *actionOpenTexture;
    QAction *actionOpenAtlas;
    QAction *actionSaveAtlas;
    QAction *actionCellsView;
    QAction *actionPivotView;
    QAction *actionClipsView;
    QAction *actionSaveAtlasAs;
    QWidget *centralwidget;
    QHBoxLayout *horizontalLayout;
    QFrame *frame;
    QMenuBar *menubar;
    QMenu *menuFile;
    QMenu *menuNew;
    QMenu *menuOpen;
    QMenu *menuEdit;
    QMenu *menuView;
    QStatusBar *statusbar;
    QToolBar *toolBar;

    void setupUi(QMainWindow *AtlasEditorUI)
    {
        if (AtlasEditorUI->objectName().isEmpty())
            AtlasEditorUI->setObjectName(QString::fromUtf8("AtlasEditorUI"));
        AtlasEditorUI->resize(800, 600);
        actionNewAtlas = new QAction(AtlasEditorUI);
        actionNewAtlas->setObjectName(QString::fromUtf8("actionNewAtlas"));
        actionOpenTexture = new QAction(AtlasEditorUI);
        actionOpenTexture->setObjectName(QString::fromUtf8("actionOpenTexture"));
        actionOpenAtlas = new QAction(AtlasEditorUI);
        actionOpenAtlas->setObjectName(QString::fromUtf8("actionOpenAtlas"));
        actionSaveAtlas = new QAction(AtlasEditorUI);
        actionSaveAtlas->setObjectName(QString::fromUtf8("actionSaveAtlas"));
        actionCellsView = new QAction(AtlasEditorUI);
        actionCellsView->setObjectName(QString::fromUtf8("actionCellsView"));
        actionCellsView->setCheckable(true);
        actionCellsView->setChecked(true);
        actionPivotView = new QAction(AtlasEditorUI);
        actionPivotView->setObjectName(QString::fromUtf8("actionPivotView"));
        actionPivotView->setCheckable(true);
        actionClipsView = new QAction(AtlasEditorUI);
        actionClipsView->setObjectName(QString::fromUtf8("actionClipsView"));
        actionClipsView->setCheckable(true);
        actionSaveAtlasAs = new QAction(AtlasEditorUI);
        actionSaveAtlasAs->setObjectName(QString::fromUtf8("actionSaveAtlasAs"));
        centralwidget = new QWidget(AtlasEditorUI);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        horizontalLayout = new QHBoxLayout(centralwidget);
        horizontalLayout->setSpacing(2);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(2, 2, 2, 2);
        frame = new QFrame(centralwidget);
        frame->setObjectName(QString::fromUtf8("frame"));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);

        horizontalLayout->addWidget(frame);

        AtlasEditorUI->setCentralWidget(centralwidget);
        menubar = new QMenuBar(AtlasEditorUI);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 800, 22));
        menuFile = new QMenu(menubar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        menuNew = new QMenu(menuFile);
        menuNew->setObjectName(QString::fromUtf8("menuNew"));
        menuOpen = new QMenu(menuFile);
        menuOpen->setObjectName(QString::fromUtf8("menuOpen"));
        menuEdit = new QMenu(menubar);
        menuEdit->setObjectName(QString::fromUtf8("menuEdit"));
        menuView = new QMenu(menubar);
        menuView->setObjectName(QString::fromUtf8("menuView"));
        AtlasEditorUI->setMenuBar(menubar);
        statusbar = new QStatusBar(AtlasEditorUI);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        AtlasEditorUI->setStatusBar(statusbar);
        toolBar = new QToolBar(AtlasEditorUI);
        toolBar->setObjectName(QString::fromUtf8("toolBar"));
        AtlasEditorUI->addToolBar(Qt::TopToolBarArea, toolBar);

        menubar->addAction(menuFile->menuAction());
        menubar->addAction(menuEdit->menuAction());
        menubar->addAction(menuView->menuAction());
        menuFile->addAction(menuNew->menuAction());
        menuFile->addAction(menuOpen->menuAction());
        menuFile->addSeparator();
        menuFile->addAction(actionSaveAtlas);
        menuFile->addAction(actionSaveAtlasAs);
        menuNew->addAction(actionNewAtlas);
        menuOpen->addAction(actionOpenTexture);
        menuOpen->addAction(actionOpenAtlas);
        menuView->addAction(actionCellsView);
        menuView->addAction(actionPivotView);
        menuView->addAction(actionClipsView);
        toolBar->addAction(actionNewAtlas);
        toolBar->addSeparator();
        toolBar->addAction(actionOpenTexture);
        toolBar->addAction(actionOpenAtlas);
        toolBar->addSeparator();
        toolBar->addAction(actionSaveAtlas);
        toolBar->addSeparator();
        toolBar->addAction(actionCellsView);
        toolBar->addAction(actionPivotView);
        toolBar->addAction(actionClipsView);

        retranslateUi(AtlasEditorUI);

        QMetaObject::connectSlotsByName(AtlasEditorUI);
    } // setupUi

    void retranslateUi(QMainWindow *AtlasEditorUI)
    {
        AtlasEditorUI->setWindowTitle(QCoreApplication::translate("AtlasEditorUI", "Elang Atlas Editor", nullptr));
        actionNewAtlas->setText(QCoreApplication::translate("AtlasEditorUI", "Atlas...", nullptr));
        actionNewAtlas->setIconText(QCoreApplication::translate("AtlasEditorUI", "New Atlas", nullptr));
#if QT_CONFIG(tooltip)
        actionNewAtlas->setToolTip(QCoreApplication::translate("AtlasEditorUI", "New Atlas", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(shortcut)
        actionNewAtlas->setShortcut(QCoreApplication::translate("AtlasEditorUI", "Ctrl+N", nullptr));
#endif // QT_CONFIG(shortcut)
        actionOpenTexture->setText(QCoreApplication::translate("AtlasEditorUI", "Texture...", nullptr));
        actionOpenTexture->setIconText(QCoreApplication::translate("AtlasEditorUI", "Open Texture", nullptr));
#if QT_CONFIG(tooltip)
        actionOpenTexture->setToolTip(QCoreApplication::translate("AtlasEditorUI", "Open Texture", nullptr));
#endif // QT_CONFIG(tooltip)
        actionOpenAtlas->setText(QCoreApplication::translate("AtlasEditorUI", "Atlas...", nullptr));
        actionOpenAtlas->setIconText(QCoreApplication::translate("AtlasEditorUI", "Open Atlas", nullptr));
#if QT_CONFIG(tooltip)
        actionOpenAtlas->setToolTip(QCoreApplication::translate("AtlasEditorUI", "Open Atlas", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(shortcut)
        actionOpenAtlas->setShortcut(QCoreApplication::translate("AtlasEditorUI", "Ctrl+O", nullptr));
#endif // QT_CONFIG(shortcut)
        actionSaveAtlas->setText(QCoreApplication::translate("AtlasEditorUI", "Save Atlas", nullptr));
        actionSaveAtlas->setIconText(QCoreApplication::translate("AtlasEditorUI", "Save Atlas", nullptr));
#if QT_CONFIG(tooltip)
        actionSaveAtlas->setToolTip(QCoreApplication::translate("AtlasEditorUI", "Save Atlas", nullptr));
#endif // QT_CONFIG(tooltip)
#if QT_CONFIG(shortcut)
        actionSaveAtlas->setShortcut(QCoreApplication::translate("AtlasEditorUI", "Ctrl+S", nullptr));
#endif // QT_CONFIG(shortcut)
        actionCellsView->setText(QCoreApplication::translate("AtlasEditorUI", "Cells View", nullptr));
        actionPivotView->setText(QCoreApplication::translate("AtlasEditorUI", "Pivot View", nullptr));
#if QT_CONFIG(tooltip)
        actionPivotView->setToolTip(QCoreApplication::translate("AtlasEditorUI", "Pivot View", nullptr));
#endif // QT_CONFIG(tooltip)
        actionClipsView->setText(QCoreApplication::translate("AtlasEditorUI", "Clips View", nullptr));
        actionSaveAtlasAs->setText(QCoreApplication::translate("AtlasEditorUI", "Save Atlas As...", nullptr));
#if QT_CONFIG(shortcut)
        actionSaveAtlasAs->setShortcut(QCoreApplication::translate("AtlasEditorUI", "Ctrl+Shift+S", nullptr));
#endif // QT_CONFIG(shortcut)
        menuFile->setTitle(QCoreApplication::translate("AtlasEditorUI", "File", nullptr));
        menuNew->setTitle(QCoreApplication::translate("AtlasEditorUI", "New", nullptr));
        menuOpen->setTitle(QCoreApplication::translate("AtlasEditorUI", "Open", nullptr));
        menuEdit->setTitle(QCoreApplication::translate("AtlasEditorUI", "Edit", nullptr));
        menuView->setTitle(QCoreApplication::translate("AtlasEditorUI", "View", nullptr));
        toolBar->setWindowTitle(QCoreApplication::translate("AtlasEditorUI", "toolBar", nullptr));
    } // retranslateUi

};

namespace Ui {
    class AtlasEditorUI: public Ui_AtlasEditorUI {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_Q_ELANG_ATLAS_EDITOR_H
