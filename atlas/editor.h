#pragma once

#include <QtWidgets/QMainWindow>
#include <qactiongroup.h>
#include <qfiledialog>
#include <qcombobox.h>
#include <qcloseevent>
#include <qhideevent>
#include <qlabel.h>
#include <apparatus/loader.h>

#include "../elqt/palette_widget.h"
#include "../elang_qt_builder.h"
#include "auto_gen.h"
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
		AtlasEditor(QWidget* parent = Q_NULLPTR, Project* gui = 0);

		void loop();
		void initializeGLWidgets();

	private:

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

		Project* mGui;
		QVBoxLayout* mViewLayout;
		QVBoxLayout* mListLayout;
		QAction* mSetGhost;
		QToolBar* mCellToolbar;
		//CellsWidget* mCellsWidget;

		QToolBar* mOriginToolbar;
		//OriginView* mOriginView;

		QToolBar* mClipsToolbar;
		//ClipsView* mClipsView;

		AtlasAutoGenerator* mAutoGen;

		ViewMode mViewMode;
		bool mAcceptClose;
		bool mOpen;
		QTimer* mTimer;
		QActionGroup* mViewActions;
		QString mLastSavedPath;

		Ui::AtlasEditorUI ui;
	};
};
