#include "clips_widget.h"
#include "../elqt/color_code.h"

namespace el
{

	ClipsWidget::ClipsWidget(QWidget* parent) : mPaused(false) {
		ui.setupUi(this);
		ui.scroll->setEnabled(false);

		ui.reel->setMouseTracking(true);
		
		ui.view->sig_Start.connect([&]() {
			glClearColor(0.2f, 0.2f, 0.5f, 1.0f);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			safeCreateViewObjects();
			connectList();
		});

		ui.reel->sig_Start.connect([&]() {
			glClearColor(0.2f, 0.2f, 0.5f, 1.0f);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			safeCreateFrameObjects();
		});

		ui.view->sig_Paint.connect([&]() {
			auto item = reinterpret_cast<ClipItem*>(gAtlsUtil.clipList->currentItem());

			if (item && item->clip) {
				if (mClipObj && mClipObj->cell() != asset<Cell>()) {
					mClipObj->update(mClipObj);
					mClipObj->batch();
					mViewPainter->paint();
					ui.reel->update();
				}
			}
		});


		ui.view->sig_ScrollWheel.connect([&]() {
			float val = (5.0f / 6.0f);
			val = pow(val, gMouse.wheel());
			if (val != 0) {
				val = (val > 0) ? val : -1.0f / val; // scroll up: 5/6, down: 6/5

				auto mpos = *mViewCam * gMouse.currentPosition();
				auto cpos = mViewCam->position();
				auto delta = mpos - cpos;
				delta *= (1 - val);

				mViewCam->scale(vec3(val, val, 1));
				//mViewCam->move(vec3(delta, 0));
			}
		});

		ui.reel->sig_Paint.connect([&]() {
			updateViewport(0, ui.reel->width(), -ui.reel->height(), 0);

			auto view = gStage->view<ClipframeHolder>();
			for (obj<ClipframeHolder> holder : view) {
				holder->canvas.batch();
			}
			mReelPainter->paint();

			if (mClipObj && mClipObj->cell() != asset<Cell>() && view.size() > 0) {
				auto frame = mClipObj.get<EditorProjectClipAnimation>().frame();
				auto rect = aabb(frame * cReelFrameSize, -cReelFrameSize, frame * cReelFrameSize + cReelFrameSize, 0);
				rect.b = -14;
				mReelShapes->line.batchAABB(rect, color8(255, 0, 0, 255));
				mReelShapes->fill.batchAABB(rect, color8(255, 0, 0, 180));
			}

			if (mMoving) {
				auto rect = mMoving->canvas.bounds;
				auto color = gEditorColor.cellShadow;
				mReelShapes->line.batchAABB(rect, color);
				color.a = gEditorColor.cellFillAlpha;
				mReelShapes->fill.batchAABB(rect, color);

				if (mMovingSide > 0) {
					int fx = mMovingSide * cReelFrameSize;
					mReelShapes->line.batchline(line(fx, 0, fx, -cReelFrameSize), color8(255, 255, 255, 255));
					fx += 1;
					mReelShapes->line.batchline(line(fx, 0, fx, -cReelFrameSize), color8(255, 255, 255, 255));
				}

			} else if (mHovering) {
				color8 color = gEditorColor.cellHovered;

				switch (cursor().shape()) {
				case Qt::ArrowCursor:
					if (gMouse.state(1) == eInput::HOLD || gMouse.state(1) == eInput::ONCE) {
						color = gEditorColor.cellSizing;
					} else if (gMouse.state(0) == eInput::HOLD || gMouse.state(0) == eInput::ONCE) {
						color = gEditorColor.cellSelected;
					}
					break;
				case Qt::OpenHandCursor:
					color = gEditorColor.cellOpenHanded;
					break;
				}

				auto rect = mHovering->canvas.bounds;
				//rect.t = -15;
				mReelShapes->line.batchAABB(rect, color);
				color.a = gEditorColor.cellFillAlpha;
				mReelShapes->fill.batchAABB(rect, color);
			}

			mReelShapes->draw();
		});

		ui.reel->sig_MousePress.connect([&]() {
			updateAllCanvasButton();
			});
		ui.reel->sig_MouseRelease.connect([&]() {
			updateAllCanvasButton();
			if (mMoving && mMovingSide >= 0) {
				ClipItem* item = reinterpret_cast<ClipItem*>(gAtlsUtil.clipList->currentItem());
				assert(item && item->clip);
				auto clip = item->clip;
				auto index = mMoving->index;

				swapshift(*clip, index, mMovingSide);
				recreateReel(clip);
				mMoving = NullEntity;

				auto view = gStage->view<ClipframeHolder>();
				mHovering = (index < view.size()) ? view[index] : NullEntity;
			}

			if (mRemovable) {
				ClipItem* item = reinterpret_cast<ClipItem*>(gAtlsUtil.clipList->currentItem());
				assert(item && item->clip);

				auto clip = item->clip;
				auto index = mRemovable->index;
				assert(index < clip->size());

				clip->erase(clip->begin() + index);
				recreateReel(clip);
				mRemovable = NullEntity;

				auto view = gStage->view<ClipframeHolder>();
				mHovering = (index < view.size()) ? view[index] : NullEntity;
			}
		});
		ui.reel->sig_MouseMove.connect([&]() {
			mHovering = NullEntity; 
			setCursor(Qt::ArrowCursor);
			updateAllCanvasButton();

			if (mMoving) {
				int fx = round(gMouse.currentPosition().x + ui.reel->width() / 2.0f + mReelCam->position().x);
				fx = max(0, fx);
				int trail = fx % cReelFrameSize;
				auto max = gStage->view<ClipframeHolder>().size();


				mMovingSide = 0;
				if (fx >= max * cReelFrameSize) {
					mMovingSide = max;
				} else {
					if (trail < 35) {
						mMovingSide = fx - trail;
						mMovingSide = mMovingSide / cReelFrameSize;
					}
					else if (trail > 65) {
						mMovingSide = fx - trail + cReelFrameSize;
						mMovingSide = mMovingSide / cReelFrameSize;
					}
					else if (fx < max * cReelFrameSize) {
						mMovingSide = -1;
					}
				}
			}
		});
		ui.reel->sig_ScrollWheel.connect([&]() {
			if (ui.scroll->isEnabled()) {
				float val = -30.0f;
				val *= gMouse.wheel();
				ui.scroll->setValue((int)val + ui.scroll->value());
			}
		}); 

		connect(ui.scroll, &QScrollBar::valueChanged, [&](int value) {
			ui.reel->bindStage();
			mReelCam->setPosX(value);
			ui.reel->update();
		});

		crng.seed(SEED()());
	}

	void ClipsWidget::onHover(Entity self, Entity context) {
		assert(obj<ClipframeHolder>(self));
		mHovering = self;
		auto holder = obj<ClipframeHolder>(self);

		if (QApplication::queryKeyboardModifiers() & Qt::AltModifier)
			setCursor(Qt::OpenHandCursor);
		
		if (gMouse.state(0) == eInput::ONCE || gMouse.state(0) == eInput::HOLD) {
			if (cursor().shape() == Qt::OpenHandCursor) {
				setCursor(Qt::ClosedHandCursor);
				if (gMouse.state(0) == eInput::ONCE)
					mMoving = self;
			}
		}
		else if (gMouse.state(0) == eInput::LIFT) {
			if (cursor().shape() == Qt::ClosedHandCursor) {
				setCursor(Qt::OpenHandCursor);
				return;
			}

			if (cursor().shape() == Qt::OpenHandCursor)
				return;

			auto prev = gStage;
			Stage stage;
			QDialog dialog(this);
			AtlasPalette palette(&dialog, true);
			palette.view()->setStage(&stage);

			palette.view()->sig_Start.connect([&]() {
				palette.updateMaterial(gAtlsUtil.currentMaterial);
				});

			palette.sig_Clicked.connect([&](CellResult h) {
				bind(prev);
				ClipItem* item = reinterpret_cast<ClipItem*>(gAtlsUtil.clipList->currentItem());
				assert(item && item->clip);
				assert(holder->index < item->clip->size());

				item->clip->at(holder->index) = h.cell;
				holder->reshape(item->clip);

				ui.reel->update();
				bind(&stage);
				dialog.close();
				});

			dialog.exec();
		}
		else if (gMouse.state(1) == eInput::LIFT) {
			mRemovable = self;
		}
	}

	void ClipsWidget::loop() {
		if (mClipObj)
			mClipObj.get<EditorProjectClipAnimation>().update(mClipObj);
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
			list.setCurrentItem(item);
		}
	}

	void ClipsWidget::removeClip() {
		if (mTexture && mTexture->atlas) {
			ClipItem* item = reinterpret_cast<ClipItem*>(gAtlsUtil.clipList->currentItem());
			if (item && item->clip) {
				auto clip = item->clip;
				assert(mTexture->atlas->clips.contains(clip));
				mTexture->atlas->clips.erase(clip);
				clip.destroy();

				auto view = gStage->view<ClipframeHolder>();
				gStage->destroy(view.begin(), view.end());
				delete item;
			}
		}
	}

	void ClipsWidget::addFrame() {
		if (gAtlsUtil.cellList->count() > 0) {
			ClipItem* item = reinterpret_cast<ClipItem*>(gAtlsUtil.clipList->currentItem());
			if (item && item->clip) {
				asset<Cell> cell;
				if (item->clip->size() > 0) {
					cell = item->clip->at(item->clip->size() - 1);
				} else if (gAtlsUtil.cellList->count() > 0) {
					cell = reinterpret_cast<CellItem*>(gAtlsUtil.cellList->item(0))->holder->cell;
				}
			
				item->clip->emplace_back(cell);
				recreateReel(item->clip);
			}
		}
	}

	void ClipsWidget::onKeyPress(QKeyEvent* e) {
		if (e->key() == Qt::Key::Key_Alt) {
			if (cursor().shape() == Qt::ArrowCursor)
				setCursor(Qt::OpenHandCursor);
			ui.reel->update();
		}
	}

	void ClipsWidget::onKeyRelease(QKeyEvent* e) {
		if (e->key() == Qt::Key::Key_Alt) {
			setCursor(Qt::ArrowCursor);
			ui.reel->update();
		}
	}

	void ClipsWidget::addReel(asset<Clip> clip) {
		auto holder = gStage->make<ClipframeHolder>(gAtlsUtil.currentMaterial, mReelPainter);
		holder.add<Button>(this);
		holder->reorder(clip->size() - 1);
		holder->reshape(clip);
	}

	void ClipsWidget::recreateReel(asset<Clip> clip) {
		auto currsize = gStage->view<ClipframeHolder>().size();
		auto destsize = clip->size();

		if (destsize > currsize) {
			for (auto i = 0; i < destsize - currsize; i++) {
				auto holder = gStage->make<ClipframeHolder>(gAtlsUtil.currentMaterial, mReelPainter);
				holder.add<Button>(this);
			}
		} else if (currsize > destsize) {
			auto view = gStage->view<ClipframeHolder>();
			gStage->destroy(view.begin() + destsize, view.end());
		}

		sizet i = 0;
		for (obj<ClipframeHolder> holder : gStage->view<ClipframeHolder>()) {
			holder->reorder(i);
			holder->reshape(clip);
			i++;
		}

		syncScroll();
		ui.reel->update();
	}


	void ClipsWidget::syncScroll() {
		 auto length = gStage->view<ClipframeHolder>().size() * cReelFrameSize;
		if (ui.reel->width() < length) {
			ui.scroll->setEnabled(true);
			ui.scroll->setRange(0, length - ui.reel->width());
		} else {
			ui.scroll->setValue(0);
			ui.scroll->setEnabled(false);
		}
	}

	void ClipsWidget::updateAllCanvasButton() {
		auto pos = gMouse.currentPosition();
		pos.x += ui.reel->width() / 2.0f + mReelCam->position().x;
		pos.y -= ui.reel->height() / 2.0f; // because of updateViewport in ui.reel->sig_Paint

		auto& stage = *gStage;
		for (auto button : stage.view<ClipframeHolder>()) {
			assert(stage.valid(button) && stage.all_of<ClipframeHolder>(button));
			bool hit = stage.get<ClipframeHolder>(button).canvas.bounds.contains(pos);
			stage.get<Button>(button).update(button, hit);
		}
	}

	void ClipsWidget::safeCreateViewObjects() {
		if (!mViewCam) {
			ui.view->makeCurrent();
			mViewCam = gProject->makeSub<EditorCamera>();
			mViewCam->to(vec3(0.0f, 0.0f, -1000.0f));

			mViewPainter = gProject->makeSub<EditorProjectPainter>("basic_sprite", "texture_uv", 32, mViewCam, Projection::eOrtho,
				ePainterFlags::DEPTH_SORT | ePainterFlags::MULTI_MATERIAL | ePainterFlags::Z_CLEAR);
			mViewPainter->init();

			mViewShapes = new EditorShapeDebug;
			mViewShapes->init(mViewCam);
		}

		if (!mClipObj) {
			mClipObj = gStage->make<EditorProjectSprite>(gAtlsUtil.currentMaterial, mViewPainter, "");
			mClipObj.add<Position>();
			mClipObj.add<EditorProjectClipAnimation>(asset<ClipImpl<1>>());
		}
	}

	void ClipsWidget::safeCreateFrameObjects() {
		if (!mReelCam) {
			ui.reel->makeCurrent();
			mReelCam = gProject->makeSub<EditorCamera>();
			mReelCam->to(vec3(0.0f, 0.0f, -1000.0f));

			mReelPainter = gProject->makeSub<EditorProjectPainter>("basic_sprite", "texture_uv", 3000, mReelCam, Projection::eOrtho,
				ePainterFlags::DEPTH_SORT | ePainterFlags::MULTI_MATERIAL | ePainterFlags::Z_CLEAR);
			mReelPainter->init();

			//hmm.. will i need this?
			mReelShapes = new EditorShapeDebug;
			mReelShapes->init(mReelCam);
		}
	}

	void ClipsWidget::updateTexture() {
		clearAllViews();
		auto mat = gAtlsUtil.currentMaterial;
		if (mat && mat->hasTexture()) {
			mTexture = mat->textures[0];
			if (mTexture->atlas) {
				for (auto it : mTexture->atlas->clips) {
					ClipItem* item = new ClipItem(gAtlsUtil.clipList);
					item->setText(QString::fromUtf8(it.first));
					item->clip = it.second;
					gAtlsUtil.clipList->addItem(item);
				}
			}
		} ui.reel->update();
	}

	void ClipsWidget::connectList() {
		connect(gAtlsUtil.clipList, &QListExtension::currentRowChanged, [&]() {
			assert(mClipObj);

			auto view = gStage->view<ClipframeHolder>();
			gStage->destroy(view.begin(), view.end());

			ClipItem* item = reinterpret_cast<ClipItem*>(gAtlsUtil.clipList->currentItem());
			//assert(item && item->clip);

			if (item && item->clip) {
				recreateReel(item->clip);
				mClipObj.get<EditorProjectClipAnimation>().setClip(item->clip);
			}
			syncScroll();
		});
	}

	void ClipsWidget::clearAllViews() {
		auto view = gStage->view<ClipframeHolder>();
		gStage->destroy(view.begin(), view.end());

		auto& list = *gAtlsUtil.clipList;
		list.clearSelection();
		list.setCurrentRow(-1);

		for (sizet i = 0; i < list.count(); i++) {
			delete list.item(i);
		}

		list.clear(); // not necessary but okay
	}

	void ClipsWidget::showEditor() {
		gAtlsUtil.clipList->show();
		show();
		ui.reel->update();
	}

	void ClipsWidget::hideEditor() {
		gAtlsUtil.clipList->hide();
		hide();
		ui.reel->update();
	}
};