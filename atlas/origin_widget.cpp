#include "origin_widget.h"

namespace el
{
	OriginView::OriginView(QWidget* parent)
		: QElangView(parent), mCreateState(MOVING)
	{
		//setFocusPolicy(Qt::StrongFocus);
		setMouseTracking(true);
	}

	OriginView::~OriginView() {
		release();
	}

	void OriginView::onViewStart() {
		connectList();

		glClearColor(0.2f, 0.2f, 0.5f, 1.0f);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		safeCreateObjects();
	}
	
	void OriginView::safeCreateObjects() {
		makeCurrent();

		if (!mMainCam) {
			mMainCam = gProject->makeSub<EditorCamera>();
			mMainCam->to(vec3(0.0f, 0.0f, -1000.0f));

			mPainter = gProject->makeSub<EditorProjectPainter>("basic_sprite", "texture_uv", 32, mMainCam, Projection::eOrtho,
				ePainterFlags::DEPTH_SORT | ePainterFlags::MULTI_MATERIAL | ePainterFlags::Z_CLEAR);
			mPainter->init();

			mHighlighter = new EditorShapeDebug;
			mHighlighter->init(mMainCam);
		}

		if (!mCellObj) {
			mCellObj = gStage->make<EditorProjectSprite>(gAtlsUtil.currentMaterial, mPainter, "");
			mCellObj.add<Position>();
		}
	}

	void OriginView::paintGhostCell() {
		switch (mGhostData.type) {
		case ElangAtlasGhostData::eType::NONE:
		none_label:
			mCellObj->material = NullEntity;
			mCellObj->setCell(asset<Cell>());
			break;
		case ElangAtlasGhostData::eType::PREVIOUS:
			{
				CellItem* item = reinterpret_cast<CellItem*>
					(gAtlsUtil.cellList->item(gAtlsUtil.cellList->currentRow() - 1));
				if (item) {
					mCellObj->material = gAtlsUtil.currentMaterial;
					mCellObj->setCell(item->text().toStdString());
				}
				else goto none_label;
			}
			break;
		default:
			mCellObj->material = mGhostData.material;
			mCellObj->setCell(mGhostData.cell);	
			break;
		}

		mCellObj->update(mCellObj);
		mCellObj->batch();
		drawCellHitbox();
		mPainter->color = vec4(1.0f, 1.0f, 1.0f, 0.35f);
		mPainter->paint();
	}


	void OriginView::drawCellHitbox(bool batchOnly) {
		auto cell = mCellObj->cell();
		if (cell && cell.has<aabb>()) {
			auto& rect = cell.get<aabb>();
			mHighlighter->line.batchAABB(rect, batchOnly ? color8(0, 255, 0, 255) : color8(40, 100, 150, 255));
			mHighlighter->fill.batchAABB(rect, batchOnly ? color8(0, 255, 0, 80) : color8(40, 100, 150, 100));
		} if (!batchOnly) mHighlighter->draw();
	}

	void OriginView::onViewPaint() 	{		
		glClearDepth(1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		updateViewport(-width() / 2, width() / 2, -height() / 2, height() / 2);

		if (mTexture && mTexture->atlas) {
			if (gAtlsUtil.cellList->count() > 0) {
				CellItem* item = reinterpret_cast<CellItem*>(gAtlsUtil.cellList->currentItem());
				if (item) {
					if (mGhostData.order == ElangAtlasGhostData::eOrder::BACK)
						paintGhostCell();

					mCellObj->material = gAtlsUtil.currentMaterial;
					mCellObj->setCell(item->text().toStdString());
					mCellObj->update(mCellObj); // move to on cell change, maybe
					mCellObj->batch();
					mPainter->color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
					mPainter->paint();
					drawCellHitbox(true);

					aabb rect;
					mCellObj->sync(rect);
					mHighlighter->line.batchAABB(rect, color8(255, 255, 255, 255));

					if (mGhostData.order == ElangAtlasGhostData::eOrder::FRONT) {
						paintGhostCell();
						mCellObj->material = gAtlsUtil.currentMaterial;
						mCellObj->setCell(item->text().toStdString());
						mCellObj->update(mCellObj);
					}

					mHighlighter->line.batchAABB(mCreateRect, color8(255, 255, 0, 255));

					mHighlighter->line.batchAABB(mGrabRect, color8(55, 180, 180, 255));
					mHighlighter->fill.batchAABB(mGrabRect, color8(55, 180, 180, 80));

					mHighlighter->line.batchline(line(-1000.0f, 0.0f, 1000.0f, 0.0f), color8(255, 255, 255, 255));
					mHighlighter->line.batchline(line(0.0f, -1000.0f, 0.0f, 1000.0f), color8(255, 255, 255, 255));
					mHighlighter->draw();

				}
			}
		}

	}

	void OriginView::onViewMousePress() {
		if (QApplication::queryKeyboardModifiers() & Qt::ControlModifier)
			mCreateState = eState::CREATING;

		switch (mCreateState) {
		case eState::CREATING:
		{
			auto pos = *mMainCam * gMouse.currentPosition();
			mCreateRect = aabb(pos.x, pos.y, pos.x, pos.y);
		}
			break;
		case eState::MOVING:
			if (gMouse.state(0) == eInput::ONCE && cursor() == Qt::OpenHandCursor) {
				CellItem* item = reinterpret_cast<CellItem*>(gAtlsUtil.cellList->currentItem());
				assert(item && item->holder);
				setCursor(Qt::ClosedHandCursor);
				mGrabPos = *mMainCam * gMouse.currentPosition();
				mGrabUV = vec2(item->holder->cell->oX, item->holder->cell->oY);
				update();
			}
			break;
		case eState::MOVING_HITBOX:
			if (gMouse.state(0) == eInput::ONCE) {
				CellItem* item = reinterpret_cast<CellItem*>(gAtlsUtil.cellList->currentItem());
				assert(item && item->holder);
				auto cell = item->holder->cell;
				assert(cell && cell.has<aabb>());
				if (cursor() == Qt::OpenHandCursor) {
					setCursor(Qt::ClosedHandCursor);
					mGrabPos = *mMainCam * gMouse.currentPosition();
					mGrabRect = cell.get<aabb>();
				} else if (mCursorState > 0) {
					mGrabRect = cell.get<aabb>();
				}
			}
			break;
		};
	}

	void OriginView::onViewMouseMove() {
		if (cursor() == Qt::OpenHandCursor || cursor() == Qt::ArrowCursor) {
			if (mCreateState == eState::MOVING && QApplication::queryKeyboardModifiers() & Qt::AltModifier)
				mCreateState = eState::MOVING_HITBOX;
			else if (mCreateState == eState::MOVING_HITBOX && !(QApplication::queryKeyboardModifiers() & Qt::AltModifier))
				mCreateState = eState::MOVING;
		}
		

		auto mpos = *mMainCam * gMouse.currentPosition();
		CellItem* item = reinterpret_cast<CellItem*>(gAtlsUtil.cellList->currentItem());
		switch (mCreateState) {
		case eState::CREATING:
			mCreateRect.r = mpos.x;
			mCreateRect.t = mpos.y;
			break;
		case eState::MOVING:
			if (item && item->holder) {
				if (cursor() == Qt::ClosedHandCursor) {
					auto delta = (mpos - mGrabPos);//*mMainCam * (mpos - mGrabPos);
					item->holder->cell->oX = int(delta.x + mGrabUV.x);
					item->holder->cell->oY = int(-delta.y + mGrabUV.y);
					item->holder->moldCellFromRect(mTexture->width(), mTexture->height(), item->holder->cell->index);
					update();
				} else {
					setCursor(Qt::ArrowCursor);
					if (!(QApplication::queryKeyboardModifiers() & Qt::ControlModifier) && 
						!(QApplication::queryKeyboardModifiers() & Qt::AltModifier)) {
						aabb rect;
						mCellObj->sync(rect);
						if (rect.contains(mpos)) {
							setCursor(Qt::OpenHandCursor);
						}
					}
				}
			}
		break;
		case eState::MOVING_HITBOX:
			if (item && item->holder) {
				auto cell = item->holder->cell;
				assert(cell);
				if (cell.has<aabb>()) {
					auto& rect = cell.get<aabb>();

					if (gMouse.state(0) == eInput::HOLD) {
						if (cursor() == Qt::ClosedHandCursor) {
							auto delta = (mpos - mGrabPos);
							mGrabRect = rect;
							mGrabRect.move(delta);
							update();
						} else if (mCursorState > 0) {
							if ((mCursorState & LEFT) == LEFT) {
								mGrabRect.l = mpos.x;
							}
							else if ((mCursorState & RIGHT) == RIGHT) {
								mGrabRect.r = mpos.x;
							}
							if ((mCursorState & BOTTOM) == BOTTOM) {
								mGrabRect.b = mpos.y;
							}
							else if ((mCursorState & TOP) == TOP) {
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
								mCursorState += CursorState::LEFT;
							else if (rect.r < mpos.x + 10.0f * sx)
								mCursorState += CursorState::RIGHT;
							if (rect.b > mpos.y - 10.0f * sy)
								mCursorState += CursorState::BOTTOM;
							else if (rect.t < mpos.y + 10.0f * sy)
								mCursorState += CursorState::TOP;

							bool left = ((mCursorState & CursorState::LEFT) == CursorState::LEFT);
							bool right = ((mCursorState & CursorState::RIGHT) == CursorState::RIGHT);
							bool top = ((mCursorState & CursorState::TOP) == CursorState::TOP);
							bool bottom = ((mCursorState & CursorState::BOTTOM) == CursorState::BOTTOM);

							if (left || right) {
								setCursor(Qt::SizeHorCursor);
							} if (top || bottom) {
								if (cursor() == Qt::SizeHorCursor) {
									if ((left && top) || (right && bottom)) {
										setCursor(Qt::SizeFDiagCursor);
									}
									else setCursor(Qt::SizeBDiagCursor);
								}
								else
									setCursor(Qt::SizeVerCursor);
							}
						}
					}
				} else
					setCursor(Qt::ArrowCursor);
			}
			break;
		}
	}

	void OriginView::onViewMouseRelease() {
		CellItem* item = reinterpret_cast<CellItem*>(gAtlsUtil.cellList->currentItem());
		switch (mCreateState) {
		case eState::CREATING:
			if (item && item->holder) {
				mCreateRect.normalize();
				gProject->emplace_or_replace<aabb>(item->holder->cell, mCreateRect);
			}

			mCreateRect = aabb(-10000, -10000, -10000, -10000);
			mCreateState = MOVING;
			break;
		case eState::MOVING:
			if (gMouse.state(0) == eInput::LIFT && cursor() == Qt::ClosedHandCursor) {
				setCursor(Qt::OpenHandCursor);
				update();
			}
			break;
		case eState::MOVING_HITBOX:
			if (gMouse.state(0) == eInput::LIFT && item && item->holder) {
				if (cursor() == Qt::ClosedHandCursor || mCursorState > 0) {
					mGrabRect.normalize();
					mGrabRect.roundCorners();
					gProject->emplace_or_replace<aabb>(item->holder->cell, mGrabRect);
					setCursor(Qt::OpenHandCursor);
					update();
				}
			}

			mGrabRect = aabb(-10000, -10000, -10000, -10000);
			break;
		}
	}


	void OriginView::onViewScrollWheel() {
		float val = (5.0f / 6.0f);
		val = pow(val, gMouse.wheel());
		if (val != 0) {
			val = (val > 0) ? val : -1.0f / val; // scroll up: 5/6, down: 6/5

			auto mpos = *mMainCam * gMouse.currentPosition();
			auto cpos = mMainCam->position();
			auto delta = mpos - cpos;
			delta *= (1 - val);

			mMainCam->scale(vec3(val, val, 1));
			mMainCam->move(vec3(delta, 0));
		}
	}

	void OriginView::connectList() {
		connect(gAtlsUtil.cellList, &QListExtension::currentRowChanged, [&]() {
			//setFocus();
			update();
		});
	}

	void OriginView::updateTexture() {
		auto mat = gAtlsUtil.currentMaterial;
		if (mat && mat->hasTexture()) {
			mTexture = mat->textures[0];
		}
	}

	void OriginView::showEditor() {
		auto& list = *gAtlsUtil.cellList;
		list.setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
		list.setDragDropMode(QAbstractItemView::DragDropMode::NoDragDrop);
		list.setDefaultDropAction(Qt::DropAction::IgnoreAction);
		list.show();
		show();
	}

	void OriginView::hideEditor() {
		gAtlsUtil.cellList->hide();
		hide();
	}

	void OriginView::moveCellOrigin(int x, int y) {
		if (mTexture && mTexture->atlas && cursor() != Qt::ClosedHandCursor) {
			CellItem* item = reinterpret_cast<CellItem*>(gAtlsUtil.cellList->currentItem());
			if (item && item->holder) {
				item->holder->cell->oX += x;
				item->holder->cell->oY += y;
				item->holder->moldCellFromRect(mTexture->width(), mTexture->height(), item->holder->cell->index);
			} update();
		}
	}

	void OriginView::shiftCell(int dir) {
		if (cursor() != Qt::ClosedHandCursor) {
			auto& list = *gAtlsUtil.cellList;
			int row = list.currentRow() + dir;
			row = clamp(row, 0, list.count() - 1);
			list.setCurrentRow(row);
		} update();
	}

	void OriginView::captureGhost() {
		CellItem* item = reinterpret_cast<CellItem*>(gAtlsUtil.cellList->currentItem());
		if (item) {
			assert(item->holder);
			mGhostData.material = gAtlsUtil.currentMaterial;
			mGhostData.type = ElangAtlasGhostData::eType::CUSTOM;
			mGhostData.order = ElangAtlasGhostData::eOrder::BACK;
			mGhostData.cell = item->holder->cell;
		} update();
	}

	void OriginView::onKeyPress(QKeyEvent* e) {
		if (e->key() == Qt::Key::Key_Control || e->key() == Qt::Key::Key_Alt) {
			onViewMouseMove();
		}
	}

	void OriginView::onKeyRelease(QKeyEvent* e) {
		if (e->key() == Qt::Key::Key_Alt || e->key() == Qt::Key::Key_Control) {
			onViewMouseMove();
		}
	}
}