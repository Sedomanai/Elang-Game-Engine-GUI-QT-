#pragma once

#include <QtWidgets/QMainWindow>
#include <qactiongroup.h>
#include <qfiledialog>
#include <qcombobox.h>
#include <qcloseevent>
#include <qhideevent>
#include <qlabel.h>
#include <qtimer.h>

#include <apparatus/loader.h>

#include "../elqt/widget/palette.h"
#include "../elang_qt_builder.h"
#include "../elang_qt_globals.h"
#include "cells_widget.h"
#include "origin_widget.h"
#include "clips_widget.h"
#include "auto_gen.h"
#include "ghost_dialog.h" // TODO: I'm thinking of bringing this to the ribbon bar
#include "util.h"


#include <uic/ui_editor.h>

namespace el
{
	class _ELANGQT_EXPORT AtlasEditor : public QMainWindow
	{
		Q_OBJECT

		enum class ViewMode
		{
			CELL,
			ORIGIN,
			CLIP
		};

	public:
		AtlasEditor(QWidget* parent = Q_NULLPTR);

		void loop();

	private:
		bool mSuppressSignal;
		bool mInitialized;

		void init(bool debug = false);

		//Set up all actions and their 
		void setupActions();
		//Set up layout for Viewport and List Widget
		void setupLayout();
		//Set up layout for Clip and Cell List
		void setupList();
		//Set up settings for Cell Mode
		void setupCellMode();
		//Set up settings for Origin Mode
		void setupOriginMode();
		//Set up settings for Clip Mode
		void setupClipMode();;
		//Set up initial view upon opening Atlas Editor
		void setupInitView();

		void keyPressEvent(QKeyEvent*) override;
		void keyReleaseEvent(QKeyEvent*) override;

		Stage mStage;

		QVBoxLayout* mViewLayout;
		QVBoxLayout* mListLayout;

		QToolBar* mCellToolbar;
		CellsWidget *mCellsWidget;

		QToolBar* mOriginToolbar;
		OriginView* mOriginView;

		QToolBar* mClipsToolbar;
		ClipsWidget* mClipsWidget;
		QTimer* mClipsTimer;

		AtlasAutoGenerator* mAutoGen;
		ElangAtlasGhostDialog* mGhostDialog;
		QComboBox* mTextureBox;

		ViewMode mViewMode;
		bool mAcceptClose;
		bool mOpen;
		QTimer* mTimer;
		QActionGroup* mViewActions;
		QString mLastSavedPath;

		QAction* mDebugLoad;

		Ui::AtlasEditorUI ui;

	};
};
