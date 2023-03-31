#include <elqtpch.h>
#include "clips_widget.h"

#include "../elqt/color_code.h"

#include "util.h"
#include <tools/project.h>
#include <common/algorithm.h>
#include <common/line.h>
#include <common/container.h>
#include <tools/clip.h>
#include <tools/painter.h>
#include <tools/material.h>
#include <tools/controls.h>
#include <tools/texture.h>
#include <tools/atlas.h>
#include <apparatus/ui.h>
#include <apparatus/asset_loader.h>

#include <common/shapestream.h>
#include <common/vecstream.h>

namespace el
{

	ClipframeHolder::ClipframeHolder(
		asset<Material> material, asset<Painter> painter, IButtonEvent* btnEvent
	) : canvas(material, painter), button(btnEvent) {
	}

	void ClipframeHolder::reshape(asset<Clip> clip) {
		assert(index < clip->cells.size());
		auto cell = clip->cells.at(index);
		if (cell)
			canvas.recalc(cell);
	}

#define cReelFrameSize 100

	void ClipframeHolder::reorder(sizet index_) {
		index = index_;
		canvas.bounds = aabb(index * cReelFrameSize, -cReelFrameSize, index * cReelFrameSize + cReelFrameSize, -15);
	}

	ClipsWidget::ClipsWidget(QWidget* parent) : mPaused(false), mClip(asset<Clip>()), mSuppressScroll(false), mSuppressSelect(false), mSuppressSpinbox(false) {
		ui.setupUi(this);
		ui.scroll->setEnabled(false);

		connect(ui.scroll, &QScrollBar::valueChanged, [&](int value) {
			if (mClip.clip() && !mSuppressScroll) {
				mReelCamTarget.toX(value);
				*mReelCam = mReelCamTarget;
				setupCameraTween(mReelCamTween);
				ui.reel->update();
			}
			});

		connectView();
		connectReel();
	}

	void ClipsWidget::connectView() {
		ui.view->setMouseTracking(true);

		ui.view->sig_Start.connect([&]() {
			glClearColor(0.2f, 0.2f, 0.5f, 1.0f);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			safeCreateViewObjects();
			connectList();
		});

		ui.view->sig_Paint.connect([&]() {
			auto clip = mClip.clip();
			if (clip && mClipSprite.cell() != asset<Cell>()) {
				auto width = ui.view->width();
				auto height = ui.view->height();
				updateViewport(-width / 2, width / 2, -height / 2, height / 2);

				mClipSprite.recalc(mClipPosition);
				mClipSprite.batch();

				mViewShapes->line.batchline(line(-1000000.0f, 0.0f, 1000000.0f, 0.0f), color8(255, 255, 255, 80));
				mViewShapes->line.batchline(line(0.0f, -1000000.0f, 0.0f, 1000000.0f), color8(255, 255, 255, 80));

				mViewShapes->draw();
				mViewPainter->paint();
				ui.reel->update();
			}
		});

		ui.view->sig_MousePress.connect([&]() {
			if (gMouse.state(2) == eInput::Once) {
				auto pos = *mViewCam * gMouse.currentPosition();
				auto w = size().width() / 2.0f;
				auto h = size().height() / 2.0f;
				aabb trapbox(-w, -h, w, h);
				trapbox.trap(pos);
				mViewCamTarget.to(vec3(pos, -1000.0f));
				tweenCameraInput(mViewCamTween, *mViewCam, mViewCamTarget);
			}
		});

		ui.view->sig_MouseMove.connect([&]() {
			setCursor(Qt::CursorShape::ArrowCursor);
		});

		ui.view->sig_ScrollWheel.connect([&]() {
			auto clip = mClip.clip();
			if (clip && zoomCamera(mViewCamTarget, false, 2.0f)) {
				tweenCameraInput(mViewCamTween, *mViewCam, mViewCamTarget);
			}
		});
	}

	void ClipsWidget::connectReel() {
		ui.reel->setMouseTracking(true);

		ui.reel->sig_Start.connect([&]() {
			glClearColor(0.2f, 0.2f, 0.5f, 1.0f);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			ui.speedBox->setEnabled(false);
			ui.repeatBox->setEnabled(false);
			connect(ui.speedBox, &QDoubleSpinBox::valueChanged, [&](double value) {
				auto item = reinterpret_cast<ClipItem*>(gAtlasUtil.clipList->currentItem());
				if (item && item->clip && mClip.clip()) {
					item->clip->speed = (float)value;
					if (!mSuppressSpinbox)
						sig_Modified.invoke();
				}
				});
			connect(ui.repeatBox, &QCheckBox::toggled, [&](bool value) {
				auto item = reinterpret_cast<ClipItem*>(gAtlasUtil.clipList->currentItem());
				if (item && item->clip && mClip.clip()) {
					item->clip->repeat = (float)value;
					if (!mSuppressSpinbox)
						sig_Modified.invoke();
				}
				});
			connect(ui.playButton, &QPushButton::clicked, [&]() {
				mClip.reset();
				});

			safeCreateFrameObjects();
			});

		ui.reel->sig_Paint.connect([&]() {
			auto clip = mClip.clip();
			if (clip) {
				updateViewport(0, ui.reel->width(), -ui.reel->height(), 0);

				// batch reel frame
				auto view = gProject.view<ClipframeHolder>();
				sizet i = 0;
				for (asset<ClipframeHolder> holder : view) {
					holder->canvas.batch();
					auto x = i * cReelFrameSize + cReelFrameSize;
					mReelShapes->line.batchline(line(x, 0, x, -cReelFrameSize), color8(255, 255, 255, 255));
					i++;
				}
				mReelShapes->line.batchline(line(0, -14, cReelFrameSize * i, -14), color8(255, 255, 255, 255));
				mReelShapes->fill.batchAABB(aabb(0, -14, cReelFrameSize * i, 0), color8(255, 255, 255, 80));
				if (mClipSprite.cell() != asset<Cell>() && view.size() > 0) {
					auto frame = mClip.frame();
					auto rect = aabb(frame * cReelFrameSize + 2, -cReelFrameSize + 2, frame * cReelFrameSize + cReelFrameSize - 2, -2);
					rect.b = -14 +2;
					mReelShapes->line.batchAABB(rect, color8(255, 0, 0, 255));
					mReelShapes->fill.batchAABB(rect, color8(255, 0, 0, 150));
				}

				for (asset<ClipframeHolder> holder : gProject.view<AtlasLastModifiedReelHolder>()) {
					auto rect = holder->canvas.bounds;
					auto color = gEditorColor.cell;
					color.a = 80.0f;
					mReelShapes->fill.batchAABB(rect, color);
				}

				if (mHeld) {
					auto pos = screenToReel();
					color8 color(255, 255, 255, 255);
					switch (mState) {
						case eState::Selecting:
							color = (mHeld->canvas.bounds.contains(pos)) ? gEditorColor.cellSelected : gEditorColor.cellHovered;
							break;
						case eState::Deleting:
							color = (mHeld->canvas.bounds.contains(pos)) ? gEditorColor.deleteRect : gEditorColor.cellHovered;
							break;
						case eState::Moving:
							color = gEditorColor.cellShadow;
							if (mMovingSide > 0) {
								int fx = mMovingSide * cReelFrameSize;
								mReelShapes->line.batchline(line(fx, 0, fx, -cReelFrameSize), color8(255, 0, 0, 255));
								fx += 1;
								mReelShapes->line.batchline(line(fx, 0, fx, -cReelFrameSize), color8(255, 0, 0, 255));
							}
							break;
					}

					auto rect = mHeld->canvas.bounds;
					mReelShapes->line.batchAABB(rect, color);
					color.a = gEditorColor.cellFillAlpha;
					mReelShapes->fill.batchAABB(rect, color);
				} else if (mHovering) {
					color8 color = (QApplication::queryKeyboardModifiers() & Qt::AltModifier) ? 
						gEditorColor.cellOpenHanded : gEditorColor.cellHovered;
					auto rect = mHovering->canvas.bounds;
					mReelShapes->line.batchAABB(rect, color);
					color.a = gEditorColor.cellFillAlpha;
					mReelShapes->fill.batchAABB(rect, color);
				}


				mReelPainter->paint();
				mReelShapes->draw();
			}
		});

		ui.reel->sig_MousePress.connect(this, &ClipsWidget::updateAllCanvasButton);
		ui.reel->sig_MouseRelease.connect(this, &ClipsWidget::updateAllCanvasButton);
		ui.reel->sig_MouseMove.connect(this, &ClipsWidget::updateAllCanvasButton);
		ui.reel->sig_ScrollWheel.connect([&]() {
			if (mClip.clip() && ui.scroll->isEnabled()) {
				float val = -30.0f;
				val *= gMouse.wheel();

				mSuppressScroll = true;
				ui.scroll->setValue((int)val + ui.scroll->value());
				mSuppressScroll = false;

				mReelCamTarget.toX((float)ui.scroll->value()); // clamped automatically by qt scroll
				tweenCameraInput(mReelCamTween, *mReelCam, mReelCamTarget);
				ui.reel->update();
			}
		});
	}

	void ClipsWidget::onHover(Entity self, Entity context) {
		assert(asset<ClipframeHolder>(self));
		mHovering = self;
		auto holder = asset<ClipframeHolder>(self);

		switch (gMouse.state(0)) {
			case eInput::Once:
				if (gMouse.state(1) != eInput::Hold) {
					mHeld = mHovering;
					if (QApplication::queryKeyboardModifiers() & Qt::AltModifier) {
						mState = eState::Moving;
					} else {
						mState = eState::Selecting;
					}
				}
				break;
		}

		if (gMouse.state(1) == eInput::Once) {
			if (gMouse.state(0) != eInput::Hold) {
				mHeld = mHovering;
				mState = eState::Deleting;
			}
		}
	}

	void ClipsWidget::postUpdate(Entity self, Entity context) {
		if (self != mHeld)
			return;

		switch (gMouse.state(0)) {
			case eInput::Hold:
				if (mClip.clip() && mState == eState::Moving) {
					int fx = round(screenToReel().x);
					fx = max(0, fx);
					int trail = fx % cReelFrameSize;
					auto max = mClip.clip()->cells.size();

					mMovingSide = 0;
					if (fx >= max * cReelFrameSize) {
						mMovingSide = max;
					} else {
						if (trail < 35) {
							mMovingSide = fx - trail;
							mMovingSide = mMovingSide / cReelFrameSize;
						} else if (trail > 65) {
							mMovingSide = fx - trail + cReelFrameSize;
							mMovingSide = mMovingSide / cReelFrameSize;
						} else if (fx < max * cReelFrameSize) {
							mMovingSide = -1;
						}
					}
				}
				break;
			case eInput::Lift:
				if (mState == eState::Moving) {
					if (mMovingSide >= 0 && mClip.clip()) {
						auto index = mHeld->index;
						el_vector::swapshift(mClip.clip()->cells, index, mMovingSide);
						recreateReel();

						auto& view = mClip.clip()->cells;
						mHovering = (index < view.size()) ? view[index] : NullEntity;

						sig_Modified.invoke();
					}
				} else if (mState == eState::Selecting) {
					if (mHeld->canvas.bounds.contains(screenToReel())) {
						QDialog dialog(this);
						AtlasPalette palette(&dialog, true);
						palette.updateAtlas(gAtlasUtil.currentAtlas);
						palette.updateMaterial(gAtlasUtil.currentMaterial, gAtlasUtil.globalPalettePositon, gAtlasUtil.globalPaletteScale);

						palette.sig_Clicked.connect([&](asset<Cell> cell) {
							ClipItem* item = reinterpret_cast<ClipItem*>(gAtlasUtil.clipList->currentItem());
							auto clip = item->clip;
							auto& frames = clip->cells;
							assert(item && clip);
							assert(mHeld->index < frames.size());

							auto prev = frames.at(mHeld->index);
							if (prev != cell) {
								frames.at(mHeld->index) = cell;
								mHeld->reshape(clip);
								sig_Modified.invoke();
								ui.view->update();
							}
							gProject.clear<AtlasLastModifiedReelHolder>();
							mHeld.add<AtlasLastModifiedReelHolder>();

							dialog.close();
							});
						mPaused = true;
						dialog.exec();
						mPaused = false;
						gAtlasUtil.globalPalettePositon = palette.camPosition();
						gAtlasUtil.globalPaletteScale = palette.camScale();
					}
				}

				if (mState != eState::Deleting) {
					mHeld = NullEntity;
					gMouse.reset();
					updateAllCanvasButton();
				}
				break;

		}

		if (gMouse.state(1) == eInput::Lift) {
			if (mState == eState::Deleting && mClip.clip()) {
				if (mHeld->canvas.bounds.contains(screenToReel())) {
					auto index = mHeld->index;
					auto& frames = mClip.clip()->cells;
					assert(index < frames.size());

					frames.erase(frames.begin() + index);
					recreateReel();

					auto& view = mClip.clip()->cells;
					mHovering = (index < view.size()) ? view[index] : NullEntity;
					sig_Modified.invoke();
				}

				mHeld = NullEntity;
				gMouse.reset();
				updateAllCanvasButton();
			}
		}
	}

	void ClipsWidget::reorderClipsAccordingToList() {
		auto atlas = gAtlasUtil.currentAtlas;
		assert(gAtlasUtil.currentAtlas && gAtlasUtil.currentAtlas.has<AssetLoaded>());
		auto& list = gAtlasUtil.clipList;
		auto& meta = atlas.get<AtlasMeta>();

		meta.cliporder.clear();
		for (auto i = 0; i < list->count(); i++) {
			auto item = reinterpret_cast<ClipItem*>(list->item(i));
			meta.cliporder.emplace_back(item->clip);
			item->clip.get<SubAssetData>().index = i;
		}
	}

	void ClipsWidget::updateAllCanvasButton() {
		mHovering = NullEntity;
		auto view = gProject.view<ClipframeHolder>();
		if (view.size() > 0) {
			auto pos = gMouse.currentPosition();
			pos.x += ui.reel->width() / 2.0f + mReelCam->position().x;
			pos.y -= ui.reel->height() / 2.0f; // Minused because of updateViewport in ui.reel->sig_Paint

			for (asset<ClipframeHolder> holder : view) {
				auto hit = holder->canvas.bounds.contains(pos);
				holder->button.update(holder, hit);
			}
		} updateCursor();
	}

	void ClipsWidget::loop() {
		if (isActiveWindow()) {
			if (mViewCam && mViewCamTween.progress() != 1.0f) {
				mViewCamTween.step(1);
				ui.view->update();
			}

			if (mReelCam) {
				static bool prevUnder = ui.reel->underMouse();
				auto currUnder = ui.reel->underMouse();

				if (currUnder == false && prevUnder == true) {
					onMouseReelExit();
				}

				prevUnder = ui.reel->underMouse();

				if (mReelCamTween.progress() != 1.0f) {
					mReelCamTween.step(1);
					updateAllCanvasButton();
					ui.reel->update();
					if (mReelCamTween.progress() == 1.0f && !ui.reel->underMouse()) {
						onMouseReelExit();
					}
				}
			}
		}
	}

	void ClipsWidget::onMouseReelExit() {
		gMouse.reset();
		mHovering = NullEntity;
		if (mState != eState::Selecting)
			mHeld = NullEntity;
		updateCursor();
		ui.reel->update();
	}

	void ClipsWidget::animLoop() {
		if (!mPaused) {
			if (mClip.clip())
				mClip.step(mClipSprite);
			ui.view->update();
		}
	}

	void ClipsWidget::createClip() {
		assert(gAtlasUtil.currentAtlas);

		if (gAtlasUtil.currentAtlas.has<AssetLoaded>()) {
			mSuppressSelect = true;
			auto& list = *gAtlasUtil.clipList;
			ClipItem* item = new ClipItem(&list);
			item->setFlags(item->flags() | Qt::ItemIsEditable);

			auto clipname = gAtlasUtil.currentAtlas.get<GUIAsset>().filePath.stem().generic_u8string();
			auto newname = list.getNoneConflictingName(QString::fromUtf8(clipname), false);
			item->setText(newname);
			clipname = newname.toUtf8();

			// CREATE CLIP
			auto& meta = gAtlasUtil.currentAtlas.get<AtlasMeta>();
			auto clip = item->clip = gProject.make<ClipMeta>().add<Clip>();
			clip.add<SubAssetData>(meta.cliporder.size(), clipname, item->clip);
			meta.cliporder.emplace_back(clip);
			gAtlasUtil.currentAtlas->clips.emplace(clipname, clip);

			list.addItem(item);
			mSuppressSelect = false;

			list.setCurrentItem(item);
			sig_Modified.invoke();
		}
	}

	void ClipsWidget::deleteClip() {
		assert(gAtlasUtil.currentAtlas);
		auto atlas = gAtlasUtil.currentAtlas;

		ClipItem* item = reinterpret_cast<ClipItem*>(gAtlasUtil.clipList->currentItem());
		if (item && item->clip && mClip.clip()) {
			auto& list = gAtlasUtil.clipList;
			auto row = list->currentRow();
			auto clip = item->clip;
			auto& data = clip.get<SubAssetData>();
			auto& meta = atlas.get<AtlasMeta>();

			clip->cells.clear();
			atlas->clips.erase(data.name);
			clip.destroy();

			mSuppressSelect = true;
			delete item;
			mSuppressSelect = false;

			reorderClipsAccordingToList();
			row = clamp(row, 0, list->count() - 1);
			list->setCurrentRow(-1);
			list->setCurrentRow(row);

			sig_Modified.invoke();
		}
	}

	void ClipsWidget::addFrame() {
		if (gAtlasUtil.cellList->count() > 0 && mClip.clip()) {
			ClipItem* item = reinterpret_cast<ClipItem*>(gAtlasUtil.clipList->currentItem());
			if (item && item->clip) {
				asset<Cell> cell;
				auto& frames = item->clip->cells;
				auto size = frames.size();
				if (size > 0) {
					cell = frames.at(size - 1);
				} else if (gAtlasUtil.cellList->count() > 0) {
					cell = reinterpret_cast<CellItem*>(gAtlasUtil.cellList->item(0))->holder;
				}
				frames.emplace_back(cell);

				auto holder = gProject.make<ClipframeHolder>(gAtlasUtil.currentMaterial, mReelPainter, this);
				holder->reorder(size);
				holder->reshape(item->clip);

				syncScroll();
				ui.reel->update();
				sig_Modified.invoke();
			}
		}
	}

	void ClipsWidget::updateOnAtlasLoad() {
		recreateList();
	}

	void ClipsWidget::recreateList() {
		if (gAtlasUtil.currentAtlas.has<AssetLoaded>()) {
			mSuppressSelect = true;
			auto& list = *gAtlasUtil.clipList;
			auto row = list.currentRow();

			for (auto i = 0; i < list.count(); i++) {
				delete list.item(i);
			} list.clear();

			auto& clips = gAtlasUtil.currentAtlas.get<AtlasMeta>().cliporder;
			for (sizet i = 0; i < clips.size(); i++) {
				auto data = asset<SubAssetData>(clips[i]);
				ClipItem* item = new ClipItem(&list);
				item->setFlags(item->flags() | Qt::ItemIsEditable);

				item->clip = data;
				item->setText(QString::fromUtf8(data->name));
				list.addItem(item);
			}
			mSuppressSelect = false;

			list.setCurrentRow(row);
			recreateReel();
		}
	}


	void ClipsWidget::onKeyPress(QKeyEvent* e) {
		if (e->key() == Qt::Key::Key_Alt) {
			updateCursor();
			ui.reel->update();
		}
	}

	void ClipsWidget::onKeyRelease(QKeyEvent* e) {
		if (e->key() == Qt::Key::Key_Alt) {
			updateCursor();
			ui.reel->update();
		}
	}

	void ClipsWidget::recreateReel() {
		auto clip = mClip.clip();
		auto view = gProject.view<ClipframeHolder>();
		gProject.destroy(view.begin(), view.end());

		if (clip) {
			auto size = clip->cells.size();
			for (sizet i = 0; i < size; i++) {
				asset<ClipframeHolder> holder = gProject.make<ClipframeHolder>(gAtlasUtil.currentMaterial, mReelPainter, this);
				holder->reorder(i);
				holder->reshape(clip);
			}
		}

		syncScroll();
		ui.reel->update();
	}


	void ClipsWidget::syncScroll() {
		auto clip = mClip.clip();
		if (clip) {
			auto length = clip->cells.size() * cReelFrameSize;
			if (ui.reel->width() < length) {
				ui.scroll->setEnabled(true);
				ui.scroll->setRange(0, length - ui.reel->width());
			} else {
				ui.scroll->setValue(0);
				ui.scroll->setEnabled(false);
			}
		}
	}

	void ClipsWidget::updateCursor() {
		setCursor(Qt::CursorShape::ArrowCursor);
		if (mHeld) {
			if (mState == eState::Moving)
				setCursor(Qt::CursorShape::ClosedHandCursor);
			else setCursor(Qt::CursorShape::PointingHandCursor);
		} else if (mHovering) {
			if (QApplication::queryKeyboardModifiers() & Qt::AltModifier) {
				setCursor(Qt::CursorShape::OpenHandCursor);
			} else setCursor(Qt::CursorShape::PointingHandCursor);
		}
	}

	vec2 ClipsWidget::screenToReel() {
		auto pos = *mReelCam * gMouse.currentPosition();
		pos.x += ui.reel->width() / 2.0f;
		pos.y -= ui.reel->height() / 2.0f;
		return pos;
	}

	void ClipsWidget::recenterCamera() {
		mViewCamTarget.to(0.0f, 0.0f, 0.0f);
		tweenCameraInput(mViewCamTween, *mViewCam, mViewCamTarget);
	}

	void ClipsWidget::safeCreateViewObjects() {
		if (!mViewCam) {
			ui.view->makeCurrent();
			mViewCam = gProject.make<Camera>().add<EditorAsset>();
			mViewCamTarget.to(vec3(0.0f, 0.0f, -1000.0f));

			mViewPainter = gProject.make<Painter>("__el_editor_/shader/basic_sprite.vert", "__el_editor_/shader/texture_uv.frag",
				32, mViewCam, Projection::eOrtho,
				ePainterFlags::DEPTH_SORT | ePainterFlags::MULTI_MATERIAL | ePainterFlags::Z_CLEAR).add<EditorAsset>();
			mViewPainter->init();

			mViewShapes = new ShapeDebug2d;
			mViewShapes->init(mViewCam);
			*mViewCam = mViewCamTarget;
			setupCameraTween(mViewCamTween);
		}

		mClipSprite.material = gAtlasUtil.currentMaterial;
		mClipSprite.painter = mViewPainter;
	}

	void ClipsWidget::safeCreateFrameObjects() {
		if (!mReelCam) {
			ui.reel->makeCurrent();
			mReelCam = gProject.make<Camera>().add<EditorAsset>();
			mReelCamTarget.to(vec3(0.0f, 0.0f, -1000.0f));

			mReelPainter = gProject.make<Painter>("__el_editor_/shader/basic_sprite.vert", "__el_editor_/shader/texture_uv.frag",
				3000, mReelCam, Projection::eOrtho,
				ePainterFlags::DEPTH_SORT | ePainterFlags::MULTI_MATERIAL | ePainterFlags::Z_CLEAR).add<EditorAsset>();
			mReelPainter->init();

			mReelShapes = new ShapeDebug2d;
			mReelShapes->init(mReelCam);

			*mReelCam = mReelCamTarget;
			setupCameraTween(mReelCamTween);
		}
	}

	void ClipsWidget::connectList() {
		connect(gAtlasUtil.clipList, &QListExtension::currentRowChanged, [&]() {
			if (isVisible() && !mSuppressSelect) {
				ClipItem* item = reinterpret_cast<ClipItem*>(gAtlasUtil.clipList->currentItem());
				if (item && item->clip) {
					mClip.setClip(item->clip);

					ui.speedBox->setEnabled(true);
					ui.repeatBox->setEnabled(true);
					ui.playButton->setEnabled(true);

					mSuppressSpinbox = true;
					ui.speedBox->setValue(item->clip->speed);
					ui.repeatBox->setChecked(item->clip->repeat);
					mSuppressSpinbox = false;

					recreateReel();
				} else {
					mClip.setClip(asset<Clip>());

					ui.speedBox->setEnabled(false);
					ui.repeatBox->setEnabled(false);
					ui.playButton->setEnabled(false);

					mSuppressSpinbox = true;
					ui.speedBox->setValue(0);
					ui.repeatBox->setChecked(false);
					mSuppressSpinbox = false;

					recreateReel();
				}
			}
			});

		connect(gAtlasUtil.clipList->model(), &QAbstractItemModel::rowsMoved, [&]() {
			if (isVisible() && !mSuppressSelect) {
				reorderClipsAccordingToList();
				sig_Modified.invoke();
			}
			});

		connect(gAtlasUtil.clipList->itemDelegate(), &QAbstractItemDelegate::commitData, [&](QWidget* pLineEdit) {
			auto atlas = gAtlasUtil.currentAtlas;
			if (isVisible() && atlas.has<AssetLoaded>()) {
				ClipItem* item = reinterpret_cast<ClipItem*>(gAtlasUtil.clipList->currentItem());

				auto& data = item->clip.get<SubAssetData>();
				auto name = gAtlasUtil.clipList->
					getNoneConflictingName(reinterpret_cast<QLineEdit*>(pLineEdit)->text()).toStdString();

				assert(atlas->clips.contains(data.name));
				if (data.name != name && atlas->clips.contains(data.name)) {
					atlas->clips.erase(data.name);
					atlas->clips[name] = item->clip;
					atlas.get<AtlasMeta>().clipnames[item->clip] = name;
					item->setText(QString::fromStdString(name));
					data.name = name;
					sig_Modified.invoke();
				}
			}
		});
	}

	void ClipsWidget::showEditor() {
		gAtlasUtil.clipList->show();
		recreateList();
		ui.reel->update();
		show();
	}

	void ClipsWidget::hideEditor() {
		gAtlasUtil.clipList->hide();
		ui.reel->update();
		hide();
	}
};