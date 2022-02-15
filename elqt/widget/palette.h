#pragma once

#include <elements/button.h>

#include "texture_widget.h"

namespace el {

	
	struct CellHolder
	{
		CellHolder(asset<Cell> cell_, const Box& rect_) : cell(cell_), rect(rect_) {};

		void moldCellFromRect(int aw, int ah, int index) {
			*cell = Cell(
				round(rect.l), round(-rect.t), 
				round(rect.r - rect.l), round(rect.t - rect.b),
				(int)cell->oX, (int)cell->oY,
				aw, ah, index
			); 
		}

		asset<Cell> cell;
		Box rect;
	};

	class _ELANGQT_EXPORT QElangPaletteWidget : public QElangTextureWidget, public IButtonEvent
	{
	public:

		QElangPaletteWidget(QWidget* parent = Q_NULLPTR, bool internalLoop = false);
		virtual ~QElangPaletteWidget() override {
			release();
		}

		single_signal<CellHolder> sig_Clicked;
		void release() override {
			QElangTextureWidget::release();
			if (mCellShapes) {
				delete mCellShapes;
				delete mHighlighter;
				mCellShapes = 0;
			} 
			
		}
	protected:

		bool mHighlightBatched;
		EditorShapeDebug* mCellShapes, *mHighlighter;
		void safeCreatePalette();
		void forceUnlockDebuggers();
		void resetMainCamera();
		void coloring(Box& box);
		void recreateCellHoldersFromAtlas();
		void redrawAllCellHolders();
		void updateAllHolderCheck();

		void onTextureUpdate() override;
		void onHover(Entity self, Entity context) override;
		void onEnter(Entity self, Entity context) override {};
		void onExit(Entity self, Entity context) override {};
		void postUpdate(Entity self, Entity context) override {};
	};

	using AtlasPalette = QElangPaletteWidget;
	using CellResult = CellHolder;
} 
