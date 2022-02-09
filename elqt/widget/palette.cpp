#include "palette.h"

#include <qopenglcontext.h>

namespace el {

	QElangPaletteWidget::QElangPaletteWidget(QWidget* parent)
		: QElangTextureWidget(parent), mHighlightBatched(false)
	{
		ui.view->setMouseTracking(true);

		ui.view->sig_Start.connect([&]() {
			bind(mStage);
			mStage.storage<asset<Cell>>().reserve(300);
			mStage.storage<Box>().reserve(300);
			mStage.storage<Button>().reserve(300);
			mCellShapes = new ShapeDebug;
			mCellShapes->init(mMainCam);

			mHighlighter = new ShapeDebug;
			mHighlighter->init(mMainCam);
		});

		ui.view->sig_Paint.connect([&]() {
			if (gGUI.open()) {
				bind(mStage);
				mCellShapes->draw();
				mHighlighter->draw();
				mHighlightBatched = false;
			}
		});
	}

	void QElangPaletteWidget::onTextureUpdate() {
		redrawAllCellHolders(true);
	}

	void QElangPaletteWidget::redrawAllCellHolders(bool recreateFromAtlas) {
		assert(gGUI.open());
		assert(mTexture);

		bind(mStage);
		if (recreateFromAtlas) {
			auto view = mStage.view<Button>();
			mStage.destroy(view.begin(), view.end());
		}

		forceUnlockDebuggers();
		resetMainCamera();

		if (!recreateFromAtlas) {
			for (obj<CellHolder> holder : mStage.view<CellHolder>()) {
				mCellShapes->line.batchAABB(holder->rect, color8(0, 255, 55, 255));
			}
		} else if (mTexture->atlas) {
			auto& cells = mTexture->atlas->cells;
			for (auto it : cells) {
				auto& cell = *asset<Cell>(it.second);
				Box rect;
				rect.l = cell.uvLeft * mTexture->width();
				rect.r = cell.uvRight * mTexture->width();
				rect.b = -cell.uvDown * mTexture->height();
				rect.t = -cell.uvUp * mTexture->height();

				assert(cells.contains(it.second));
				auto holder = mStage.make<CellHolder>(it.second, rect);
				holder.add<Button>(this);
				mCellShapes->line.batchAABB(holder->rect, color8(0, 255, 55, 255));
			}
		}
		mCellShapes->line.flags |= Painter::LOCKED;
	}

	void QElangPaletteWidget::forceUnlockDebuggers() {
		mCellShapes->line.forceUnlock();
		mHighlighter->line.forceUnlock();
		mHighlighter->fill.forceUnlock();
	}

	void QElangPaletteWidget::resetMainCamera() {
		mCellShapes->line.camera = mMainCam;
		mCellShapes->fill.camera = mMainCam;
		mHighlighter->line.camera = mMainCam;
		mHighlighter->fill.camera = mMainCam;
	}

	void QElangPaletteWidget::coloring(Box& box) {
		color8 color(30, 255, 220, 255);
		if (gMouse.state(0) >= eInput::ONCE) {
			color.r = 180;
			color.g = 255;
			color.b = 30;
		}
		mHighlighter->line.batchAABB(box, color);
		color.a = 80;
		mHighlighter->fill.batchAABB(box, color);
	}

	void QElangPaletteWidget::onHover(Entity self, Entity context) {
		bind(mStage);
		auto holder = obj<CellHolder>(self);
		if (holder) {
			if (!mHighlightBatched) {
				coloring(holder->rect);
				mHighlightBatched = true;
			}
			
			if (gMouse.state(0) == eInput::LIFT) {
				if (holder) {
					sig_Clicked.invoke(*holder);
				}
			}
		}

	}
}
