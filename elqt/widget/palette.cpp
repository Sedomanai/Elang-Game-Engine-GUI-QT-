#include "palette.h"

#include <qopenglcontext.h>

namespace el {
	
	QElangPaletteWidget::QElangPaletteWidget(QWidget* parent, bool internalLoop)
		: QElangTextureWidget(parent, internalLoop), mHighlightBatched(false), mCellShapes(0)
	{
		ui.view->setMouseTracking(true);

		ui.view->sig_Start.connect([&]() {
			safeCreatePalette();
		});

		ui.view->sig_Paint.connect([&]() {
			if (gGUI.open()) {
				mCellShapes->draw();
				mHighlighter->draw();
				mHighlightBatched = false;
			}
		});

		ui.view->sig_MousePress.connect([&]() {
			updateAllHolderCheck();
			});
		ui.view->sig_MouseRelease.connect([&]() {
			updateAllHolderCheck();
			});
		ui.view->sig_MouseMove.connect([&]() {
			updateAllHolderCheck();
			});
	}

	
	void QElangPaletteWidget::onTextureUpdate() {
		safeCreatePalette();
		recreateCellHoldersFromAtlas();
		redrawAllCellHolders();
	}

	
	void QElangPaletteWidget::redrawAllCellHolders() {
		assert(gGUI.open());
		assert(mTexture);
		forceUnlockDebuggers();
		resetMainCamera();
		for (obj<CellHolder> holder : gStage->view<CellHolder>()) {
			mCellShapes->line.batchAABB(holder->rect, color8(0, 255, 55, 255));
		}
		mCellShapes->line.flags |= ePainterFlags::LOCKED;
	}

	
	void QElangPaletteWidget::updateAllHolderCheck() {
		auto pos = *mMainCam * gMouse.currentPosition();
		sizet i = 0;
		for (obj<CellHolder> holder : gStage->view<CellHolder>()) {
			assert(holder);
			bool hit = holder->rect.contains(pos);
			holder.get<Button>().update(holder, hit);
		}
	}

	
	void QElangPaletteWidget::safeCreatePalette() {
		if (!mCellShapes) {
			ui.view->makeCurrent();
			gStage->storage<asset<Cell>>().reserve(300);
			gStage->storage<Box>().reserve(300);
			gStage->storage<Button>().reserve(300);
			mCellShapes = new EditorShapeDebug;
			mCellShapes->init(mMainCam);

			mHighlighter = new EditorShapeDebug;
			mHighlighter->init(mMainCam);
		}
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

	
	void QElangPaletteWidget::recreateCellHoldersFromAtlas() {
		assert(gGUI.open());
		assert(mTexture);

		auto view = gStage->view<CellHolder>();
		gStage->destroy(view.begin(), view.end());

		if (mTexture->atlas) {
			auto&& cv = mTexture->atlas->packedAndCachedCells();
			for (auto i = 0; i < cv.size(); i++) {
				auto it = cv[i];
				auto& cell = *asset<Cell>(it);

				Box rect;
				rect.l = cell.uvLeft * mTexture->width();
				rect.r = cell.uvRight * mTexture->width();
				rect.b = -cell.uvDown * mTexture->height();
				rect.t = -cell.uvUp * mTexture->height();

				auto holder = gStage->make<CellHolder>(it, rect);
				holder.add<Button>(this);
			}
		}
	}

	
	void QElangPaletteWidget::onHover(Entity self, Entity context) {
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
