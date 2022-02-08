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

		void autoGenCells(uint sortorder, uint margin);
		//void combineCells();
		void showEditor();
		//void hideEditor();

		//void importAtlasBegin();
		//void importAtlasEnd();
		//void autoCreateCell();
		//void deleteSelected();

	private:
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
			MOVING,
			SIZING,
		} mState;

		//vec2 mCellOrigin;
		QCursor mTempCursor;

		uint mAlphaCut;
		bool mAlt;
		//bool mListClicked;
		bool mSuppressSelect;
		bool mConnected;

		Box mSelectRect, mHighlightRect;
		vector<obj<CellHolder>> mSelects;

		uint mCursorState;

		void onTextureUpdate() override;
		void connectMouseInput();
		void connectList();
		void recreateList();
		void findCursorState(vec2 pos);
		void deleteAll();
		void highlightSelected();
	};
}
