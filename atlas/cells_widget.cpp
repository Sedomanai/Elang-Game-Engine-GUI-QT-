#include "cells_widget.h"
#include <qmenu.h>
#include <qlineedit.h>


namespace el
{
	CellsWidget::CellsWidget(QWidget* parent) : 
		mState(CellsWidget::SNONE), QElangPaletteWidget(parent), mSuppressSelect(false), mAlphaCut(10)
	{
		ui.view->sig_Start.connect([&]() {
			connectMouseInput();
			});

		ui.view->sig_Paint.connect([&]() {
			bind(mStage);

			for (sizet i = 0; i < mSelects.size(); i++) {
				mHighlighter->line.batchAABB(mSelects[i]->rect, color8(30, 255, 220, 255), 0.0f);
				mHighlighter->fill.batchAABB(mSelects[i]->rect, color8(30, 255, 220, 80), 0.0f);
			}

			switch (mState) {
			case SELECTING:
				mHighlighter->line.batchAABB(mHighlightRect, color8(255, 255, 255, 255), -10.0f);
				break;
			} 
			
			
			mHighlighter->draw();
		});
		//connectList();
	}

	void CellsWidget::autoGenCells(uint sortorder, uint target_margin) {
		bind(mStage);
		if (gGUI.open() && mTexture) {
			mTexture->autoGenerateAtlas(mTexture, mAlphaCut);
			onTextureUpdate();
			update();
		}
	}

	void CellsWidget::connectMouseInput() {
		ui.view->sig_MousePress.connect([&]() {
			if (gGUI.open() && gMouse.state(0) == eInput::ONCE) {
				bind(mStage);
				auto pos = *mMainCam * gMouse.currentPosition();
				//findCursorState(pos);
				if (QApplication::keyboardModifiers() & Qt::AltModifier) {
					if (mCursorState == MOVE)
						mState = MOVING;
					else if (mCursorState > MOVE) {
						mState = SIZING;
					}
				} else {
					mState = SELECTING;
					mHighlightRect.l = pos.x;
					mHighlightRect.r = pos.x;
					mHighlightRect.t = pos.y;
					mHighlightRect.b = pos.y;
				}

				ui.view->update();
			}
		});
		ui.view->sig_MouseRelease.connect([&]() {
			if (gGUI.open() && gMouse.state(0) == eInput::LIFT) {
				bind(mStage);
				switch (mState) {
				case SIZING:
				{
					assert(mSelects.size() > 0);
					auto sel = mSelects[0];
					assert(sel);
					sel->rect = mSelectRect;
					sel->reshapeCell((int)mTexture->width(), (int)mTexture->height());
				}
					break;
				case MOVING:
					for (uint i = 0; i < mSelects.size(); i++) {
						auto sel = mSelects[i];
						assert(sel);
						//sel->rect.move() // "how much?
						sel->reshapeCell((int)mTexture->width(), (int)mTexture->height());
					}
					break;
				case SELECTING:
					if (mTexture && mTexture->atlas) {
						mSuppressSelect = true;
						gAtlsUtil.cellList->clearSelection();
						mSelects.clear();
						mHighlightRect.normalize();
						{
							for (obj<CellHolder> holder : gStage->view<CellHolder>()) {
								if (holder->rect.intersects(mHighlightRect)) {
									mSelects.emplace_back(holder);
									auto it = gAtlsUtil.cellItems.find(holder->cell);
									if (it != gAtlsUtil.cellItems.end())
										it->second->setSelected(true);
								}
							}
						}

						mSuppressSelect = false;
					}

					break;
				}

				highlightSelected();
				mState = SNONE;
				ui.view->update();
			}
		});

		ui.view->sig_MouseMove.connect([&]() {
			if (gGUI.open()) {
				bind(mStage);
				auto pos = *mMainCam * gMouse.currentPosition();
				//findCursorState(pos);

				if (gMouse.state(0) == eInput::HOLD) {
					auto delta = *mMainCam * gMouse.deltaPosition();
					delta -= mMainCam->position();

					switch (mState) {
					case SELECTING:
						mHighlightRect.r = pos.x;
						mHighlightRect.t = pos.y;
						ui.view->update();
						break;

					case MOVING:
						mSelectRect.l += delta.x;
						mSelectRect.r += delta.x;
						mSelectRect.t += delta.y;
						mSelectRect.b += delta.y;

						//for (uint i = 0; i < mSelects.size(); i++) {
						//	auto& rect = mSelects[i];
						//	rect->rect.l += delta.x;
						//	rect->rect.r += delta.x;
						//	rect->rect.t += delta.y;
						//	rect->rect.b += delta.y;
						//}
						ui.view->update();
						break;

					case SIZING:
						//if (mSelects.size() == 1) {
						//	auto& rect = mSelects[0];

						//	if ((mCursorState & LEFT) == LEFT) {
						//		mSelectRect.l = rect->rect.l = pos.x;
						//	}
						//	else if ((mCursorState & RIGHT) == RIGHT) {
						//		mSelectRect.r = rect->rect.r = pos.x;
						//	}
						//	if ((mCursorState & BOTTOM) == BOTTOM) {
						//		mSelectRect.b = rect->rect.b = pos.y;
						//	}
						//	else if ((mCursorState & TOP) == TOP) {
						//		mSelectRect.t = rect->rect.t = pos.y;
						//	}
						//	ui.view->update();
						//}
						break;
					}
				}
			}
		});
	}

	void CellsWidget::connectList() {
		connect(gAtlsUtil.cellList, &QListExtension::itemSelectionChanged, [&]() {
			if (!gGUI.open())
				return;

			bind(mStage);
			if (isVisible() && !mSuppressSelect) {
				mSelects.clear();
				assert(mTexture && mTexture->atlas);
				auto& cells = mTexture->atlas->cells;

				for (uint i = 0; i < gAtlsUtil.cellList->count(); i++) {
					auto item = gAtlsUtil.cellList->item(i);
					if (item->isSelected()) {
						auto name = item->text().toStdString();
						if (cells.contains(name))
							mSelects.emplace_back(cells[name]);
					}
				}
				highlightSelected();
				ui.view->update();
				setFocus();
			}
		});
	}

	void CellsWidget::highlightSelected() {
		mSelectRect = Box(INFINITY, INFINITY, -INFINITY, -INFINITY);

		for (sizet i = 0; i < mSelects.size(); i++) {
			auto& rect = mSelects[i]->rect;
			mSelectRect.l = min(mSelectRect.l, rect.l);
			mSelectRect.b = min(mSelectRect.b, rect.b);
			mSelectRect.r = max(mSelectRect.r, rect.r);
			mSelectRect.t = max(mSelectRect.t, rect.t);
		}
		if (mSelectRect.l == INFINITY) {
			mSelectRect = Box();
		}
	}

	/*

	void CellsWidget::connectList() {
		connect(gCellList, &QWidget::customContextMenuRequested, [=](const QPoint& pos) {
			if (isVisible()) {
				QMenu contextMenu(tr("Context menu"), this);
				auto CREATE_ACT = contextMenu.addAction("Create");
				auto DELETE_ACT = contextMenu.addAction("Delete");

				auto selected = contextMenu.exec(gCellList->mapToGlobal(pos));
				if (selected) {
					if (selected == CREATE_ACT) {

					} else if (selected == DELETE_ACT) {

					}
				}
			}
			});

		connect(gCellList, &QListExtension::itemSelectionChanged, [&]() {
			if (isVisible() && !mSuppressSelect) {
				mSelects.clear();
				for (uint i = 0; i < gCells.count(); i++) {
					auto& data = gCells[i];
					if (data.item->isSelected()) {
						mSelects.push_back(&data);
					}
				}
				highlightSelected();
				ui.view->update();
				//setFocus();
			}
		});

		connect(gCellList->itemDelegate(), &QAbstractItemDelegate::commitData, [&](QWidget* pLineEdit) {
			if (isVisible()) {
				auto new_name = reinterpret_cast<QLineEdit*>(pLineEdit)->text();
				for (uint i = 0; i < gCellList->count(); i++) {
					if (gCellList->currentRow() != i && gCellList->item(i)->text() == new_name) {
						new_name.push_back('_');
						i = 0;
					}
				}

				gCellList->item(gCellList->currentRow())->setText(new_name);
				auto& data = gCells[gCellList->currentRow()];
				data.name = new_name.toStdString();
				//setFocus();
			}
			});

		connect(gCellList->model(), &QAbstractItemModel::rowsMoved, [&](const QModelIndex&, int from, int, const QModelIndex&, int to) {
			if (isVisible()) {
				if (from < to)
					to--;
				if (uint(to) >= gCellList->count())
					to = gCellList->count() - 1;
				if (from < 0)
					from = 0;
				gCells.swap(from, to);
			}
		});
	}

	void CellsWidget::combineCells() {
		if (mSelects.size() > 1) {
			int l = std::numeric_limits<int>::max();
			int b = std::numeric_limits<int>::max();
			int r = std::numeric_limits<int>::min();
			int t = std::numeric_limits<int>::min();
			mSuppressSelect = true;
			for (uint i = 0; i < mSelects.size(); i++) {
				auto& temp = *mSelects[i];
				l = min(l, temp.x);
				b = min(b, temp.y);
				r = max(r, (temp.x + temp.w));
				t = max(t, (temp.y + temp.h));
				if (i == 0) {
					temp.rect = mSelectRect;
				} else {
					delete temp.item;
					temp.item = nullptr;
				}
			} mSelects.erase(mSelects.begin() + 1, mSelects.end());
			mSelects[0]->setAbsData();
			mSelects[0]->ox = 0;
			mSelects[0]->oy = 0;

			for (uint i = 0; i < gCells.count(); i++) {
				if (gCells[i].item == 0) {
					gCells.erase(i);
					i--;
				}
			}

			mSelectRect = aabb();
			mSuppressSelect = false;
			update();
		}
	}

	void CellsWidget::deleteAll() {
		mSuppressSelect = true;
		for (uint i = 0; i < gCells.count(); i++) {
			delete gCells[i].item;
		} gCells.clear();
		mSelects.clear();

		mSelectRect = aabb();
		mSuppressSelect = false;
	}

	void CellsWidget::importAtlasBegin() {
		deleteAll();
	}

	void CellsWidget::importAtlasEnd() {
		recreateList();
		update();
	}

	void CellsWidget::deleteSelected() {
		mSuppressSelect = true;
		for (uint i = 0; i < mSelects.size(); i++) {
			delete mSelects[i]->item;
			mSelects[i]->item = 0;
		} mSelects.clear();

		for (uint i = 0; i < gCells.count(); i++) {
			if (gCells[i].item == 0) {
				gCells.erase(i);
				i--;
			}
		}

		mSelectRect = aabb();
		mSuppressSelect = false;

		recreateList();
		ui.view->update();
	}

	void CellsWidget::highlightSelected() {
		mSelectRect = aabb(INFINITY, INFINITY, -INFINITY, -INFINITY);
		for (uint i = 0; i < gCells.count(); i++) {
			auto& sel = gCells[i];
			if (sel.item->isSelected()) {
				sel.color = color(0, 255, 255, 255);
				mSelectRect.l = min(mSelectRect.l, sel.rect.l);
				mSelectRect.b = min(mSelectRect.b, sel.rect.b);
				mSelectRect.r = max(mSelectRect.r, sel.rect.r);
				mSelectRect.t = max(mSelectRect.t, sel.rect.t);
			} else sel.color = color(0, 255, 0, 255);
		}
		if (mSelectRect.l == INFINITY) {
			mSelectRect = aabb();
		}
	}
	/**/

	void CellsWidget::onTextureUpdate() {
		QElangPaletteWidget::onTextureUpdate();
		recreateList();
	}

	void CellsWidget::recreateList() {
		assert(gGUI.open());

		gAtlsUtil.cellList->clear();
		gAtlsUtil.cellItems.clear();
		mSelects.clear();
		if (mTexture && mTexture->atlas) {
			mSuppressSelect = true;
			for (auto it : mTexture->atlas->cells) {
				QListWidgetItem* item = new QListWidgetItem(gAtlsUtil.cellList);
				item->setText(QString::fromUtf8(it.first));
				gAtlsUtil.cellList->addItem(item);
				gAtlsUtil.cellItems.emplace(it.second, item);
			} mSuppressSelect = false;
		}
	}

	//void CellsWidget::showEditor() {
	//	gCellList->setSelectionMode(QAbstractItemView::SelectionMode::ExtendedSelection);
	//	gCellList->setDragDropMode(QAbstractItemView::DragDropMode::DragDrop);
	//	gCellList->setDefaultDropAction(Qt::DropAction::MoveAction);
	//	gCellList->show();
	//	show();

	//	mScene.makeCurrent();
	//	if (gCellList->count() > 0) {
	//		gCellRow = clamp(gCellRow, 0, gCellList->count() - 1);
	//	} else gCellRow = -1;
	//	gCellList->setCurrentRow(gCellRow);
	//}

	//void CellsWidget::hideEditor() {
	//	if (mSelects.size() > 0)
	//		gCellRow = gCellList->row(mSelects[0]->item);
	//	mSelects.clear();
	//	
	//	gCellList->clearSelection();
	//	gCellList->setCurrentRow(-1);
	//	gCellList->hide();
	//	hide();
	//}

	//void CellsWidget::findCursorState(vec2 pos) {
	//	if (mState == SNONE) {
	//		mCursorState = CursorState::NONE;
	//		if (mSelectRect.contains(pos)) {
	//			auto sx = mViewport->scale().x;

	//			if (mSelectRect.l > pos.x - 5.0f * sx)
	//				mCursorState += CursorState::LEFT;
	//			else if (mSelectRect.r < pos.x + 5.0f * sx)
	//				mCursorState += CursorState::RIGHT;

	//			if (mSelectRect.b > pos.y - 5.0f * sx)
	//				mCursorState += CursorState::BOTTOM;
	//			else if (mSelectRect.t < pos.y + 5.0f * sx)
	//				mCursorState += CursorState::TOP;

	//			if (mCursorState == 0)
	//				mCursorState = CursorState::MOVE;
	//		}

	//		if (QApplication::queryKeyboardModifiers() & Qt::AltModifier) {
	//			mTempCursor = Qt::ArrowCursor;
	//			if (mCursorState == CursorState::MOVE)
	//				mTempCursor = Qt::OpenHandCursor;

	//			bool left = ((mCursorState & CursorState::LEFT) == CursorState::LEFT);
	//			bool right = ((mCursorState & CursorState::RIGHT) == CursorState::RIGHT);
	//			bool top = ((mCursorState & CursorState::TOP) == CursorState::TOP);
	//			bool bottom = ((mCursorState & CursorState::BOTTOM) == CursorState::BOTTOM);

	//			if (left || right) {
	//				mTempCursor = Qt::SizeHorCursor;
	//			}
	//			if (top || bottom) {
	//				if (mTempCursor == Qt::SizeHorCursor) {
	//					if ((left && top) || (right && bottom)) {
	//						mTempCursor = Qt::SizeFDiagCursor;
	//					} else mTempCursor = Qt::SizeBDiagCursor;
	//				} else
	//					mTempCursor = Qt::SizeVerCursor;
	//			}
	//		} else mTempCursor = Qt::ArrowCursor;
	//	}	
	//	
	//	if (mState == MOVING) {
	//		mTempCursor = Qt::ClosedHandCursor;
	//	} 
	//	
	//	setCursor(mTempCursor);
	//}

	///////////////////////////////////////////////////////////////////////////////////////////////

}