#pragma once
#include "../elqt/widget/palette.h"
#include "util.h"

namespace el
{;
	struct CellsWidget : public QElangPaletteWidget
	{
		Q_OBJECT
	public:
		CellsWidget(QWidget* parent = Q_NULLPTR);

		void autoCreateCell();
		void autoNewGenAtlas(asset<Atlas> atlas, uint sortorder, uint margin);
		void reorderCellsAccordingToList();
		void combineCells();
		void showEditor();
		void hideEditor();
		void deleteSelected();

		void onMouseMove();
		void onKeyPress(QKeyEvent*);
		void onKeyRelease(QKeyEvent*);

		void updateAtlas(asset<Atlas>) override;
		void loop() override;

		signal<> sig_Modified;
	private:
		enum CursorState
		{
			eNone = 0,
			eMove = 1,
			eLeft = 2,
			eBottom = 4,
			eRight = 8,
			eTop = 16,
		};

		enum SelectState
		{
			eSNone,
			eSelecting,
			eCreating,
			eMoving,
			eSizing,
		} mState;

		void renameAll();
		void sortAtlasOnNewGen(uint sortorder, uint margin);
		void deleteCell(asset<CellHolder> cell);
		asset<CellHolder> createCell(const string& name);
		void createNamedCell();
		color8 selectColoring();
		void onHover(Entity self, Entity context) override {};

		QCursor mTempCursor;
		uint mCursorState;

		uint mAlphaCut;
		bool mSuppressSelect;

		vec2 mCamPivot, mGrabPos;
		Box mSelectRect;

		void safeClearSelection();
		void connectMouseInput();
		void connectList();
		void recreateList();
		void findCursorState();
	};
}
