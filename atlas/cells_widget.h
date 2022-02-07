#pragma once
#include "../elqt/widget/texture_widget.h"
//
//#include <loaders/atlas.h>
//#include "atlas_util.h"
//#include <qt_texture_widget.h>

namespace el {
	struct CellsWidget : public QElangTextureWidget
	{
		Q_OBJECT
	public:
		CellsWidget(QWidget* parent = Q_NULLPTR);
		~CellsWidget();

		//void autoGenCells(uint sortorder, uint margin);
		//void combineCells();
		//void showEditor();
		//void hideEditor();

		//void importAtlasBegin();
		//void importAtlasEnd();
		//void autoCreateCell();
		//void deleteSelected();

	private:
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

		aabb mSelectRect, mHighlight;
		uint mCursorState;

		void connectMouseInput();
		void connectList();
		void recreateList();
		void findCursorState(vec2 pos);
		void deleteAll();
		void highlightSelected();
	};
}
