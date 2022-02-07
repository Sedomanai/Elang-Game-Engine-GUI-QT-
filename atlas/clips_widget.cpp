#include "clips_widget.h"
#include <qmenu.h>
#include <qlayout.h>
#include <qscrollbar.h>
#include <components/canvas.h>
#include <qlineedit>

namespace il
{
	ClipsView::ClipsView(QWidget* parent) : 
		QIlangView(parent), mCurrFrame(0.0f), mSelect(0), mSpeed(0.2f), mDuration(0.0f), mMovingCell(false)
	{
		connectList();

		connect(this, &ClipsView::sig_InitializeGL, [&]() {
			revent->sig_Select.connect([&](uint index) {
				gMouse.reset();
				gPalette->open(gTexKey, &gCells);
				auto select = gPalette->selectedCellIndex;
				gMouse.reset();

				if (select != -1) {
					mSelect->at(index) = gCells.name(select);
					recreateReel();
				}
				});

			revent->sig_Erase.connect([&](uint index) {
				mSelect->erase(mSelect->begin() + index);
				recreateReel();
				});

			revent->sig_Move.connect([&](uint index) {
				Entity hover = mScene.mousehub.data().hovering;
				Entity origin = mScene.mousehub.data().select[MouseSym::MIDDLE];
				if (hover != entt::null &&
					hover != mScene.mousehub.data().select[MouseSym::MIDDLE] &&
					mScene.objects.has<ReelFrame>(hover)) {
					auto& frame = mScene.objects.get<ReelFrame>(hover);
					auto& framef = mScene.objects.get<ReelFrame>(origin);
					auto temp = mEnts[framef.index];
					mEnts.erase(mEnts.begin() + framef.index);
					mEnts.insert(mEnts.begin() + frame.index, temp);

					auto tempc = mSelect->at(framef.index);
					mSelect->erase(mSelect->begin() + framef.index);
					mSelect->insert(mSelect->begin() + frame.index, tempc); // reasons..

					framef.col = color(255, 0, 255, 255);
					reorder();

					update();
				}
				});

			makeCurrent();
			mScene.makeCurrent();
			mMainView = mScene.cameras.create("Main Cam");
			mMainView->move(vec3(0, 0, -1000));
			mReelView = mScene.cameras.create("Reel Cam");
			mReelView->move(vec3(0, 0, -1000));

			auto painter = mScene.atelier.create("Texture Painter");
			painter->init("basic_sprite.vert", "raw_pass.frag", 12, mMainView, 0, 0);
			painter = mScene.atelier.create("Cell Painter");
			painter->init("basic_sprite.vert", "raw_pass.frag", 1024, mReelView, 0, 0);
			mScene.debug.init(mMainView);

			mTexmat = mScene.materials.create("Texture");
			mTexmat->addTexture("");

			mSprite = Batch(malloc(sizeof(SpriteVertex) * 4), 4, &gBox2dFillIndices[0], 6, mTexmat, 0, 0);

			updateTexture();

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glClearColor(0.2f, 0.3f, 0.2f, 1.0f);

			mInitialized = true;

			connectMouseInput();
			});

		connect(this, &ClipsView::sig_PaintGL, [&]() {
			makeCurrent();
			mScene.makeCurrent();
			glClearDepth(1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			updateViewport(-width() / 2.0f, width() / 2.0f, -height() / 2.0f, height() / 2.0f);

			auto tex = gTextures.find(gTexKey);
			if (tex && mSelect && mSelect->size() > 0) {
				auto index = gCells.index(mSelect->at(safeFrame()));
				if (index != -1) {
					index = clamp(index, 0, gCells.count() - 1); //safety net
					SpriteVertex* verts = (SpriteVertex*)mSprite.vertices;
					auto& data = gCells.at(index);
					auto& cell = Cell(data.x, data.y, data.w, data.h, data.ox, data.oy, tex->width, tex->height);

					//???
					/*cell.left += 2;
					cell.right -= 2;
					cell.down += 2;
					cell.up -= 2;*/

					//safety net
					auto left = round(cell.left);
					auto right = round(cell.right);
					auto up = round(cell.up);
					auto down = round(cell.down);


					verts[0].pos = vec2(left, up);
					verts[0].uv = vec2(cell.uvLeft, cell.uvUp);
					verts[1].pos = vec2(right, up);
					verts[1].uv = vec2(cell.uvRight, cell.uvUp);
					verts[2].pos = vec2(right, down);
					verts[2].uv = vec2(cell.uvRight, cell.uvDown);
					verts[3].pos = vec2(left, down);
					verts[3].uv = vec2(cell.uvLeft, cell.uvDown);
					mScene.atelier.get("Texture Painter")->batch(SpriteVertexData::sVertexDataIndex, mSprite);
				}
			}

			for (auto e : mScene.objects.view<Canvas>()) {
				auto& can = mScene.objects.get<Canvas>(e);
				can.debug(&mScene.debug, mScene.objects.get<ReelFrame>(e).col, false);
			}

			mScene.paint();
			mScene.debug.line.camera = mReelView;
			mScene.debug.draw();
			});
	}

	ClipsView::~ClipsView() {
		if (mInitialized) {
			mTexmat->reset();
			free(mSprite.vertices);
		}
	}

	void ClipsView::connectMouseInput() {
		connect(this, &ClipsView::sig_MousePress, [&]() {
			mScene.makeCurrent();
			mScene.mousehub.update(mReelView);
			});
		connect(this, &ClipsView::sig_MouseRelease, [&]() {
			if (gMouse.state(MouseSym::MIDDLE) == eInput::ONCE) {
				auto sel = mScene.mousehub.data().select[MouseSym::MIDDLE];
				if (sel != entt::null && mScene.objects.has<ReelFrame>(sel)) {
					mScene.objects.get<ReelFrame>(sel).col = color(0, 255, 0, 255);
					recreateReel();
				}
			}
			mScene.mousehub.update(mReelView);
			});
		connect(this, &ClipsView::sig_MouseMove, [&]() {
			mScene.mousehub.update(mReelView);
			update();
			});
		connect(this, &ClipsView::sig_ScrollWheel, [&]() {
			float val = (5.0f / 6.0f);
			val = pow(val, gMouse.wheel());
			if (val != 0) {
				val = (val > 0) ? val : -1.0f / val; // change
				mMainView->scale(vec3(val, val, 1));
			};
			update();
		});

	}

	void ClipsView::recreateReel() {
		mScene.makeCurrent();
		static uint rcellsize = 70;

		mEnts.clear();
		for (auto e : mScene.objects.view<Canvas>()) {
			mScene.objects.destroy(e);
		}

		if (mSelect) {
			for (uint i = 0; i < mSelect->size(); i++) {
				auto e = mScene.objects.create<Object2d>();
				auto& canv = mScene.objects.emplace<Canvas>(e, mTexmat, mScene.atelier.get("Cell Painter"));
				canv.bounds = aabb(0, 0, rcellsize - 2, rcellsize - 2);
				mScene.objects.emplace<Button>(e).buttonEvent = revent;
				mScene.objects.emplace<HitAABB>(e) = HitAABB(canv.bounds.l, canv.bounds.b, canv.bounds.r, canv.bounds.t);
				mScene.objects.emplace<ReelFrame>(e);

				uint cval = gCells.find(mSelect->at(i));
				if (cval != -1) {
					auto& cell = gCells.at(cval);

					canv.uvpos = vec2(cell.x, cell.y);
					canv.uvsize = vec2(cell.w, cell.h);
				} else {
					canv.uvpos = vec2(0, 0);
					canv.uvsize = vec2(0, 0);
				}
				mEnts.push_back(e);
			}

			reorder();
			sig_ChangeBar.invoke(mSelect->size());
		}

		mCurrFrame = 0.0f;
		update();
	}

	void ClipsView::reorder() {
		static uint rcellsize = 70;
		static uint linemax = size().width() / rcellsize;
		uint linecount = (mEnts.size() - 1) / linemax;
		for (uint i = 0; i < mEnts.size(); i++) {
			uint line = i / linemax;

			auto e = mEnts[i];
			mScene.objects.get<ReelFrame>(e).index = i;

			auto& tr = mScene.objects.get<Object2d>(e);
			tr.setPosXLocal(-size().width() / 2.0f + rcellsize * (i % linemax));
			tr.setPosYLocal(-size().height() / 2.0f + rcellsize * (linecount - line));
		}
	}

	void ClipsView::importAtlasBegin() {
		gClips.clear();
		gClipList->clear();
	}

	void ClipsView::importAtlasEnd() {
		recreateList();
		update();
	}

	void ClipsView::showEditor() {
		mScene.makeCurrent();
		gClipList->show();
		show();
		updateTexture();

		if (gClipList->count() > 0) {
			gClipRow = clamp(gClipRow, 0, gClipList->count() - 1);
		} else gClipRow = -1;

		gClipList->setCurrentRow(gClipRow);
	}

	void ClipsView::hideEditor() {
		gClipList->clearSelection();
		gClipList->setCurrentRow(-1);
		gClipList->hide();
		gClipRow = gClipList->currentRow();
		hide();
		mSelect = 0;
	}

	void ClipsView::loop() {
		if (mSelect && mSelect->size() > 0) {
			auto maxframe = mSelect->size();
			while (mCurrFrame > maxframe) {
				mCurrFrame -= maxframe;
			} mCurrFrame += mSpeed;
			
			/*auto curr = mSelect->at(safeFrame());
			mDuration += mSpeed;
			while (mDuration > curr.duration) {
				mDuration -= curr.duration;
				mCurrFrame++;
				if (mCurrFrame >= maxframe)
					mCurrFrame -= maxframe;
				curr = mSelect->at(safeFrame());
			}*/
		}

		update();
	}

	void ClipsView::updateTexture() {
		auto tex = gTextures.find(gTexKey);
		if (tex && mTexmat) {
			mTexmat->textures[0] = tex;
			update();
		}
	}

	uint ClipsView::safeFrame() {
		auto frame = floor(mCurrFrame);
		frame = clamp(frame, 0, mSelect->size() - 1);
		return frame;
	}

	void ClipsView::addFrame() {
		if (mSelect) {
			if (mSelect->size() > 0) {
				mSelect->push_back(mSelect->back());
			}
			else mSelect->push_back(gCells.at(0).name);
			recreateReel();
		}
	}

	void ClipsView::connectList() {
		connect(gClipList, &QListExtension::currentRowChanged, [&]() {
			mScene.makeCurrent();
			auto row = gClipList->currentRow();
			if (row >= 0) {
				mSelect = &gClips.at(row);
			} else {
				mSelect = 0;
			}
			setFocus();
			recreateReel();
		});

		connect(gClipList, &QWidget::customContextMenuRequested, [=](const QPoint& pos) {
			QMenu contextMenu(tr("Context menu"), this);
			auto CREATE_ACT = contextMenu.addAction("Create");
			auto DELETE_ACT = contextMenu.addAction("Delete");

			auto selected = contextMenu.exec(gClipList->mapToGlobal(pos));
			if (selected) {
				if (selected == CREATE_ACT) {
					string basename;
					tokenize(gTexKey, '/', [&](strview, strview tail) {
						tokenize(tail, '.', [&](strview head, strview) {
							basename = head;
							});
						});

					for (uint i = 0; i < gClips.count(); i++) {
						if (gClips.name(i) == basename) {
							basename += '_';
							i = 0;
						}
					}
					gClips.emplace(basename);
					auto item = new QListWidgetItem(QString::fromStdString(basename));
					item->setFlags(item->flags() | Qt::ItemIsEditable);
					gClipList->addItem(item);
					gClipList->setCurrentItem(item);
					mSelect = &gClips.at(basename);
				} else if (selected == DELETE_ACT) {
					auto row = gClipList->currentRow();
					auto item = gClipList->takeItem(row);
					if (item) 
						delete item;
					gClips.erase(row);
				}
			}
		});

		connect(gClipList->itemDelegate(), &QAbstractItemDelegate::commitData, [&](QWidget* pLineEdit) {
			auto new_name = reinterpret_cast<QLineEdit*>(pLineEdit)->text();
			for (uint i = 0; i < gClipList->count(); i++) {
				if (gClipList->currentRow() != i && gClipList->item(i)->text() == new_name) {
					new_name.push_back('_');
					i = 0;
				}
			}
			QString old_name = gClipList->item(gClipList->currentRow())->text();
			gClipList->item(gClipList->currentRow())->setText(new_name);
			gClips.rename(gClipList->currentRow(), new_name.toStdString());
			setFocus();
		});

		connect(gClipList->model(), &QAbstractItemModel::rowsMoved, [&](const QModelIndex&, int from, int, const QModelIndex&, int to) {
			if (from < to)
				to--;
			if (uint(to) >= gClipList->count())
				to = gClipList->count() - 1;
			if (from < 0)
				from = 0;
			gClips.swap(from, to);

			auto row = gClipList->currentRow();
			if (row >= 0) {
				mSelect = &gClips.at(row);
			} else {
				mSelect = 0;
			}
		});
	}

	void ClipsView::recreateList() {
		gClipList->clear();
		for (uint i = 0; i < gClips.count(); i++) {
			auto& clip = gClips.nameAt(i);
			auto item = new QListWidgetItem(QString::fromStdString(clip));
			item->setFlags(item->flags() | Qt::ItemIsEditable);
			gClipList->addItem(item);
		}
	}

	void ReelEvent::onGrab(uint buttonIndex, ButtonData& data) {
		if (buttonIndex == MouseSym::MIDDLE) {
			Entity e = data.select[buttonIndex];
			if (e != entt::null && gObjects->has<ReelFrame>(e)) {
				sig_Move.invoke(gObjects->get<ReelFrame>(e).index);
			}
		}
	}

	void ReelEvent::onRelease(uint buttonIndex, ButtonData& data) {
		Entity e;
		switch (buttonIndex) {
		case MouseSym::LEFT:
			e = data.select[buttonIndex];
			if (e != entt::null && gObjects->has<ReelFrame>(e)) {
				sig_Select.invoke(gObjects->get<ReelFrame>(e).index);
			}
			break;
		case MouseSym::RIGHT:
			e = data.select[buttonIndex];
			if (e != entt::null && gObjects->has<ReelFrame>(e)) {
				sig_Erase.invoke(gObjects->get<ReelFrame>(e).index);
			}
			break;
		}
	}

};