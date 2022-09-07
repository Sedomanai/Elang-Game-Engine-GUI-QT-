/*****************************************************************//**
 * @file   palette.h
 * @brief  An Elang Texture Widget that displays an atlas along with a texture
 *		   It's called palette because it's used to select a cell in an atlas 
 *		   Like choosing a paint from a palette
 *		   While this is widget functional all on its own with a cell-selected signal,
 *		   Its updateAtlas method can be overriden to implement other custom behavior
 * 
 * @author Sedomanai
 * @date   September 2022
 *********************************************************************/

#pragma once
#include "texture_widget.h"
#include <elements/button.h>
#include <tools/cell.h>

namespace el {
	struct Atlas;
	struct ShapeDebug2d;
	struct PaletteSelectedCell {};
	struct CellHolder
	{
		CellHolder(const Box& rect_, IButtonEvent* buttonEvent) : rect(rect_), button(buttonEvent) {};

		void moldCellFromRect(asset<Cell> self, int aw, int ah) {
			auto& meta = self.get<CellMeta>();
			meta = {
				(sizet)round(rect.l), (sizet)round(-rect.t),
				(sizet)round(rect.r - rect.l), (sizet)round(rect.t - rect.b),
				(sizet)meta.oX, (sizet)meta.oY
			};
			self->mold(meta, aw, ah);
		}

		Box rect, hitbox;
		Button button;
	};

	class QElangPaletteWidget : public QElangTextureWidget, public IButtonEvent
	{
	public:
		virtual void cleanAtlas();

		QElangPaletteWidget(QWidget* parent = Q_NULLPTR, bool internalLoop = false);
		virtual ~QElangPaletteWidget() override {
			release();
		}

		// Call this in a custom loop if the parameter internalLoop was false when constructed
		// Used for tweening, panning, etc.
		void loop() override;

		// Signal to connect to. Parameter should be the selected cell in entity form 
		// (with Cell, CellMeta, CellHolder, and AssetData attached)
		single_signal<asset<Cell>> sig_Clicked;
		void release() override {
			QElangTextureWidget::release();
			if (mCellShapes) {
				delete mCellShapes;
				delete mHighlighter;
				mCellShapes = 0;
			}
		}

		// Update atlas using an asset entity that holds the Atlas.
		// If you reimport or modify the atlas, you must call this method again
		virtual void updateAtlas(asset<Atlas>);

	protected:
		asset<Atlas> mAtlas;
		asset<CellHolder> mHovering, mHeld;
		bool mHighlightBatched;
		ShapeDebug2d* mCellShapes, *mHighlighter;

		void safeCreatePalette();
		void forceUnlockDebuggers();
		void resetMainCamera();
		void recreateCellHoldersFromAtlas();
		void rebatchAllCellHolders();
		void updateAllHolderCheck();
		void updateCursor();

		void onHover(Entity self, Entity context) override;
		void onEnter(Entity self, Entity context) override {};
		void onExit(Entity self, Entity context) override {};
		void postUpdate(Entity self, Entity context) override;
	};

	using AtlasPalette = QElangPaletteWidget;
	using CellResult = CellHolder;
} 
