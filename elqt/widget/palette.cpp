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
			mHovering = NullEntity;
			for (auto i = 0; i < cells.size(); i++) {
				if (asset<CellHolder> holder = cells[i]) {
					bool hit = holder->rect.contains(pos);
					holder->button.update(holder, hit);
				}
			}
			if (gMouse.state(0) == eInput::Lift)
				mHeld = NullEntity;

			if (mHeld && !mHighlightBatched) {
				color8 color = (mHovering == mHeld) ? gEditorColor.cellSelected : gEditorColor.cellHovered;
				mHighlighter->line.batchAABB(mHeld->rect, color);
				color.a = gEditorColor.cellFillAlpha;
				mHighlighter->fill.batchAABB(mHeld->rect, color);
				mHighlightBatched = true;
			}
			updateCursor();
		}
	}

	void QElangPaletteWidget::updateCursor() {
		setCursor((mHovering || mHeld) ? Qt::CursorShape::PointingHandCursor : Qt::CursorShape::ArrowCursor);
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

	void QElangPaletteWidget::onHover(Entity self, Entity context) {
		mHovering = asset<CellHolder>(self);
		if (mHovering) {
			if (gMouse.state(0) == eInput::Once) {
				mHeld = self;
			}

			if (gMouse.state(0) == eInput::Lift && mHeld) {
				if (mHovering == mHeld) {
					sig_Clicked.invoke(mHovering);
					gProject.clear<PaletteSelectedCell>();
					mHovering.add<PaletteSelectedCell>();
				}
			}

			if (!mHeld && !mHighlightBatched) {
				color8 color = gEditorColor.cellHovered;
				mHighlighter->line.batchAABB(mHovering->rect, color);
				color.a = gEditorColor.cellFillAlpha;
				mHighlighter->fill.batchAABB(mHovering->rect, color);
				mHighlightBatched = true;
			}
		}
	}
	void QElangPaletteWidget::postUpdate(Entity self, Entity context) {

	}
}
