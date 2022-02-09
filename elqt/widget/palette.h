#pragma once

#include <elements/button.h>

#include "texture_widget.h"

namespace el {

	struct CellHolder
	{
		CellHolder(asset<Cell> cell_, const Box& rect_) : cell(cell_), rect(rect_) {};

		void moldCellFromRect(int aw, int ah) {
			*cell = Cell(
				round(rect.l), round(-rect.t), 
				round(rect.r - rect.l), round(rect.t - rect.b),
				(int)cell->oX, (int)cell->oY,
				aw, ah
			); 
		}

		asset<Cell> cell;
		Box rect;
	};

	class _ELANGQT_EXPORT QElangPaletteWidget : public QElangTextureWidget, public IButtonEvent
	{
		Q_OBJECT

	public:
		QElangPaletteWidget(QWidget* parent = Q_NULLPTR);
		~QElangPaletteWidget() {
			delete mCellShapes;
			delete mHighlighter;
		}

		asset<Atlas> atlas;
		single_signal<CellHolder> sig_Clicked;
	protected:
		bool mHighlightBatched;
		ShapeDebug* mCellShapes, * mHighlighter;
		void forceUnlockDebuggers();
		void resetMainCamera();
		void coloring(Box& box);
		void redrawAllCellHolders(bool recreateFromAtlas);

		void onTextureUpdate() override;
		void onHover(Entity self, Entity context) override;
		void onEnter(Entity self, Entity context) override {};
		void onExit(Entity self, Entity context) override {};
		void postUpdate(Entity self, Entity context) override {};
	};
} 
