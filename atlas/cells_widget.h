#pragma once
#include "../elqt/widget/palette.h"
#include "util.h"
//
//#include <loaders/atlas.h>
//#include "atlas_util.h"
//#include <qt_texture_widget.h>

namespace el {
	struct CellsWidget : public QElangPaletteWidget
	{
		Q_OBJECT
	public:
		CellsWidget(QWidget* parent = Q_NULLPTR);

		void autoCreateCell();
		void autoGenCells(uint sortorder, uint margin);
		void combineCells();
		void showEditor();
		void hideEditor();

		//void importAtlasBegin();
		//void importAtlasEnd();
		//void autoCreateCell();
		void deleteSelected();


		void onKeyPress(QKeyEvent*);
		void onKeyRelease(QKeyEvent*);

	private:
		void deleteCell(obj<CellHolder> cell);
		obj<CellHolder> createCell(const string& name);
		void createNamedCell();
		color8 selectColoring();
		void onHover(Entity self, Entity context) override {};

		enum CursorState
		{
			NONE = 0,
			MOVE = 1,
			LEFT = 2,
			BOTTOM = 4,
			RIGHT = 8,
			TOP = 16,
		};

		enum SelectState
		{
			SNONE,
			SELECTING,
			CREATING,
			MOVING,
			SIZING,
		} mState;

		//vec2 mCellOrigin;
		QCursor mTempCursor;
		uint mCursorState;

		uint mAlphaCut;
		bool mAlt, mCtrl;
		bool mSuppressSelect;

		Observer mSelects;
		Box mSelectRect;

		void onTextureUpdate() override;
		void connectMouseInput();
		void connectList();
		void recreateList();
		void findCursorState();
		void deleteAll();
	};
}
