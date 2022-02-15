#include "clips_widget.h"

namespace el
{

	ClipsWidget::ClipsWidget(QWidget* parent) : mPaused(false) {
		ui.setupUi(this);
		ui.scroll->setEnabled(false);

		ui.frames->setFocusPolicy(Qt::StrongFocus);
		ui.frames->setMouseTracking(true);
		
		ui.view->sig_Start.connect([&]() {
			glClearColor(0.2f, 0.2f, 0.5f, 1.0f);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			safeCreateViewObjects();
			connectList();
		});

		ui.frames->sig_Start.connect([&]() {
			glClearColor(0.2f, 0.2f, 0.5f, 1.0f);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			safeCreateFrameObjects();
		});

		ui.view->sig_Paint.connect([&]() {
			auto item = reinterpret_cast<ClipItem*>(gAtlsUtil.clipList->currentItem());
			if (item && item->clip) {
				auto& clip = *item->clip;
				auto count = clip.size();
				if (count > 0) {
					if (mFrame >= count)
						mFrame = 0;
					if (mClipObj) {
						mClipObj->setCell(clip[mFrame].cell);
						mClipObj->update(mClipObj);
						mClipObj->batch();
						mPainterView->paint();
					}
				}
			}
		});


		ui.frames->sig_Paint.connect([&]() {
			updateViewport(0, ui.frames->width(), -ui.frames->height(), 0);

			for (obj<EditorProjectCanvas> canvas : gStage->view<EditorProjectCanvas>()) {
				canvas->batch();
			}
			mPainterFrames->paint();
		});


		ui.frames->sig_MousePress.connect([&]() {
			updateAllCanvasButton();
			});
		ui.frames->sig_MouseRelease.connect([&]() {
			updateAllCanvasButton();
			});
		ui.frames->sig_MouseMove.connect([&]() {
			updateAllCanvasButton();
			});
		ui.frames->sig_ScrollWheel.connect([&]() {
			if (ui.scroll->isEnabled()) {
				float val = -30.0f;
				val *= gMouse.wheel();
				ui.scroll->setValue((int)val + ui.scroll->value());
			}
		}); 

		connect(ui.scroll, &QScrollBar::valueChanged, [&](int value) {
			mMainCamFrames->setPosX(value);
			ui.frames->update();
		});

		crng.seed(SEED()());
	}

	void ClipsWidget::loop() {
		mFrame++;
		ui.view->update();
	}

	void ClipsWidget::addClip() {		
		if (mTexture && mTexture->atlas) {
			ClipItem* item = new ClipItem(gAtlsUtil.clipList);
			auto& list = *gAtlsUtil.clipList;
			assert(gProject->atlases.contains(mTexture->atlas));
			auto clipname = gProject->atlases[mTexture->atlas];
			item->setText( list.getNoneConflictingName(QString::fromUtf8(clipname), false) );

			item->clip = gProject->makeSub<Clip>();
			mTexture->atlas->clips.emplace(item->text().toStdString(), item->clip);
			list.addItem(item);

			// select maybe
			ui.view->update();
			ui.frames->update();
		}
	}

	void ClipsWidget::addFrame() {
		if (gAtlsUtil.cellList->count() > 0) {
			ClipItem* item = reinterpret_cast<ClipItem*>(gAtlsUtil.clipList->currentItem());
			if (item && item->clip) {
				irand r(0, gAtlsUtil.cellList->count() - 1);
				CellItem* cit = reinterpret_cast<CellItem*>(gAtlsUtil.cellList->item(r(crng)));
				Clipframe frame;
				frame.duration = 1;
				frame.cell = cit->holder->cell;
				item->clip->emplace_back(frame);
				addReel(frame);
			}
		}
	}

	void ClipsWidget::addReel(Clipframe frame) {
		ClipItem* item = reinterpret_cast<ClipItem*>(gAtlsUtil.clipList->currentItem());
		assert(item && item->clip);
		auto size = gStage->view<EditorProjectCanvas>().size();
		auto rframe = gStage->make<EditorProjectCanvas>(gAtlsUtil.currentMaterial, mPainterFrames);
		rframe->bounds = aabb(size * 100, -100, size * 100 + 100, -0);
		if (frame.cell)
			rframe->update(frame.cell);
		rframe.add<Button>(this);
		rframe.add<sizet>(item->clip->size() - 1);
		syncScroll();
		ui.frames->update();
	}


	void ClipsWidget::syncScroll() {
		 auto length = gStage->view<EditorProjectCanvas>().size() * 100;
		if (ui.frames->width() < length) {
			ui.scroll->setEnabled(true);
			ui.scroll->setRange(0, length - ui.frames->width());
		} else {
			ui.scroll->setEnabled(false);
		}
	}

	void ClipsWidget::updateAllCanvasButton() {
		auto pos = gMouse.currentPosition();
		pos.x += ui.frames->width() / 2.0f + mMainCamFrames->position().x;
		pos.y -= ui.frames->height() / 2.0f; // because of updateViewport in ui.frames->sig_Paint

		sizet i = 0;

		auto& stage = *gStage;
		for (auto button : stage.view<EditorProjectCanvas>()) {
			assert(stage.valid(button) && stage.all_of<EditorProjectCanvas>(button));
			bool hit = stage.get<EditorProjectCanvas>(button).bounds.contains(pos);
			stage.get<Button>(button).update(button, hit);
		}
	}

	void ClipsWidget::safeCreateViewObjects() {
		if (!mMainCamView) {
			ui.view->makeCurrent();
			mMainCamView = gProject->makeSub<EditorCamera>();
			mMainCamView->to(vec3(0.0f, 0.0f, -1000.0f));

			mPainterView = gProject->makeSub<EditorProjectPainter>("basic_sprite", "texture_uv", 32, mMainCamView, Projection::eOrtho,
				ePainterFlags::DEPTH_SORT | ePainterFlags::MULTI_MATERIAL | ePainterFlags::Z_CLEAR);
			mPainterView->init();

			mDebugView = new EditorShapeDebug;
			mDebugView->init(mMainCamView);
		}

		if (!mClipObj) {
			mClipObj = gStage->make<EditorProjectSprite>(gAtlsUtil.currentMaterial, mPainterView, "");
			mClipObj.add<Position>();
		}
	}

	void ClipsWidget::safeCreateFrameObjects() {
		if (!mMainCamFrames) {
			ui.frames->makeCurrent();
			mMainCamFrames = gProject->makeSub<EditorCamera>();
			mMainCamFrames->to(vec3(0.0f, 0.0f, -1000.0f));

			mPainterFrames = gProject->makeSub<EditorProjectPainter>("basic_sprite", "texture_uv", 3000, mMainCamFrames, Projection::eOrtho,
				ePainterFlags::DEPTH_SORT | ePainterFlags::MULTI_MATERIAL | ePainterFlags::Z_CLEAR);
			mPainterFrames->init();

			//hmm.. will i need this?
			mDebugFrames = new EditorShapeDebug;
			mDebugFrames->init(mMainCamFrames);
		}

		if (!mTestObj) {
			mTestObj = gStage->make<EditorProjectSprite>(gAtlsUtil.currentMaterial, mPainterFrames, "");
			mTestObj.add<Position>();
			mTestObj->update(mTestObj);
		}
	}


	void ClipsWidget::updateTexture() {
		auto mat = gAtlsUtil.currentMaterial;
		if (mat && mat->hasTexture()) {
			mTexture = mat->textures[0];
			if (mTexture->atlas) {

				auto view = gStage->view<EditorProjectCanvas>();
				gStage->destroy(view.begin(), view.end());

				auto& list = *gAtlsUtil.clipList;
				list.clearSelection();
				list.setCurrentRow(-1);

				for (sizet i = 0; i < list.count(); i++) {
					delete list.item(i);
				}

				list.clear();

				for (auto it : mTexture->atlas->clips) {
					cout << it.first << endl;
					ClipItem* item = new ClipItem(&list);
					item->setText(QString::fromUtf8(it.first));
					item->clip = it.second;
					list.addItem(item);
				}
			}
		}
	}

	void ClipsWidget::connectList() {
		connect(gAtlsUtil.clipList, &QListExtension::currentRowChanged, [&]() {
			assert(mClipObj);

			auto view = gStage->view<EditorProjectCanvas>();
			gStage->destroy(view.begin(), view.end());

			ClipItem* item = reinterpret_cast<ClipItem*>(gAtlsUtil.clipList->currentItem());
			assert(item && item->clip);
			for (auto i = 0; i < item->clip->size(); i++) {
				addReel(item->clip->at(i));
			}
			
			setFocus();
			ui.view->update();
			ui.frames->update();
		});
	}

	void ClipsWidget::showEditor() {
		gAtlsUtil.clipList->show();
		show();
	}

	void ClipsWidget::hideEditor() {
		gAtlsUtil.clipList->hide();
		hide();
	}
	void ClipsWidget::onEnter(Entity self, Entity context) {

		
	}

	void ClipsWidget::onHover(Entity self, Entity context) {
		if (gMouse.state(0) == eInput::LIFT) {
			auto prev = gStage;
			Stage stage;
			QDialog dialog(this);
			AtlasPalette palette(&dialog, true);
			palette.view()->setStage(&stage);

			palette.view()->sig_Start.connect([&](){
				palette.updateMaterial(gAtlsUtil.currentMaterial);
			});

			palette.sig_Clicked.connect([&](CellResult h) {
				bind(prev);
				assert(obj<sizet>(self));
				sizet index = *obj<sizet>(self);
				ClipItem* item = reinterpret_cast<ClipItem*>(gAtlsUtil.clipList->currentItem());
				assert(item && item->clip);
				assert(index < item->clip->size());

				item->clip->at(index).cell = h.cell;
				obj<EditorProjectCanvas>(self)->update(h.cell);
				ui.frames->update();
				dialog.close();
			});

			dialog.exec();
		} 
		
	}

	void ClipsWidget::onExit(Entity self, Entity context) {
		
	}

	void ClipsWidget::postUpdate(Entity self, Entity context) {
		
	}

	void ClipsWidget::recreateReel() {
		ui.view->makeCurrent();
		static sizet rcellsize = 70;

		auto view = gStage->view<Canvas>();
		gStage->destroy(view.begin(), view.end());

		ClipItem* item = reinterpret_cast<ClipItem*>(gAtlsUtil.cellList->currentItem());
		if (item && item->clip) {
			auto& clip = *item->clip;

			for (sizet i = 0; i < clip.size(); i++) {
				for (auto i = 0; i < clip[i].duration; i++) {
					auto canvas = gStage->make<Canvas>(gAtlsUtil.currentMaterial, mPainterFrames);
					canvas->bounds = aabb(0, 0, rcellsize - 2, rcellsize - 2);
					//canvas.add<Button>(this, clip[i].cell);
				}
			}
			//	uint cval = gCells.find(mSelect->at(i));
			//	if (cval != -1) {
			//		auto& cell = gCells.at(cval);

			//		canv.uvpos = vec2(cell.x, cell.y);
			//		canv.uvsize = vec2(cell.w, cell.h);
			//	} else {
			//		canv.uvpos = vec2(0, 0);
			//		canv.uvsize = vec2(0, 0);
			//	}
			//	mEnts.push_back(e);

			//reorder();
			//sig_ChangeBar.invoke(mSelect->size());
		}

		update();
	}


	//ClipsWidget::ClipsWidget(QWidget* parent) : 
	//	QIlangView(parent), mCurrFrame(0.0f), mSelect(0), mSpeed(0.2f), mDuration(0.0f), mMovingCell(false)
	//{
	//	connectList();

	//		revent->sig_Erase.connect([&](uint index) {
	//			mSelect->erase(mSelect->begin() + index);
	//			recreateReel();
	//			});

	//		revent->sig_Move.connect([&](uint index) {
	//			Entity hover = mScene.mousehub.data().hovering;
	//			Entity origin = mScene.mousehub.data().select[MouseSym::MIDDLE];
	//			if (hover != entt::null &&
	//				hover != mScene.mousehub.data().select[MouseSym::MIDDLE] &&
	//				mScene.objects.has<ReelFrame>(hover)) {
	//				auto& frame = mScene.objects.get<ReelFrame>(hover);
	//				auto& framef = mScene.objects.get<ReelFrame>(origin);
	//				auto temp = mEnts[framef.index];
	//				mEnts.erase(mEnts.begin() + framef.index);
	//				mEnts.insert(mEnts.begin() + frame.index, temp);

	//				auto tempc = mSelect->at(framef.index);
	//				mSelect->erase(mSelect->begin() + framef.index);
	//				mSelect->insert(mSelect->begin() + frame.index, tempc); // reasons..

	//				framef.col = color(255, 0, 255, 255);
	//				reorder();

	//				update();
	//			}
	//			});


	//void ClipsWidget::connectMouseInput() {
	//	connect(this, &ClipsWidget::sig_MousePress, [&]() {
	//		mScene.makeCurrent();
	//		mScene.mousehub.update(mReelView);
	//		});
	//	connect(this, &ClipsWidget::sig_MouseRelease, [&]() {
	//		if (gMouse.state(MouseSym::MIDDLE) == eInput::ONCE) {
	//			auto sel = mScene.mousehub.data().select[MouseSym::MIDDLE];
	//			if (sel != entt::null && mScene.objects.has<ReelFrame>(sel)) {
	//				mScene.objects.get<ReelFrame>(sel).col = color(0, 255, 0, 255);
	//				recreateReel();
	//			}
	//		}
	//		mScene.mousehub.update(mReelView);
	//		});
	//	connect(this, &ClipsWidget::sig_MouseMove, [&]() {
	//		mScene.mousehub.update(mReelView);
	//		update();
	//		});
	//	connect(this, &ClipsWidget::sig_ScrollWheel, [&]() {
	//		float val = (5.0f / 6.0f);
	//		val = pow(val, gMouse.wheel());
	//		if (val != 0) {
	//			val = (val > 0) ? val : -1.0f / val; // change
	//			mMainView->scale(vec3(val, val, 1));
	//		};
	//		update();
	//	});

	//}


	//void ClipsWidget::reorder() {
	//	static uint rcellsize = 70;
	//	static uint linemax = size().width() / rcellsize;
	//	uint linecount = (mEnts.size() - 1) / linemax;
	//	for (uint i = 0; i < mEnts.size(); i++) {
	//		uint line = i / linemax;

	//		auto e = mEnts[i];
	//		mScene.objects.get<ReelFrame>(e).index = i;

	//		auto& tr = mScene.objects.get<Object2d>(e);
	//		tr.setPosXLocal(-size().width() / 2.0f + rcellsize * (i % linemax));
	//		tr.setPosYLocal(-size().height() / 2.0f + rcellsize * (linecount - line));
	//	}
	//}

	//void ClipsWidget::importAtlasBegin() {
	//	gClips.clear();
	//	gClipList->clear();
	//}

	//void ClipsWidget::importAtlasEnd() {
	//	recreateList();
	//	update();
	//}

	//void ClipsWidget::loop() {
	//	if (mSelect && mSelect->size() > 0) {
	//		auto maxframe = mSelect->size();
	//		while (mCurrFrame > maxframe) {
	//			mCurrFrame -= maxframe;
	//		} mCurrFrame += mSpeed;
	//		
	//		/*auto curr = mSelect->at(safeFrame());
	//		mDuration += mSpeed;
	//		while (mDuration > curr.duration) {
	//			mDuration -= curr.duration;
	//			mCurrFrame++;
	//			if (mCurrFrame >= maxframe)
	//				mCurrFrame -= maxframe;
	//			curr = mSelect->at(safeFrame());
	//		}*/
	//	}

	//	update();
	//}

	//void ClipsWidget::updateTexture() {
	//	auto tex = gTextures.find(gTexKey);
	//	if (tex && mTexmat) {
	//		mTexmat->textures[0] = tex;
	//		update();
	//	}
	//}

	//uint ClipsWidget::safeFrame() {
	//	auto frame = floor(mCurrFrame);
	//	frame = clamp(frame, 0, mSelect->size() - 1);
	//	return frame;
	//}

	//void ClipsWidget::addFrame() {
	//	if (mSelect) {
	//		if (mSelect->size() > 0) {
	//			mSelect->push_back(mSelect->back());
	//		}
	//		else mSelect->push_back(gCells.at(0).name);
	//		recreateReel();
	//	}
	//}

	//void ClipsWidget::connectList() {
	//	connect(gClipList, &QListExtension::currentRowChanged, [&]() {
	//		mScene.makeCurrent();
	//		auto row = gClipList->currentRow();
	//		if (row >= 0) {
	//			mSelect = &gClips.at(row);
	//		} else {
	//			mSelect = 0;
	//		}
	//		setFocus();
	//		recreateReel();
	//	});

	//	connect(gClipList, &QWidget::customContextMenuRequested, [=](const QPoint& pos) {
	//		QMenu contextMenu(tr("Context menu"), this);
	//		auto CREATE_ACT = contextMenu.addAction("Create");
	//		auto DELETE_ACT = contextMenu.addAction("Delete");

	//		auto selected = contextMenu.exec(gClipList->mapToGlobal(pos));
	//		if (selected) {
	//			if (selected == CREATE_ACT) {
	//				string basename;
	//				tokenize(gTexKey, '/', [&](strview, strview tail) {
	//					tokenize(tail, '.', [&](strview head, strview) {
	//						basename = head;
	//						});
	//					});

	//				for (uint i = 0; i < gClips.count(); i++) {
	//					if (gClips.name(i) == basename) {
	//						basename += '_';
	//						i = 0;
	//					}
	//				}
	//				gClips.emplace(basename);
	//				auto item = new QListWidgetItem(QString::fromStdString(basename));
	//				item->setFlags(item->flags() | Qt::ItemIsEditable);
	//				gClipList->addItem(item);
	//				gClipList->setCurrentItem(item);
	//				mSelect = &gClips.at(basename);
	//			} else if (selected == DELETE_ACT) {
	//				auto row = gClipList->currentRow();
	//				auto item = gClipList->takeItem(row);
	//				if (item) 
	//					delete item;
	//				gClips.erase(row);
	//			}
	//		}
	//	});

	//	connect(gClipList->itemDelegate(), &QAbstractItemDelegate::commitData, [&](QWidget* pLineEdit) {
	//		auto new_name = reinterpret_cast<QLineEdit*>(pLineEdit)->text();
	//		for (uint i = 0; i < gClipList->count(); i++) {
	//			if (gClipList->currentRow() != i && gClipList->item(i)->text() == new_name) {
	//				new_name.push_back('_');
	//				i = 0;
	//			}
	//		}
	//		QString old_name = gClipList->item(gClipList->currentRow())->text();
	//		gClipList->item(gClipList->currentRow())->setText(new_name);
	//		gClips.rename(gClipList->currentRow(), new_name.toStdString());
	//		setFocus();
	//	});

	//	connect(gClipList->model(), &QAbstractItemModel::rowsMoved, [&](const QModelIndex&, int from, int, const QModelIndex&, int to) {
	//		if (from < to)
	//			to--;
	//		if (uint(to) >= gClipList->count())
	//			to = gClipList->count() - 1;
	//		if (from < 0)
	//			from = 0;
	//		gClips.swap(from, to);

	//		auto row = gClipList->currentRow();
	//		if (row >= 0) {
	//			mSelect = &gClips.at(row);
	//		} else {
	//			mSelect = 0;
	//		}
	//	});
	//}

	//void ClipsWidget::recreateList() {
	//	gClipList->clear();
	//	for (uint i = 0; i < gClips.count(); i++) {
	//		auto& clip = gClips.nameAt(i);
	//		auto item = new QListWidgetItem(QString::fromStdString(clip));
	//		item->setFlags(item->flags() | Qt::ItemIsEditable);
	//		gClipList->addItem(item);
	//	}
	//}

	//void ReelEvent::onGrab(uint buttonIndex, ButtonData& data) {
	//	if (buttonIndex == MouseSym::MIDDLE) {
	//		Entity e = data.select[buttonIndex];
	//		if (e != entt::null && gObjects->has<ReelFrame>(e)) {
	//			sig_Move.invoke(gObjects->get<ReelFrame>(e).index);
	//		}
	//	}
	//}

	//void ReelEvent::onRelease(uint buttonIndex, ButtonData& data) {
	//	Entity e;
	//	switch (buttonIndex) {
	//	case MouseSym::LEFT:
	//		e = data.select[buttonIndex];
	//		if (e != entt::null && gObjects->has<ReelFrame>(e)) {
	//			sig_Select.invoke(gObjects->get<ReelFrame>(e).index);
	//		}
	//		break;
	//	case MouseSym::RIGHT:
	//		e = data.select[buttonIndex];
	//		if (e != entt::null && gObjects->has<ReelFrame>(e)) {
	//			sig_Erase.invoke(gObjects->get<ReelFrame>(e).index);
	//		}
	//		break;
	//	}
	//}

};