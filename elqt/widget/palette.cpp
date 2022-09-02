#include <elqtpch.h>
#include "palette.h"

#include <apparatus/ui.h>
#include <tools/material.h>
#include <tools/controls.h>
#include <tools/painter.h>
#include <tools/atlas.h>

#include "../color_code.h"

namespace el {
	QElangPaletteWidget::QElangPaletteWidget(QWidget* parent, bool internalLoop)
		: QElangTextureWidget(parent, internalLoop), mHighlightBatched(false), mCellShapes(0)
	{
		ui.view->setMouseTracking(true);

		ui.view->sig_Start.connect([&]() {
			safeCreatePalette();
			rebatchAllCellHolders();
		});

		ui.view->sig_Paint.connect([&]() {
			mCellShapes->draw();
			mHighlighter->draw();
			mHighlightBatched = false;
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

	void QElangPaletteWidget::updateAtlas(asset<Atlas> atlas) {
		gMouse.reset();
		mAtlas = atlas;
		recreateCellHoldersFromAtlas();
		rebatchAllCellHolders();
		ui.view->update();
	}

	void QElangPaletteWidget::loop() {
		if (isActiveWindow() && mMainCam) {
			if (gMouse.state(1) == eInput::Hold) {
				mMainCamTarget.move(vec3(mMoveDelta * 0.5, 0));
				syncCameraTarget();
				tweenCameraInput(mMainCamTween, *mMainCam, mMainCamTarget);
				//snapCamera();
				ui.view->update();
			}

			if (gMouse.state(1) == eInput::Hold || gMouse.wheel() != 0.0f) {
				updateAllHolderCheck();
			}
			if (mMainCamTween.progress() != 1.0f) {
				mMainCamTween.step(1);
				ui.view->update();
			}
		}
	}

	void QElangPaletteWidget::cleanAtlas() {
		if (mAtlas) {
			auto& cells = mAtlas.get<AtlasMeta>().cellorder;
		}
	}

	void QElangPaletteWidget::recreateCellHoldersFromAtlas() {
		if (mAtlas && mAtlas.has<AssetLoaded>()) {
			auto& meta = mAtlas.get<AtlasMeta>();

			for (auto i = 0; i < meta.cellorder.size(); i++) {
				asset<CellMeta> cm = meta.cellorder[i];
				if (cm.has<CellHolder>()) {
					cm.get<CellHolder>().button.setEvent(this);
				} else {
					auto& cell = cm.get<Cell>();

					Box rect;
					rect.l = cell.uvLeft * meta.width;
					rect.r = cell.uvRight * meta.width;
					rect.b = -cell.uvDown * meta.height;
					rect.t = -cell.uvUp * meta.height;

					gProject.emplace<CellHolder>(cm, rect, this);
				}
			}
		}
	}
	
	void QElangPaletteWidget::rebatchAllCellHolders() {
		if (mAtlas && mAtlas.has<AssetLoaded>() && mCellShapes) {
			auto& cells = mAtlas.get<AtlasMeta>().cellorder;
			forceUnlockDebuggers();
			resetMainCamera();
			for (asset<CellHolder> holder : cells) {
				mCellShapes->line.batchAABB(holder->rect, gEditorColor.cell);
			}
			for (asset<CellHolder> holder : gProject.view<PaletteSelectedCell>()) {
				auto color = gEditorColor.cell;
				color.a = 80;
				mCellShapes->fill.batchAABB(holder->rect, color);
			}
			mCellShapes->line.flags |= ePainterFlags::LOCKED;
			mCellShapes->fill.flags |= ePainterFlags::LOCKED;
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

	
	void QElangPaletteWidget::updateAllHolderCheck() {
		if (mMaterial && mMaterial->hasTexture() && mAtlas && mMainCam) {
			auto& cells = mAtlas.get<AtlasMeta>().cellorder;
			auto pos = *mMainCam * gMouse.currentPosition();
			for (auto i = 0; i < cells.size(); i++) {
				if (asset<CellHolder> holder = cells[i]) {
					bool hit = holder->rect.contains(pos);
					holder->button.update(holder, hit);
				}
			}
		}
	}

	void QElangPaletteWidget::safeCreatePalette() {
		if (!mCellShapes) {
			ui.view->makeCurrent();
			mCellShapes = new ShapeDebug2d;
			mCellShapes->init(mMainCam);

			mHighlighter = new ShapeDebug2d;
			mHighlighter->init(mMainCam);
		}
	}

	
	void QElangPaletteWidget::coloring(Box& box) {
		color8 color = (gMouse.state(0) >= eInput::Once) ?
			gEditorColor.cellSelected : gEditorColor.cellHovered;
		mHighlighter->line.batchAABB(box, color);
		color.a = gEditorColor.cellFillAlpha;
		mHighlighter->fill.batchAABB(box, color);
	}
	
	void QElangPaletteWidget::onHover(Entity self, Entity context) {
		auto holder = asset<CellHolder>(self);
		if (holder) {
			if (!mHighlightBatched) {
				coloring(holder->rect);
				mHighlightBatched = true;
			}

			if (gMouse.state(0) == eInput::Lift) {
				if (holder) {
					sig_Clicked.invoke(holder);
					gProject.clear<PaletteSelectedCell>();
					holder.add<PaletteSelectedCell>();
				}
			}
		}
	}
}
