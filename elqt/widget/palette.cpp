#include "palette.h"

namespace el {

	QElangPaletteWidget::QElangPaletteWidget(QWidget* parent)
		: QElangTextureWidget(parent)
	{
		ui.view->setMouseTracking(true);

		ui.view->sig_Start.connect([&]() {
			bind(mStage);
			mStage.storage<asset<Cell>>().reserve(300);
			mStage.storage<Box>().reserve(300);
			mStage.storage<Button>().reserve(300);
			mCellShapes = new ShapeDebug;
			mHighlight = new ShapeDebug;
			mCellShapes->init(mMainCam);
			mHighlight->init(mMainCam);
		});

		ui.view->sig_Paint.connect([&]() {
			bind(mStage);
			mCellShapes->draw();
			mHighlight->draw();
		});
	}

	void QElangPaletteWidget::onTextureUpdate(asset<Texture> tex) {
		bind(mStage);
		for (auto e : gStage->view<Button>()) {
			gStage->destroy(e);
		}

		if (tex->atlas) {
			atlas = tex->atlas;
			auto& cells = atlas->cells;
			for (auto it : cells) {
				auto& cell = *asset<Cell>(it.second);
				Box rect;
				rect.l = cell.uvLeft * tex->width();
				rect.r = cell.uvRight * tex->width();
				rect.b = -cell.uvDown * tex->height();
				rect.t = -cell.uvUp * tex->height();

				auto obj = gStage->make<Box>(rect);
				obj.add<Button>(this);
				obj.add<asset<Cell>>(it.second);
				mCellShapes->line.batchAABB(*obj, color8(0, 255, 55, 255));
			}
			mCellShapes->line.flags |= Painter::LOCKED;
		}
	}

	void QElangPaletteWidget::refresh() {
		mCellShapes->line.forceUnlock();
		mHighlight->line.forceUnlock();
		mHighlight->fill.forceUnlock();
		QElangTextureWidget::refresh();
		mCellShapes->line.camera = mMainCam;
		mCellShapes->fill.camera = mMainCam;
		mHighlight->line.camera = mMainCam;
		mHighlight->fill.camera = mMainCam;
	}

	void QElangPaletteWidget::coloring(Box& box) {
		color8 color(30, 255, 220, 255);
		if (gMouse.state(0) >= eInput::ONCE) {
			color.r = 180;
			color.g = 255;
			color.b = 30;
		}
		mHighlight->line.batchAABB(box, color);
		color.a = 80;
		mHighlight->fill.batchAABB(box, color);
	}

	void QElangPaletteWidget::onHover(Entity self, Entity context) {
		bind(mStage);
		auto box = obj<Box>(self);
		if (box && mHighlight->line.currentBatchCount() == 0) {
			coloring(*box);
		}

		if (gMouse.state(0) == eInput::LIFT) {
			auto celldata = obj<asset<Cell>>(self);
			if (celldata) {
				sig_Clicked.invoke(*celldata);
			}
		}
	}
}
