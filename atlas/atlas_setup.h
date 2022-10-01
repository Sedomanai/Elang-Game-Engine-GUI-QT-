#pragma once
// QT includes in precompiler

#include <uic/ui_q_elang_atlas_editor.h>
#include <atlas/util.h>

namespace el
{
	class CellsWidget;
	class PivotView;
	class ClipsWidget;
	class AtlasSetup : public QMainWindow
	{
		Q_OBJECT

	public:
		AtlasSetup(QWidget* parent = nullptr);

	protected:
		AtlasViewMode mViewMode;
		QActionGroup* mViewActions;
		QVBoxLayout* mViewLayout;
		QVBoxLayout* mListLayout;

		QToolBar* mCellToolbar;
		CellsWidget* mCellsWidget;

		QToolBar* mPivotToolbar1, * mPivotToolbar2;
		PivotView* mPivotView;

		QToolBar* mClipsToolbar;
		ClipsWidget* mClipsWidget;
		QTimer* mClipsTimer;

		Ui::AtlasEditorUI ui;
		void updateEditorTitle(asset<Atlas>);
		void loop();
	private:
		QString mAtlasFilename;
		void setModified();
		void setupActions();
		void setupLayout();
		void setupList();
		void setupInitView();
		void setupCellMode();
		void setupPivotMode();
		void setupClipMode();
	};
};
