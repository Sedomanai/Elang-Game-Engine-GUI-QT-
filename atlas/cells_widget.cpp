#include "cells_widget.h"
#include <qmenu.h>
#include <qlineedit.h>


namespace el
{
	CellsWidget::CellsWidget(QWidget* parent) : 
		mState(CellsWidget::SNONE), QElangTextureWidget(parent), mSuppressSelect(false), mAlphaCut(10)
	{
		ui.view->setMouseTracking(true);

		ui.view->sig_Start.connect([&]() {
			//connectMouseInput();
		});

		ui.view->sig_Paint.connect([&]() {
			//for (uint i = 0; i < gCells.count(); i++) {
			//	auto& data = gCells[i];
			//	color fill = data.color;
			//	aabb frect = data.rect;
			//	mScene.debug.line.batchAABB(frect, fill, 0);
			//	mScene.debug.fill.batchAABB(frect, fill, E_DANGLING_VERTICES);
			//};
		});

		//connect(ui.view, &QIlangView::sig_PaintGL, [&]() {
		//	

		//	switch (mState) {
		//	case SELECTING:
		//		mScene.debug.line.batchAABB(mHighlight, color(255, 255, 255, 255), E_DANGLING_VERTICES);
		//		break;
		//	}

		//	mScene.debug.line.batchAABB(mSelectRect, color(255, 255, 255, 255), E_DANGLING_VERTICES);
		//	mScene.debug.draw();
		//	});

		//connectList();
	}

	CellsWidget::~CellsWidget() {}


	//void CellsWidget::connectMouseInput() {
	//	connect(ui.view, &QIlangView::sig_MousePress, [&]() {
	//		if (gMouse.state(0) == eInput::ONCE) {
	//			auto cell = mScene.mousehub.data().select[0];
	//			auto pos = *mViewport * gMouse.currentPosition();

	//			findCursorState(pos);
	//			if (QApplication::keyboardModifiers() & Qt::AltModifier) {
	//				if (mCursorState == MOVE)
	//					mState = MOVING;
	//				else if (mCursorState > MOVE) {
	//					mState = SIZING;
	//				}
	//			} else {
	//				mState = SELECTING;
	//				mHighlight.l = pos.x;
	//				mHighlight.r = pos.x;
	//				mHighlight.t = pos.y;
	//				mHighlight.b = pos.y;
	//			}

	//			ui.view->update();
	//		}
	//		});
	//	connect(ui.view, &QIlangView::sig_MouseRelease, [&]() {
	//		if (gMouse.state(0) == eInput::LIFT) {
	//			switch (mState) {
	//			case SIZING:
	//				mSelectRect.roundCorners();
	//				{
	//					auto& sel = mSelects[0];
	//					sel->rect = mSelectRect;
	//					sel->setAbsData();
	//				}
	//				break;
	//			case MOVING:
	//				for (uint i = 0; i < mSelects.size(); i++) {
	//					auto& sel = mSelects[i];
	//					sel->rect.roundCorners();
	//					sel->setAbsData();
	//				}
	//				break;
	//			case SELECTING:
	//				mSuppressSelect = true;
	//				mSelects.clear();
	//				gCellList->clearSelection();
	//				mHighlight.normalize();
	//				for (uint i = 0; i < gCells.count(); i++) {
	//					if (gCells[i].rect.intersects(mHighlight)) {
	//						mSelects.push_back(&gCells[i]);
	//						gCells[i].item->setSelected(true);
	//					}
	//				}

	//				mSuppressSelect = false;
	//				break;
	//			}

	//			highlightSelected();
	//			mState = SNONE;

	//			ui.view->update();
	//		}
	//		});
	//	connect(ui.view, &QIlangView::sig_MouseMove, [&]() {
	//		auto pos = *mViewport * gMouse.currentPosition();
	//		findCursorState(pos);

	//		if (gMouse.state(0) == eInput::HOLD) {
	//			auto delta = *mViewport * gMouse.deltaPosition();
	//			delta -= mViewport->position();

	//			switch (mState) {
	//			case SELECTING:
	//				mHighlight.r = pos.x;
	//				mHighlight.t = pos.y;
	//				ui.view->update();
	//				break;

	//			case MOVING:
	//				mSelectRect.l += delta.x;
	//				mSelectRect.r += delta.x;
	//				mSelectRect.t += delta.y;
	//				mSelectRect.b += delta.y;

	//				for (uint i = 0; i < mSelects.size(); i++) {
	//					auto& rect = mSelects[i];
	//					rect->rect.l += delta.x;
	//					rect->rect.r += delta.x;
	//					rect->rect.t += delta.y;
	//					rect->rect.b += delta.y;
	//				}
	//				ui.view->update();
	//				break;

	//			case SIZING:
	//				if (mSelects.size() == 1) {
	//					auto& rect = mSelects[0];

	//					if ((mCursorState & LEFT) == LEFT) {
	//						mSelectRect.l = rect->rect.l = pos.x;
	//					} else if ((mCursorState & RIGHT) == RIGHT) {
	//						mSelectRect.r = rect->rect.r = pos.x;
	//					}
	//					if ((mCursorState & BOTTOM) == BOTTOM) {
	//						mSelectRect.b = rect->rect.b = pos.y;
	//					} else if ((mCursorState & TOP) == TOP) {
	//						mSelectRect.t = rect->rect.t = pos.y;
	//					}
	//					ui.view->update();
	//				}
	//				break;
	//			}
	//		}
	//		});
	//}

	//void CellsWidget::connectList() {
	//	connect(gCellList, &QWidget::customContextMenuRequested, [=](const QPoint& pos) {
	//		if (isVisible()) {
	//			QMenu contextMenu(tr("Context menu"), this);
	//			auto CREATE_ACT = contextMenu.addAction("Create");
	//			auto DELETE_ACT = contextMenu.addAction("Delete");

	//			auto selected = contextMenu.exec(gCellList->mapToGlobal(pos));
	//			if (selected) {
	//				if (selected == CREATE_ACT) {

	//				} else if (selected == DELETE_ACT) {

	//				}
	//			}
	//		}
	//		});

	//	connect(gCellList, &QListExtension::itemSelectionChanged, [&]() {
	//		if (isVisible() && !mSuppressSelect) {
	//			mSelects.clear();
	//			for (uint i = 0; i < gCells.count(); i++) {
	//				auto& data = gCells[i];
	//				if (data.item->isSelected()) {
	//					mSelects.push_back(&data);
	//				}
	//			}
	//			highlightSelected();
	//			ui.view->update();
	//			//setFocus();
	//		}
	//	});

	//	connect(gCellList->itemDelegate(), &QAbstractItemDelegate::commitData, [&](QWidget* pLineEdit) {
	//		if (isVisible()) {
	//			auto new_name = reinterpret_cast<QLineEdit*>(pLineEdit)->text();
	//			for (uint i = 0; i < gCellList->count(); i++) {
	//				if (gCellList->currentRow() != i && gCellList->item(i)->text() == new_name) {
	//					new_name.push_back('_');
	//					i = 0;
	//				}
	//			}

	//			gCellList->item(gCellList->currentRow())->setText(new_name);
	//			auto& data = gCells[gCellList->currentRow()];
	//			data.name = new_name.toStdString();
	//			//setFocus();
	//		}
	//		});

	//	connect(gCellList->model(), &QAbstractItemModel::rowsMoved, [&](const QModelIndex&, int from, int, const QModelIndex&, int to) {
	//		if (isVisible()) {
	//			if (from < to)
	//				to--;
	//			if (uint(to) >= gCellList->count())
	//				to = gCellList->count() - 1;
	//			if (from < 0)
	//				from = 0;
	//			gCells.swap(from, to);
	//		}
	//	});
	//}

	//void CellsWidget::autoGenCells(uint sortorder, uint target_margin) {
	//	static vector<EditorCell> tempcells;
	//	auto tex = gTextures.find(mTexKey);
	//	if (tex) {
	//		for (auto button : mScene.objects.view<EditorCell>()) {
	//			mScene.objects.destroy(button);
	//			gCells.clear();
	//		}

	//		uint pixcount = mTexWidth * mTexHeight;
	//		auto pixels = (unsigned char*)malloc(pixcount);
	//		glActiveTexture(GL_TEXTURE0);
	//		glBindTexture(GL_TEXTURE_2D, tex->id);
	//		glPixelStorei(GL_PACK_ALIGNMENT, 1);
	//		glGetTexImage(GL_TEXTURE_2D, 0, GL_ALPHA, GL_UNSIGNED_BYTE, pixels);

	//		uint name = 0;
	//		uint* buffer = (uint*)malloc((pixcount) * sizeof(uint));
	//		hashmap<uint, vector<uint>> result;

	//		tempcells.clear();
	//		for (uint i = 0; i < pixcount; i++) {
	//			bool notop = ((i < mTexWidth) || (buffer[i - mTexWidth] == 0));
	//			bool noleft = ((i % mTexWidth == 0) || (buffer[i - 1] == 0));
	//			bool notopleft = ((i < mTexWidth) || (i % mTexWidth == 0) || (buffer[i - mTexWidth - 1] == 0));
	//			bool notopright = ((i < mTexWidth) || (i % mTexWidth == mTexWidth - 1) || (buffer[i - mTexWidth + 1] == 0));

	//			buffer[i] = 0;

	//			if (pixels[i] > mAlphaCut) {
	//				if (noleft) {
	//					if (notop) {
	//						if (notopleft) {
	//							buffer[i] = ++name;
	//							result[name].reserve(256);
	//							result[name].push_back(i);
	//						} else {
	//							// only topleft
	//							buffer[i] = buffer[i - mTexWidth - 1];
	//							result[buffer[i]].push_back(i);
	//						}
	//					} else { 
	//						// top (topleft should it exist would already be the same as top)
	//						buffer[i] = buffer[i - mTexWidth];
	//						result[buffer[i]].push_back(i);
	//					}
	//				} else {
	//					if (notop) {
	//						// left (topleft should it exist would already be the same as left)
	//						buffer[i] = buffer[i - 1];
	//						result[buffer[i]].push_back(i);
	//					} else {
	//						if (notopleft) {
	//							// only left and above
	//							auto top = buffer[i - mTexWidth];
	//							auto left = buffer[i - 1];

	//							if (top != left) {
	//								auto& tops = result[top];
	//								auto& lefts = result[left];
	//								for (uint i = 0; i < tops.size(); i++) {
	//									buffer[tops[i]] = left;
	//								}
	//								lefts.reserve(lefts.size() + tops.size());
	//								lefts.insert(lefts.end(), tops.begin(), tops.end());
	//								result.erase(top);
	//							}

	//							buffer[i] = left;
	//							result[left].push_back(i);
	//						} else {
	//							// all three topleft pixels are the same, could use any
	//							buffer[i] = buffer[i - 1];
	//							result[buffer[i]].push_back(i);
	//						}
	//					}
	//				}

	//				if (notop && !notopright) {
	//					// special case where topright exists but top doesn't
	//					auto topright = buffer[i - mTexWidth + 1];
	//					auto curr = buffer[i];
	//					if (curr == 0) {
	//						buffer[i] = topright;
	//						result[topright].push_back(i);
	//					} else if (curr != topright) {
	//						auto& toprights = result[topright];
	//						auto& currs = result[curr];
	//						for (uint i = 0; i < currs.size(); i++) {
	//							buffer[currs[i]] = topright;
	//						}
	//						toprights.reserve(toprights.size() + currs.size());
	//						toprights.insert(toprights.end(), currs.begin(), currs.end());
	//						result.erase(curr);
	//					}
	//				}
	//			}
	//		} 

	//		deleteAll();
	//		string strname;
	//		tokenize(mTexKey, '/', [&](strview, strview tail) {
	//			tokenize(tail, '.', [&](strview head, strview) {
	//				strname = head;
	//				});
	//			});

	//		auto intmax = std::numeric_limits<int>::max();
	//		for (auto& vec : result) {
	//			int il = intmax;
	//			int ib = intmax;
	//			int ir = -intmax;
	//			int it = -intmax;
	//			for (int s : vec.second) {
	//				il = min(il, (s % mTexWidth));
	//				ib = min(ib, (s / mTexWidth));
	//				ir = max(ir, (s % mTexWidth) + 1);
	//				it = max(it, (s / mTexWidth) + 1);
	//			}

	//			tempcells.emplace_back(aabb(il, -ib, ir, -it), strname);
	//			tempcells.back().setAbsData();
	//			tempcells.back().rect.normalize();
	//		}

	//		std::sort(tempcells.begin(), tempcells.end(), [&](EditorCell& l, EditorCell& r) -> bool {
	//			bool ret = false;
	//			float margin;
	//			if (sortorder == 0) {
	//				margin = abs(l.rect.t - r.rect.t);
	//				ret = (margin < target_margin) ? (l.rect.l < r.rect.l) : (l.rect.t > r.rect.t);
	//			} else {
	//				margin = abs(r.rect.l - l.rect.l);
	//				ret = (margin < target_margin) ? (l.rect.t > r.rect.t) : (l.rect.l < r.rect.l);
	//			}
	//			return ret;
	//		});
	//		
	//		for (uint i = 0; i < tempcells.size(); i++) {
	//			tempcells[i].name += "_" + std::to_string(i);
	//			gCells.emplace(tempcells[i].name, tempcells[i]);
	//		}

	//		free(pixels);
	//		free(buffer);

	//		recreateList();
	//		update();
	//	}
	//}

	//TODO: Count corners into consideration
	//void CellsWidget::autoCreateCell() {
	//	static vector<int> valids;

	//	auto tex = gTextures.find(mTexKey);
	//	if (tex) {
	//		uint pixcount = mTexWidth * mTexHeight;
	//		glActiveTexture(GL_TEXTURE0);
	//		glBindTexture(GL_TEXTURE_2D, tex->id);

	//		auto pixels = (unsigned char*)malloc(pixcount);
	//		glPixelStorei(GL_PACK_ALIGNMENT, 1);
	//		glGetTexImage(GL_TEXTURE_2D, 0, GL_ALPHA, GL_UNSIGNED_BYTE, pixels);

	//		bool* visited = (bool*)calloc(pixcount, sizeof(bool));

	//		//TODO: change resolution
	//		uint reader = 0, rend = 1;
	//		vec2 mousepix = (*mViewport) * gMouse.currentPosition();
	//		mousepix.x = round(mousepix.x);
	//		mousepix.y = round(mousepix.y);
	//		mousepix.y *= -1;
	//		if (0 < mousepix.x &&
	//			0 < mousepix.y &&
	//			mousepix.x < mTexWidth &&
	//			mousepix.y < mTexHeight) {
	//			uint start = mousepix.x + mousepix.y * mTexWidth;
	//			if (pixels[start] > mAlphaCut) {
	//				valids.clear();
	//				valids.push_back(start);
	//				visited[start] = true;

	//				while (true) {
	//					rend = valids.size();
	//					for (; reader < rend; reader++) {
	//						auto curr = valids[reader];
	//						bool notop =
	//							((curr < mTexWidth) || (pixels[curr - mTexWidth] <= mAlphaCut) || visited[curr - mTexWidth] == true);
	//						bool nobottom =
	//							(curr > (mTexWidth * (mTexHeight - 1)) || (pixels[curr + mTexWidth] <= mAlphaCut) || (visited[curr + mTexWidth] == true));
	//						bool noleft =
	//							((curr % mTexWidth == 0) || (pixels[curr - 1] <= mAlphaCut) || (visited[curr - 1] == true));
	//						bool noright =
	//							((curr % mTexWidth == (mTexWidth - 1)) || (pixels[curr + 1] <= mAlphaCut) || (visited[curr + 1] == true));
	//						/*bool notopleft =
	//							((curr < mTexWidth) || (pixels[curr - mTexWidth] <= 25) || visited[curr - mTexWidth] == true);
	//						bool notopright =
	//							(curr > (mTexWidth * (mTexHeight - 1)) || (pixels[curr + mTexWidth] <= 25) || (visited[curr + mTexWidth] == true));
	//						bool nobottomleft =
	//							((curr % mTexWidth == 0) || (pixels[curr - 1] <= 25) || (visited[curr - 1] == true));
	//						bool nobottomright =
	//							((curr % mTexWidth == (mTexWidth - 1)) || (pixels[curr + 1] <= 25) || (visited[curr + 1] == true));*/

	//						if (!notop) {
	//							valids.push_back(curr - mTexWidth);
	//							visited[curr - mTexWidth] = true;
	//						} if (!nobottom) {
	//							valids.push_back(curr + mTexWidth);
	//							visited[curr + mTexWidth] = true;
	//						} if (!noleft) {
	//							valids.push_back(curr - 1);
	//							visited[curr - 1] = true;
	//						} if (!noright) {
	//							valids.push_back(curr + 1);
	//							visited[curr + 1] = true;
	//						}
	//					}

	//					if (rend == valids.size())
	//						break;
	//				}


	//				string strname;
	//				tokenize(mTexKey, '.', [&](strview name, strview) {
	//					tokenize(name, '/', [&](strview head, strview) {
	//						strname = head;
	//						});
	//					});

	//				strname.push_back('_');

	//				uint targetindex = gCellList->count();
	//				string targetname = strname + std::to_string(targetindex);

	//				for (uint i = 0; i < gCellList->count(); i++) {
	//					if (gCellList->item(i)->text() == QString::fromStdString(targetname)) {
	//						targetindex++;
	//						targetname = strname + std::to_string(targetindex);
	//					}
	//				}

	//				auto intmax = std::numeric_limits<int>::max();
	//				int il = intmax;
	//				int ib = intmax;
	//				int ir = -intmax;
	//				int it = -intmax;

	//				for (uint i = 0; i < valids.size(); i++) {
	//					auto& s = valids[i];
	//					il = min(il, (s % mTexWidth));
	//					ib = min(ib, (s / mTexWidth));
	//					ir = max(ir, (s % mTexWidth) + 1);
	//					it = max(it, (s / mTexWidth) + 1);
	//				}

	//				gCells.emplace(targetname, aabb(il, -ib, ir, -it), targetname);
	//				gCells.back().setAbsData();
	//				gCells.back().rect.normalize();
	//			}
	//		}
	//		free(pixels);
	//		recreateList();
	//		ui.view->update();
	//	}
	//}

	//void CellsWidget::combineCells() {
	//	if (mSelects.size() > 1) {
	//		int l = std::numeric_limits<int>::max();
	//		int b = std::numeric_limits<int>::max();
	//		int r = std::numeric_limits<int>::min();
	//		int t = std::numeric_limits<int>::min();
	//		mSuppressSelect = true;
	//		for (uint i = 0; i < mSelects.size(); i++) {
	//			auto& temp = *mSelects[i];
	//			l = min(l, temp.x);
	//			b = min(b, temp.y);
	//			r = max(r, (temp.x + temp.w));
	//			t = max(t, (temp.y + temp.h));
	//			if (i == 0) {
	//				temp.rect = mSelectRect;
	//			} else {
	//				delete temp.item;
	//				temp.item = nullptr;
	//			}
	//		} mSelects.erase(mSelects.begin() + 1, mSelects.end());
	//		mSelects[0]->setAbsData();
	//		mSelects[0]->ox = 0;
	//		mSelects[0]->oy = 0;

	//		for (uint i = 0; i < gCells.count(); i++) {
	//			if (gCells[i].item == 0) {
	//				gCells.erase(i);
	//				i--;
	//			}
	//		}

	//		mSelectRect = aabb();
	//		mSuppressSelect = false;
	//		update();
	//	}
	//}

	//void CellsWidget::deleteAll() {
	//	mSuppressSelect = true;
	//	for (uint i = 0; i < gCells.count(); i++) {
	//		delete gCells[i].item;
	//	} gCells.clear();
	//	mSelects.clear();

	//	mSelectRect = aabb();
	//	mSuppressSelect = false;
	//}

	//void CellsWidget::importAtlasBegin() {
	//	deleteAll();
	//}

	//void CellsWidget::importAtlasEnd() {
	//	recreateList();
	//	update();
	//}

	//void CellsWidget::deleteSelected() {
	//	mSuppressSelect = true;
	//	for (uint i = 0; i < mSelects.size(); i++) {
	//		delete mSelects[i]->item;
	//		mSelects[i]->item = 0;
	//	} mSelects.clear();

	//	for (uint i = 0; i < gCells.count(); i++) {
	//		if (gCells[i].item == 0) {
	//			gCells.erase(i);
	//			i--;
	//		}
	//	}

	//	mSelectRect = aabb();
	//	mSuppressSelect = false;

	//	recreateList();
	//	ui.view->update();
	//}

	//void CellsWidget::highlightSelected() {
	//	mSelectRect = aabb(INFINITY, INFINITY, -INFINITY, -INFINITY);
	//	for (uint i = 0; i < gCells.count(); i++) {
	//		auto& sel = gCells[i];
	//		if (sel.item->isSelected()) {
	//			sel.color = color(0, 255, 255, 255);
	//			mSelectRect.l = min(mSelectRect.l, sel.rect.l);
	//			mSelectRect.b = min(mSelectRect.b, sel.rect.b);
	//			mSelectRect.r = max(mSelectRect.r, sel.rect.r);
	//			mSelectRect.t = max(mSelectRect.t, sel.rect.t);
	//		} else sel.color = color(0, 255, 0, 255);
	//	}
	//	if (mSelectRect.l == INFINITY) {
	//		mSelectRect = aabb();
	//	}
	//}

	//void CellsWidget::recreateList() {
	//	mSuppressSelect = true;
	//	gCellList->clear();
	//	for (uint i = 0; i < gCells.count(); i++) {
	//		auto& cell = gCells[i];
	//		cell.item = new QListWidgetItem(QString::fromStdString(cell.name));
	//		cell.item->setFlags(cell.item->flags() | Qt::ItemIsEditable);
	//		gCellList->addItem(cell.item);
	//	}
	//	mSuppressSelect = false;
	//}

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