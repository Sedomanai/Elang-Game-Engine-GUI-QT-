#include <elqtpch.h>
#include "cells_widget.h"

#include <tools/cell.h>
#include <tools/texture.h>
#include <tools/atlas.h>
#include <tools/controls.h>
#include <tools/painter.h>
#include <tools/material.h>
#include <common/algorithm.h>
#include <apparatus/ui.h>
#include "../elqt/color_code.h"

namespace el
{
	CellsWidget::CellsWidget(QWidget* parent) :
		mState(CellsWidget::eSNone), QElangPaletteWidget(parent), mSuppressSelect(false), mAlphaCut(10) {
		ui.view->sig_Start.connect([&]() {
			ui.view->makeCurrent();
			connectMouseInput();
			});

		ui.view->sig_Paint.connect([&]() {
			if (mAtlas && mAtlas.has<AssetLoaded>()) {
				color8 c = selectColoring();
				auto& cells = mAtlas.get<AtlasMeta>().cellorder;
				for (asset<AtlasSelectedCell> selected : cells) {
					if (selected) {
						auto& holder = selected.get<CellHolder>();
						c.a = 255;
						mHighlighter->line.batchAABB(holder.rect, c, 0.0f);
						c.a = gEditorColor.cellFillAlpha;
						mHighlighter->fill.batchAABB(holder.rect, c, 0.0f);
					}
				}

				switch (mState) {
					case eSelecting:
						if (QApplication::queryKeyboardModifiers() & Qt::ControlModifier)
							mHighlighter->line.batchAABB(mSelectRect, gEditorColor.createRect, -10.0f);
						else
							mHighlighter->line.batchAABB(mSelectRect, gEditorColor.selectRect, -10.0f);
						break;
				}
				mHighlighter->draw();
			}
			});

		connectList();
	}

	void CellsWidget::onKeyPress(QKeyEvent* e) {
		if (e->key() == Qt::Key::Key_Alt) {
			findCursorState();
			ui.view->update();
		}
		if (e->key() == Qt::Key::Key_Space) {
			findCursorState();
			ui.view->update();
		}
	}

	void CellsWidget::onKeyRelease(QKeyEvent* e) {
		if (e->key() == Qt::Key::Key_Shift) {
			findCursorState();
			ui.view->update();
		}
		if (e->key() == Qt::Key::Key_Control) {
			findCursorState();
			ui.view->update();
		}
	}

	color8 CellsWidget::selectColoring() {
		if (mTempCursor == Qt::ArrowCursor || mTempCursor == Qt::SizeAllCursor)
			return gEditorColor.cellHovered;
		if (mTempCursor == Qt::OpenHandCursor)
			return gEditorColor.cellOpenHanded;
		else if (mTempCursor == Qt::ClosedHandCursor)
			return gEditorColor.cellShadow;
		else {
			if (mState == eSNone)
				return gEditorColor.cellSizing;
			else
				return gEditorColor.cellShadow;
		}
	}

	void CellsWidget::autoNewGenAtlas(asset<Atlas> atlas, uint sortorder, uint target_margin) {
		if (mMaterial && mMaterial->hasTexture() && atlas && atlas.has<AtlasMeta>() && atlas.has<AssetData>()) {
			mMaterial->textures[0]->autoGenerateAtlas(atlas, mAlphaCut);
			mAtlas = atlas;
			sortAtlasOnNewGen(sortorder, target_margin);
			updateAtlas(atlas);
			renameAll();
		}
	}

	void CellsWidget::renameAll() {
		auto& cells = *gAtlsUtil.cellList;
		mAtlas->cells.clear();

		auto& meta = mAtlas.get<AtlasMeta>();
		meta.cellnames.clear();

		auto name = mAtlas.get<AssetData>().filePath.stem().generic_u8string();

		auto& order = meta.cellorder;

		for (auto i = 0; i < order.size(); i++) {
			auto rname = name + "_" + std::to_string(i);

			auto data = asset<SubAssetData>(meta.cellorder[i]);
			data->name = rname;

			auto& item = data.get<CellItem*>();
			item->setText(QString::fromUtf8(rname));

			mAtlas->cells.emplace(rname, data);
			meta.cellnames.emplace(data, name);
		}
	}

	void CellsWidget::sortAtlasOnNewGen(uint sortorder, uint target_margin) {
		assert(mAtlas);
		assert(mAtlas.has<AtlasMeta>());

		auto& meta = mAtlas.get<AtlasMeta>();
		auto& order = meta.cellorder;

		std::sort(order.begin(), order.end(), [&](asset<Cell> lhs, asset<Cell> rhs) ->bool {
			bool ret = false;
			float margin;

			auto lverti = lhs->uvUp;
			auto rverti = rhs->uvUp;
			auto lhori = lhs->uvLeft;
			auto rhori = rhs->uvLeft;

			if (sortorder == 0) {
				margin = abs(lverti - rverti) * meta.height;
				ret = (margin < target_margin) ? (lhori < rhori) : (lverti < rverti);
			} else {
				//margin = abs(lhori - rhori) * meta.width;
				//ret = (margin < target_margin) ? (lhori < rhori) : (lverti > rverti);
			}
			return ret;
			});

		for (sizet i = 0; i < order.size(); i++)
			asset<SubAssetData>(order[i])->index = i;
	}

	void CellsWidget::reorderCellsAccordingToList() {
		if (mAtlas) {
			auto& meta = mAtlas.get<AtlasMeta>();
			meta.cellorder.clear();

			auto& list = gAtlsUtil.cellList;
			for (auto i = 0; i < list->count(); i++) {
				auto item = reinterpret_cast<CellItem*>(list->item(i));
				meta.cellorder.emplace_back(item->holder);
				item->holder.get<SubAssetData>().index = i;
			}
		}
	}

	void CellsWidget::updateAtlas(asset<Atlas> atlas) {
		mAtlas = atlas;
		safeClearSelection();
		recreateCellHoldersFromAtlas();
		recreateList();
		rebatchAllCellHolders();
		ui.view->update();
	}

	void CellsWidget::loop() {
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
				onMouseMove();
				ui.view->update();
			}
		}
	}

	void CellsWidget::safeClearSelection() {
		mSuppressSelect = true;
		gAtlsUtil.cellList->clearSelection();
		gAtlsUtil.cellList->setCurrentItem(0);
		gProject.clear<AtlasSelectedCell>();
		mSuppressSelect = false;
	}

	void CellsWidget::connectMouseInput() {
		ui.view->sig_MousePress.connect([&]() {
			findCursorState();
			mCamPivot = mMainCam->position();
			if (mAtlas && gMouse.state(0) == eInput::Once) {
				if (QApplication::keyboardModifiers() & Qt::AltModifier) {
					if (mCursorState == eMove) {
						mState = eMoving;
						gProject.clear<AtlasMovingCell>();
						for (asset<CellHolder> holder : gProject.view<AtlasSelectedCell>()) {
							assert(holder);
							gProject.emplace<AtlasMovingCell>(holder, holder->rect);
							mGrabPos = *mMainCam * gMouse.currentPosition();;
						}
					} else if (mCursorState > eMove) {
						mState = eSizing;
					}
				} else {
					auto pos = *mMainCam * gMouse.currentPosition();
					mState = eSelecting;
					mSelectRect.l = pos.x;
					mSelectRect.r = pos.x;
					mSelectRect.t = pos.y;
					mSelectRect.b = pos.y;
				}

				ui.view->update();
			}
			findCursorState();
			});

		ui.view->sig_MouseMove.connect(this, &CellsWidget::onMouseMove);

		ui.view->sig_MouseRelease.connect([&]() {
			findCursorState();
			if (mAtlas && gMouse.state(0) == eInput::Lift) {
				auto selected = gProject.view<AtlasSelectedCell>();
				auto& meta = mAtlas.get<AtlasMeta>();

				switch (mState) {
					case eSizing:
						if (selected.size() == 1) {
							for (asset<CellHolder> holder : selected) {
								assert(holder);
								holder->rect.normalize();
								holder->rect.roundCorners();
								holder->moldCellFromRect(holder, (int)meta.width, (int)meta.height);
								rebatchAllCellHolders();
								sig_Modified.invoke();
							}
						}
						break;
					case eMoving:
						assert(selected.size() > 0);
						for (asset<CellHolder> holder : selected) {
							assert(holder && holder.has<AtlasMovingCell>());
							holder->rect.roundCorners();
							holder->moldCellFromRect(holder, (int)meta.width, (int)meta.height);
						} rebatchAllCellHolders();
						gProject.clear<AtlasMovingCell>();
						sig_Modified.invoke();
						break;
					case eSelecting:
						if (QApplication::queryKeyboardModifiers() & Qt::ControlModifier) {
							mSelectRect.normalize();
							mSelectRect.roundCorners();
							createNamedCell();
							rebatchAllCellHolders();
							sig_Modified.invoke();
						} else {
							mSuppressSelect = true;
							gAtlsUtil.cellList->clearSelection();
							mSelectRect.normalize();

							gProject.clear<AtlasSelectedCell>();
							auto& meta = mAtlas.get<AtlasMeta>();
							for (asset<CellHolder> holder : meta.cellorder) {
								if (holder->rect.intersects(mSelectRect)) {
									gProject.get_or_emplace<AtlasSelectedCell>(holder);
									assert(holder.has<CellItem*>());
									holder.get<CellItem*>()->setSelected(true);
								}
							}

							if (selected.size() > 0) {
								gAtlsUtil.cellList->setCurrentItem(asset<CellHolder>(selected[0]).get<CellItem*>());
							}
							mSuppressSelect = false;
						}
						break;
				}

				mState = eSNone;
				ui.view->update();
			}
			findCursorState();
			});
	}

	void CellsWidget::onMouseMove() {
		findCursorState();

		if (mAtlas && gMouse.state(0) == eInput::Hold) {
			auto pos = *mMainCam * gMouse.currentPosition();
			auto delta = pos - mGrabPos;
			auto selected = gProject.view<AtlasSelectedCell>();

			switch (mState) {
				case eSelecting:
					mSelectRect.r = pos.x;
					mSelectRect.t = pos.y;
					ui.view->update();
					break;

				case eMoving:
					for (asset<CellHolder> holder : selected) {
						assert(holder.has<AtlasMovingCell>());
						holder->rect = holder.get<AtlasMovingCell>().capturedRect;
						holder->rect.move(delta);
					}
					ui.view->update();
					break;

				case eSizing:
					if (selected.size() == 1) {
						for (asset<CellHolder> holder : selected) {
							if ((mCursorState & eLeft) == eLeft) {
								holder->rect.l = pos.x;
							} else if ((mCursorState & eRight) == eRight) {
								holder->rect.r = pos.x;
							}
							if ((mCursorState & eBottom) == eBottom) {
								holder->rect.b = pos.y;
							} else if ((mCursorState & eTop) == eTop) {
								holder->rect.t = pos.y;
							}
						}
						ui.view->update();
					}
					break;
			}
		}
	}

	void CellsWidget::deleteSelected() {
		auto selected = gProject.view<AtlasSelectedCell>();
		if (selected.size() > 0) {
			mSuppressSelect = true;
			for (asset<CellHolder> holder : selected) {
				deleteCell(holder);
			}
			gProject.clear<AtlasSelectedCell>();
			mSuppressSelect = false;
			reorderCellsAccordingToList();
			rebatchAllCellHolders();
			sig_Modified.invoke();
			ui.view->update();
		}
	}

	void CellsWidget::deleteCell(asset<CellHolder> holder) {
		delete holder.get<CellItem*>();
		auto cell = holder.get<SubAssetData>();
		mAtlas->cells.erase(cell.name);

		auto& meta = mAtlas.get<AtlasMeta>();
		meta.cellnames.erase(holder);

		holder.destroy();
	}

	asset<CellHolder> CellsWidget::createCell(const string& name) {
		assert(mAtlas);
		mSelectRect.roundCorners();
		mSelectRect.normalize();
		CellItem* item = new CellItem(gAtlsUtil.cellList);
		item->setFlags(item->flags() | Qt::ItemIsEditable);

		auto& meta = mAtlas.get<AtlasMeta>();

		auto holder = item->holder = gProject.make<CellHolder>(mSelectRect, this);
		holder
			.add<SubAssetData>(meta.cellorder.size(), name, mAtlas)
			.add<CellMeta>();
		mAtlas->addCell(holder, meta);
		holder.add<CellItem*>(item);
		holder->moldCellFromRect(holder, (int)meta.width, (int)meta.height);
		item->setText(QString::fromUtf8(name));

		mSuppressSelect = true;
		gAtlsUtil.cellList->addItem(item);
		gAtlsUtil.cellList->clearSelection();

		gProject.clear<AtlasSelectedCell>();
		item->setSelected(true);
		gAtlsUtil.cellList->setCurrentItem(item);
		mSuppressSelect = false;

		holder.update();
		return holder;
	}

	void CellsWidget::createNamedCell() {
		createCell(
			gAtlsUtil.cellList->getNoneConflictingName(
				QString::fromStdString(mAtlas.get<AssetData>().filePath.stem().generic_u8string()), false
			).toStdString()
		);
	}

	void CellsWidget::autoCreateCell() {
		static vector<int> valids;

		if (mAtlas && mMaterial && mMaterial->hasTexture()) {
			auto tex = mMaterial->textures[0];

			aabb rect;
			bool made = false;

			vec2 mousepix = (*mMainCam) * gMouse.currentPosition();

			uint pixcount = tex->width() * tex->height();
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, tex->id());

			auto pixels = (unsigned char*)malloc(pixcount);
			glPixelStorei(GL_PACK_ALIGNMENT, 1);
			glGetTexImage(GL_TEXTURE_2D, 0, GL_ALPHA, GL_UNSIGNED_BYTE, pixels);

			bool* visited = (bool*)calloc(pixcount, sizeof(bool));

			//TODO: change resolution
			uint reader = 0, rend = 1;
			mousepix.x = round(mousepix.x);
			mousepix.y = round(mousepix.y);
			mousepix.y *= -1;
			if (0 < mousepix.x &&
				0 < mousepix.y &&
				mousepix.x < tex->width() &&
				mousepix.y < tex->height()) {
				int start = mousepix.x + mousepix.y * tex->width();
				if (pixels[start] > mAlphaCut) {
					valids.clear();
					valids.push_back(start);
					visited[start] = true;

					while (true) {
						rend = valids.size();
						for (; reader < rend; reader++) {
							auto curr = valids[reader];
							bool notop =
								((curr < tex->width()) || (pixels[curr - tex->width()] <= mAlphaCut) || visited[curr - tex->width()] == true);
							bool nobottom =
								(curr > (tex->width() * (tex->height() - 1)) || (pixels[curr + tex->width()] <= mAlphaCut) || (visited[curr + tex->width()] == true));
							bool noleft =
								((curr % tex->width() == 0) || (pixels[curr - 1] <= mAlphaCut) || (visited[curr - 1] == true));
							bool noright =
								((curr % tex->width() == (tex->width() - 1)) || (pixels[curr + 1] <= mAlphaCut) || (visited[curr + 1] == true));
							/*bool notopleft =
								((curr < mTexWidth) || (pixels[curr - mTexWidth] <= 25) || visited[curr - mTexWidth] == true);
							bool notopright =
								(curr > (mTexWidth * (mTexHeight - 1)) || (pixels[curr + mTexWidth] <= 25) || (visited[curr + mTexWidth] == true));
							bool nobottomleft =
								((curr % mTexWidth == 0) || (pixels[curr - 1] <= 25) || (visited[curr - 1] == true));
							bool nobottomright =
								((curr % mTexWidth == (mTexWidth - 1)) || (pixels[curr + 1] <= 25) || (visited[curr + 1] == true));*/

							if (!notop) {
								valids.push_back(curr - tex->width());
								visited[curr - tex->width()] = true;
							} if (!nobottom) {
								valids.push_back(curr + tex->width());
								visited[curr + tex->width()] = true;
							} if (!noleft) {
								valids.push_back(curr - 1);
								visited[curr - 1] = true;
							} if (!noright) {
								valids.push_back(curr + 1);
								visited[curr + 1] = true;
							}
						}

						if (rend == valids.size())
							break;
					}

					auto intmax = std::numeric_limits<int>::max();
					int il = intmax;
					int ib = intmax;
					int ir = -intmax;
					int it = -intmax;
					for (uint i = 0; i < valids.size(); i++) {
						auto& s = valids[i];
						il = min(il, (s % (int)tex->width()));
						ib = min(ib, (s / (int)tex->width()));
						ir = max(ir, (s % (int)tex->width()) + 1);
						it = max(it, (s / (int)tex->width()) + 1);
					}

					rect = aabb(il, -it, ir, -ib);
					made = true;
				}
			}
			free(pixels);

			if (made) {
				mSelectRect = Box(rect.l, rect.b, rect.r, rect.t);
				createNamedCell();
				rebatchAllCellHolders();
				sig_Modified.invoke();
				ui.view->update();
			}
		}
	}

	void CellsWidget::connectList() {
		connect(gAtlsUtil.cellList, &QListExtension::itemSelectionChanged, [&]() {
			if (isVisible() && !mSuppressSelect && mAtlas) {
				gProject.clear<AtlasSelectedCell>();
				for (auto i = 0; i < gAtlsUtil.cellList->count(); i++) {
					CellItem* item = reinterpret_cast<CellItem*>(gAtlsUtil.cellList->item(i));
					if (item->isSelected())
						gProject.get_or_emplace<AtlasSelectedCell>(item->holder);
				} ui.view->update();
			}
			});

		connect(gAtlsUtil.cellList->model(), &QAbstractItemModel::rowsMoved, [&]() {
			if (isVisible() && !mSuppressSelect && mAtlas) {
				reorderCellsAccordingToList();
				sig_Modified.invoke();
			}
			});

		connect(gAtlsUtil.cellList->itemDelegate(), &QAbstractItemDelegate::commitData, [&](QWidget* pLineEdit) {
			if (isVisible() && mAtlas && mAtlas.has<AssetLoaded>()) {
				CellItem* item = reinterpret_cast<CellItem*>(gAtlsUtil.cellList->currentItem());
				auto& data = item->holder.get<SubAssetData>();
				auto name = gAtlsUtil.cellList->
					getNoneConflictingName(reinterpret_cast<QLineEdit*>(pLineEdit)->text()).toStdString();

				assert(mAtlas->cells.contains(data.name));
				if (data.name != name && mAtlas->cells.contains(data.name)) {
					mAtlas->cells.erase(data.name);
					mAtlas->cells[name] = item->holder;
					mAtlas.get<AtlasMeta>().cellnames[item->holder] = name;
					item->setText(QString::fromStdString(name));
					data.name = name;
					sig_Modified.invoke();
				}
			}
			});
	}

	void CellsWidget::combineCells() {
		auto selected = gProject.view<AtlasSelectedCell>();
		if (selected.size() > 1) {
			auto& meta = mAtlas.get<AtlasMeta>();

			auto& item = *asset<CellItem*>(selected[0]);
			string name = item->text().toStdString();
			int l = std::numeric_limits<int>::max();
			int b = std::numeric_limits<int>::max();
			int r = std::numeric_limits<int>::min();
			int t = std::numeric_limits<int>::min();

			mSuppressSelect = true;
			gAtlsUtil.cellList->clearSelection();
			sizet i = 0;
			for (asset<CellHolder> holder : selected) {
				l = min(l, holder->rect.l);
				b = min(b, holder->rect.b);
				r = max(r, holder->rect.r);
				t = max(t, holder->rect.t);
				if (i != 0)
					deleteCell(holder);
				i++;
			}
			mSuppressSelect = false;

			auto first = asset<CellHolder>(selected[0]);
			first->rect = Box(l, b, r, t);
			first->moldCellFromRect(first, (int)meta.width, (int)meta.height);
			gProject.clear<AtlasSelectedCell>();
			gAtlsUtil.cellList->setCurrentItem(first.get<CellItem*>());
			reorderCellsAccordingToList();
			rebatchAllCellHolders();
			sig_Modified.invoke();
			ui.view->update();
		}
	}

	void CellsWidget::recreateList() {
		auto& list = *gAtlsUtil.cellList;
		for (auto i = 0; i < list.count(); i++) {
			delete list.item(i);
		} list.clear();

		if (mAtlas) {
			auto& meta = mAtlas.get<AtlasMeta>();
			auto& cells = meta.cellorder;

			mSuppressSelect = true;
			for (asset<SubAssetData> cell : cells) {
				CellItem* item = new CellItem(&list);
				item->setFlags(item->flags() | Qt::ItemIsEditable);
				item->holder = cell;
				item->setText(QString::fromUtf8(cell->name));
				gProject.emplace_or_replace<CellItem*>(cell, item);
				list.addItem(item);
			}
			mSuppressSelect = false;
		}
	}

	void CellsWidget::showEditor() {
		auto list = gAtlsUtil.cellList;
		list->setSelectionMode(QAbstractItemView::SelectionMode::ExtendedSelection);
		list->setDragDropMode(QAbstractItemView::DragDropMode::DragDrop);
		list->setDefaultDropAction(Qt::DropAction::MoveAction);
		list->show();

		auto curr = gProject.view<AtlasCurrentCell>();
		if (curr.size() > 0) {
			asset<CellHolder> holder = curr[0];
			auto item = *asset<CellItem*>(holder);
			holder.add<AtlasSelectedCell>();
			mSuppressSelect = true;
			item->setSelected(true);
			mSuppressSelect = false;

			mMainCamTarget.to(vec3(holder.get<CellHolder>().rect.center(), -1000.0f));
			syncCameraTarget();
			snapCamera();
			syncScrollBarPositionToCam();
			ui.view->update();
		}

		//if (mMainCam)
		//	setCameraScale(gAtlsUtil.globalScale);
		recreateCellHoldersFromAtlas(); //TODO: fix the entire workflow with CellHolder, do not attach holder to project assets themselves 
		//		(Really makes things complicated when there can be multiple palettes open)
		//		This method should be unnecessary here once it's done
		show();
	}

	void CellsWidget::hideEditor() {
		gAtlsUtil.cellList->hide();

		gProject.clear<AtlasCurrentCell>();
		auto item = reinterpret_cast<CellItem*>(gAtlsUtil.cellList->currentItem());
		if (item)
			item->holder.add<AtlasCurrentCell>();
		safeClearSelection();

		//if (mMainCam)
		//	gAtlsUtil.globalScale = mMainCam->scale().x;
		hide();
	}

	void CellsWidget::findCursorState() {
		if (mMaterial && mMaterial->hasTexture() && mMaterial->textures[0].has<AssetLoaded>()) {

			auto pos = *mMainCam * gMouse.currentPosition();
			if (!mAtlas && mAtlas.has<AssetLoaded>()) {
				mTempCursor = Qt::ArrowCursor;
				mCursorState = CursorState::eNone;
				return;
			}

			if (mState == eSNone) {
				mCursorState = CursorState::eNone;
				auto selected = gProject.view<AtlasSelectedCell>();

				auto sx = mMainCam->scale().x;
				bool hit = false; // = mSelects.size() > 0;
				for (asset<CellHolder> holder : selected) {
					auto rect = holder->rect;
					rect.expand(6.0f * sx, 6.0f * sx);
					if (rect.contains(pos)) {
						hit = true;
						break;
					}
				}

				if (hit) {
					if (selected.size() == 1) {
						auto rect = asset<CellHolder>(selected[0])->rect;
						rect.expand(6.0f * sx, 6.0f * sx);
						if (rect.l > pos.x - 12.0f * sx)
							mCursorState += CursorState::eLeft;
						else if (rect.r < pos.x + 12.0f * sx)
							mCursorState += CursorState::eRight;
						if (rect.b > pos.y - 12.0f * sx)
							mCursorState += CursorState::eBottom;
						else if (rect.t < pos.y + 12.0f * sx)
							mCursorState += CursorState::eTop;
					}

					if (mCursorState == 0)
						mCursorState = CursorState::eMove;
				} else if (gMouse.state(1) == eInput::Hold) {
					mTempCursor = Qt::CursorShape::SizeAllCursor;
				} else {
					mTempCursor = Qt::ArrowCursor;
				}

				if (QApplication::queryKeyboardModifiers() & Qt::AltModifier) {
					if (mCursorState == CursorState::eMove)
						mTempCursor = Qt::OpenHandCursor;

					bool left = ((mCursorState & CursorState::eLeft) == CursorState::eLeft);
					bool right = ((mCursorState & CursorState::eRight) == CursorState::eRight);
					bool top = ((mCursorState & CursorState::eTop) == CursorState::eTop);
					bool bottom = ((mCursorState & CursorState::eBottom) == CursorState::eBottom);

					if (left || right) {
						mTempCursor = Qt::SizeHorCursor;
					}
					if (top || bottom) {
						if (mTempCursor == Qt::SizeHorCursor) {
							if ((left && top) || (right && bottom)) {
								mTempCursor = Qt::SizeFDiagCursor;
							} else mTempCursor = Qt::SizeBDiagCursor;
						} else
							mTempCursor = Qt::SizeVerCursor;
					}
				} else if (gMouse.state(1) == eInput::Hold) {
					mTempCursor = Qt::CursorShape::SizeAllCursor;
				} else {
					mTempCursor = Qt::ArrowCursor;
				}
			}

			if (mState == eMoving) {
				mTempCursor = Qt::ClosedHandCursor;
			}

			if (mTempCursor == Qt::ArrowCursor && QApplication::queryKeyboardModifiers() & Qt::ControlModifier) {
				mTempCursor == Qt::CrossCursor;
			}

			setCursor(mTempCursor);
		}
	}
}