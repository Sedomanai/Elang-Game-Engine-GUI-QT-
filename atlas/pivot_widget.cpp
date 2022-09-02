#include <elqtpch.h>
#include "pivot_widget.h"
#include "cells_widget.h"

#include <common/algorithm.h>
#include <common/line.h>
#include <tools/painter.h>
#include <tools/material.h>
#include <tools/controls.h>
#include <tools/texture.h>
#include <tools/atlas.h>
#include <apparatus/ui.h>
#include <apparatus/asset_loader.h>

namespace el
{
	PivotView::PivotView(QWidget* parent)
		: QElangView(parent), mCreateState(eState::Moving)
	{
		//setFocusPolicy(Qt::StrongFocus);
		setupCameraTween(mMainCamTween);
		setMouseTracking(true);
	}

	PivotView::~PivotView() {
		release();
	}

	void PivotView::release() {
		if (mMainCam) {
			mMainCam.destroy();
			mPainter.destroy();
			delete mHighlighter;
		}
	}
	void PivotView::snapCamera() {
		*mMainCam = mMainCamTarget;
		setupCameraTween(mMainCamTween);
	}

	void PivotView::loop() {
		if (isActiveWindow() && mMainCam) {
			if (mMainCamTween.progress() != 1.0f) {
				mMainCamTween.step(1);
				onViewMouseMove();
				update();
			}
		} 
	}

	void PivotView::onViewStart() {
		connectList();

		glClearColor(0.2f, 0.2f, 0.5f, 1.0f);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		safeCreateObjects();
	}

	void PivotView::safeCreateObjects() {
		makeCurrent();

		if (!mMainCam) {
			mMainCam = gProject.make<Camera>().add<EditorAsset>();
			mMainCam->to(vec3(0.0f, 0.0f, -1000.0f));
			snapCamera();

			mPainter = gProject.make<Painter>("__el_editor_/shader/basic_sprite.vert", "__el_editor_/shader/texture_uv.frag", 
				32, mMainCam, Projection::eOrtho,
				ePainterFlags::DEPTH_SORT | ePainterFlags::MULTI_MATERIAL | ePainterFlags::Z_CLEAR).add<EditorAsset>();
			mPainter->init();

			mHighlighter = new ShapeDebug2d;
			mHighlighter->init(mMainCam);
		}
		
		mCellSprite = { gAtlsUtil.currentMaterial, mPainter, "" };
		mGhostSprite = { 0, mPainter, "" };
	}

	void PivotView::paintGhostCell() {
		bool none = false;
		switch (mGhostData.type) {
		case ElangAtlasGhostData::eType::NONE: none = true; break;
		case ElangAtlasGhostData::eType::PREVIOUS:
			{
				CellItem* item = reinterpret_cast<CellItem*>(gAtlsUtil.cellList->item(gAtlsUtil.cellList->currentRow() - 1));
				if (item) {
					mGhostSprite.material = gAtlsUtil.currentMaterial;
					mGhostSprite.setCell(item->text().toStdString());
				} else none = true;
			}
			break;
		default:
			mGhostSprite.material = mGhostData.material;
			mGhostSprite.setCell(mGhostData.cell);
			break;
		}

		if (none) {
			mGhostSprite.material = NullEntity;
			mGhostSprite.setCell(asset<Cell>());
		}

		if (mGhostSprite.cell() != asset<Cell>()) {
			mGhostSprite.recalc(mCellPos);
			mGhostSprite.batch();
			//drawCellHitbox();
			mPainter->color = vec4(1.0f, 1.0f, 1.0f, 0.35f);
			mPainter->paint();
		}
	}

	void PivotView::execGhostDialog() {
		ElangAtlasGhostDialog dialog(mGhostData);
		dialog.exec();
	}

	void PivotView::moveCurrentCell() {
		auto mpos = *mMainCam * gMouse.currentPosition();
		CellItem* item = reinterpret_cast<CellItem*>(gAtlsUtil.cellList->currentItem());
		if (item && item->holder && cursor() == Qt::ClosedHandCursor) {
			auto& meta = item->holder.get<CellMeta>();
			auto delta = (mpos - mGrabPos);//*mMainCam * (mpos - mGrabPos);
			meta.oX = -int(delta.x + mGrabUV.x);
			meta.oY = int(delta.y + mGrabUV.y);
			assert(gAtlsUtil.currentAtlas);
			auto& atlasmeta = gAtlsUtil.currentAtlas.get<AtlasMeta>();
			item->holder->moldCellFromRect(item->holder, atlasmeta.width, atlasmeta.height);
			update();
		}
	}

	void PivotView::drawCellHitbox(bool batchOnly) {
		auto cell = mCellSprite.cell();
		if (cell && cell.has<CellHolder>()) {
			auto& rect = cell.get<CellHolder>().hitbox;
			mHighlighter->line.batchAABB(rect, batchOnly ? color8(0, 255, 0, 255) : color8(40, 100, 150, 255));
			mHighlighter->fill.batchAABB(rect, batchOnly ? color8(0, 255, 0, 80) : color8(40, 100, 150, 100));
		} if (!batchOnly) mHighlighter->draw();
	}

	void PivotView::onViewPaint() {		
		glClearDepth(1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		auto width = (int)mWidth;
		auto height = (int)mHeight;

		auto lw = width / 2;
		auto rw = (width % 2 == 1) ? lw : lw + 1;
		auto bh = height / 2;
		auto uh = (height % 2 == 1) ? bh : bh + 1;
		updateViewport(-lw, rw, -bh, uh);

		assert(gAtlsUtil.currentAtlas);
		if (gAtlsUtil.cellList->count() > 0) {
			CellItem* item = reinterpret_cast<CellItem*>(gAtlsUtil.cellList->currentItem());
			if (item) {
				if (mGhostData.order == ElangAtlasGhostData::eOrder::BACK)
					paintGhostCell();

				mCellSprite.setCell(item->text().toStdString());
				mCellSprite.recalc(mCellPos);
				mCellSprite.batch();

				mPainter->color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
				mPainter->paint();
				drawCellHitbox(true);

				auto& hitbox = item->holder->hitbox;
				mHighlighter->line.batchAABB(hitbox, color8(255, 0, 0, 255));
				mHighlighter->fill.batchAABB(hitbox, color8(255, 0, 0, 80));

				aabb rect;
				mCellSprite.sync(rect);
				mHighlighter->line.batchAABB(rect, color8(255, 255, 255, 255));

				if (mGhostData.order == ElangAtlasGhostData::eOrder::FRONT) {
					paintGhostCell();
				}

				mHighlighter->line.batchAABB(mCreateRect, color8(255, 255, 0, 255));

				mHighlighter->line.batchAABB(mGrabRect, color8(55, 180, 180, 255));
				mHighlighter->fill.batchAABB(mGrabRect, color8(55, 180, 180, 80));

				mHighlighter->line.batchline(line(-1000000.0f, 0.0f, 1000000.0f, 0.0f), color8(255, 255, 255, 255));
				mHighlighter->line.batchline(line(0.0f, -1000000.0f, 0.0f, 1000000.0f), color8(255, 255, 255, 255));
				mHighlighter->draw();
			}
		}
	}

	void PivotView::onViewMousePress() {
		if (QApplication::queryKeyboardModifiers() & Qt::ControlModifier)
			mCreateState = eState::Creating;

		if (gMouse.state(2) == eInput::Once) {
			auto pos = *mMainCam * gMouse.currentPosition();
			auto w = size().width() / 2.0f;
			auto h = size().height() / 2.0f;
			aabb trapbox(-w, -h, w, h);
			trapbox.trap(pos);
			mMainCamTarget.to(vec3(pos, -1000.0f));
			tweenCameraInput(mMainCamTween, *mMainCam, mMainCamTarget);
		}

		switch (mCreateState) {
		case eState::Creating:
		{
			auto pos = *mMainCam * gMouse.currentPosition();
			mCreateRect = aabb(pos.x, pos.y, pos.x, pos.y);
		}
			break;
		case eState::Moving:
			if (gMouse.state(0) == eInput::Once && cursor() == Qt::OpenHandCursor) {
				CellItem* item = reinterpret_cast<CellItem*>(gAtlsUtil.cellList->currentItem());
				assert(item && item->holder);
				setCursor(Qt::ClosedHandCursor);
				mGrabPos = *mMainCam * gMouse.currentPosition();
				auto& meta = item->holder.get<CellMeta>();
				mGrabUV = vec2(-meta.oX, meta.oY);
				update();
			}
			break;
		case eState::MovingHitbox:
			if (gMouse.state(0) == eInput::Once) {
				CellItem* item = reinterpret_cast<CellItem*>(gAtlsUtil.cellList->currentItem());
				assert(item && item->holder);
				if (cursor() == Qt::OpenHandCursor) {
					setCursor(Qt::ClosedHandCursor);
					mGrabPos = *mMainCam * gMouse.currentPosition();
					mGrabRect = item->holder->hitbox;
				} else if (mCursorState > 0) {
					mGrabRect = item->holder->hitbox;
				}
			}
			break;
		};
	}

	void PivotView::onViewMouseMove() {
		if (cursor() == Qt::OpenHandCursor || cursor() == Qt::ArrowCursor) {
			if (mCreateState == eState::Moving && QApplication::queryKeyboardModifiers() & Qt::AltModifier)
				mCreateState = eState::MovingHitbox;
			else if (mCreateState == eState::MovingHitbox && !(QApplication::queryKeyboardModifiers() & Qt::AltModifier))
				mCreateState = eState::Moving;
		}

		auto mpos = *mMainCam * gMouse.currentPosition();
		CellItem* item = reinterpret_cast<CellItem*>(gAtlsUtil.cellList->currentItem());
		switch (mCreateState) {
		case eState::Creating:
			mCreateRect.r = mpos.x;
			mCreateRect.t = mpos.y;
			break;
		case eState::Moving:
			if (item && item->holder) {
				if (cursor() == Qt::ClosedHandCursor) {
					moveCurrentCell();
				} else {
					setCursor(Qt::ArrowCursor);
					if (!(QApplication::queryKeyboardModifiers() & Qt::ControlModifier) && 
						!(QApplication::queryKeyboardModifiers() & Qt::AltModifier)) {
						aabb rect;
						mCellSprite.sync(rect);
						if (rect.contains(mpos)) {
							setCursor(Qt::OpenHandCursor);
						}
					}
				}
			}
		break;
		case eState::MovingHitbox:
			if (item && item->holder) {
				//auto cell = item->holder->cell;
				//assert(cell);

				auto& rect = item->holder->hitbox;
				if (gMouse.state(0) == eInput::Hold) {
					if (cursor() == Qt::ClosedHandCursor) {
						auto delta = (mpos - mGrabPos);
						mGrabRect = rect;
						mGrabRect.move(delta);
						update();
					} else if (mCursorState > 0) {
						if ((mCursorState & eLeft) == eLeft) {
							mGrabRect.l = mpos.x;
						} else if ((mCursorState & eRight) == eRight) {
							mGrabRect.r = mpos.x;
						}
						if ((mCursorState & eBottom) == eBottom) {
							mGrabRect.b = mpos.y;
						} else if ((mCursorState & eTop) == eTop) {
							mGrabRect.t = mpos.y;
						}
					}
				} else {
					setCursor(Qt::ArrowCursor);
					if (rect.contains(mpos)) {
						setCursor(Qt::OpenHandCursor);

						auto sx = mMainCam->scale().x;
						auto sy = mMainCam->scale().y;
						mCursorState = 0;
						if (rect.l > mpos.x - 10.0f * sx)
							mCursorState += CursorState::eLeft;
						else if (rect.r < mpos.x + 10.0f * sx)
							mCursorState += CursorState::eRight;
						if (rect.b > mpos.y - 10.0f * sy)
							mCursorState += CursorState::eBottom;
						else if (rect.t < mpos.y + 10.0f * sy)
							mCursorState += CursorState::eTop;

						bool left = ((mCursorState & CursorState::eLeft) == CursorState::eLeft);
						bool right = ((mCursorState & CursorState::eRight) == CursorState::eRight);
						bool top = ((mCursorState & CursorState::eTop) == CursorState::eTop);
						bool bottom = ((mCursorState & CursorState::eBottom) == CursorState::eBottom);

						if (left || right) {
							setCursor(Qt::SizeHorCursor);
						} if (top || bottom) {
							if (cursor() == Qt::SizeHorCursor) {
								if ((left && top) || (right && bottom)) {
									setCursor(Qt::SizeFDiagCursor);
								} else setCursor(Qt::SizeBDiagCursor);
							} else
								setCursor(Qt::SizeVerCursor);
						}
					}
				}
			}
			break;
		}
	}

	void PivotView::onViewMouseRelease() {
		CellItem* item = reinterpret_cast<CellItem*>(gAtlsUtil.cellList->currentItem());
		switch (mCreateState) {
		case eState::Creating:
			if (item && item->holder) {
				mCreateRect.normalize();
				item->holder->hitbox = mCreateRect;
			}

			mCreateRect = aabb(-10000, -10000, -10000, -10000);
			mCreateState = eState::Moving;
			break;
		case eState::Moving:
			if (gMouse.state(0) == eInput::Lift && cursor() == Qt::ClosedHandCursor) {
				sig_Modified.invoke();
				setCursor(Qt::OpenHandCursor);
				update();
			}
			break;
		case eState::MovingHitbox:
			if (gMouse.state(0) == eInput::Lift && item && item->holder) {
				if (cursor() == Qt::ClosedHandCursor || mCursorState > 0) {
					mGrabRect.normalize();
					mGrabRect.roundCorners();
					item->holder->hitbox = mGrabRect;
					setCursor(Qt::OpenHandCursor);
					update();
				}
			}

			mGrabRect = aabb(-10000, -10000, -10000, -10000);
			break;
		}
	}

	void PivotView::onViewScrollWheel() {
		if (zoomCamera(mMainCamTarget, false, 2.0f)) {
			tweenCameraInput(mMainCamTween, *mMainCam, mMainCamTarget);
		}
	}

	void PivotView::connectList() {
		connect(gAtlsUtil.cellList, &QListExtension::currentRowChanged, [&]() {
			//setFocus();
			update();
		});
	}

	void PivotView::showEditor() {
		auto& list = *gAtlsUtil.cellList;
		list.setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
		list.setDragDropMode(QAbstractItemView::DragDropMode::NoDragDrop);
		list.setDefaultDropAction(Qt::DropAction::IgnoreAction);
		list.show();

		auto curr = gProject.view<AtlasCurrentCell>();
		if (curr.size() > 0) {
			list.setCurrentItem(*asset<CellItem*>(curr[0]));
		}

		show();
	}

	void PivotView::hideEditor() {
		gProject.clear<AtlasCurrentCell>();
		auto item = reinterpret_cast<CellItem*>(gAtlsUtil.cellList->currentItem());
		if (item)
			item->holder.add<AtlasCurrentCell>();
		gAtlsUtil.cellList->hide();
		hide();
	}

	void PivotView::incrementPivot(int x, int y) {
		if (cursor() != Qt::ClosedHandCursor) {
			CellItem* item = reinterpret_cast<CellItem*>(gAtlsUtil.cellList->currentItem());
			if (item && item->holder) {
				auto atlas = gAtlsUtil.currentAtlas;
				auto& ameta = atlas.get<AtlasMeta>();
				auto& meta = item->holder.get<CellMeta>();
				meta.oX -= x;
				meta.oY += y;
				item->holder->moldCellFromRect(item->holder, ameta.width, ameta.height);
				sig_Modified.invoke();
			} update();
		}
	}

	void PivotView::shiftCell(int dir) {
		if (cursor() != Qt::ClosedHandCursor) {
			auto& list = *gAtlsUtil.cellList;
			int row = list.currentRow() + dir;
			row = clamp(row, 0, list.count() - 1);
			list.setCurrentRow(row);
		} update();
	}

	void PivotView::setPreviousGhost() {
		mGhostData.type = ElangAtlasGhostData::eType::PREVIOUS;
		mGhostData.material = gAtlsUtil.currentMaterial;
		update();
	}

	void PivotView::captureGhost() {
		CellItem* item = reinterpret_cast<CellItem*>(gAtlsUtil.cellList->currentItem());
		if (item) {
			assert(item->holder);
			mGhostData.type = ElangAtlasGhostData::eType::INDEXED;
			mGhostData.material = gAtlsUtil.currentMaterial;
			mGhostData.cell = item->holder;
		} update();
	}

	void PivotView::ghostPalette() {
		if (mGhostData.type != ElangAtlasGhostData::eType::EXTERNAL) { // NONE || PREVIOUS
			mGhostData.material = gAtlsUtil.currentMaterial;
			mGhostData.type = ElangAtlasGhostData::eType::INDEXED;
		} 

		QDialog dialog;
		AtlasPalette palette(&dialog, true);

		palette.updateAtlas(mGhostData.material->textures[0]->atlas);
		palette.updateMaterial(mGhostData.material, gAtlsUtil.globalPalettePositon, gAtlsUtil.globalPaletteScale);
		palette.sig_Clicked.connect([&](asset<Cell> cell) {
			mGhostData.cell = cell;
			dialog.close();
		});
		dialog.exec();
		gAtlsUtil.globalPalettePositon = palette.camPosition();
		gAtlsUtil.globalPaletteScale = palette.camScale();
		update();
	}

	//void PivotView::onKeyPress(QKeyEvent* e) {
	//	if (e->key() == Qt::Key::Key_Control || e->key() == Qt::Key::Key_Alt) {
	//		onViewMouseMove();
	//	}
	//}

	//void PivotView::onKeyRelease(QKeyEvent* e) {
	//	if (e->key() == Qt::Key::Key_Alt || e->key() == Qt::Key::Key_Control) {
	//		onViewMouseMove();
	//	}
	//}
}