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
				//aabb rect;
				//mClipSprite.sync(rect);
				//mViewShapes->line.batchAABB(rect, color8(255, 255, 255, 30));

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
				auto item = reinterpret_cast<ClipItem*>(gAtlsUtil.clipList->currentItem());
				if (item && item->clip && mClip.clip()) {
					item->clip->speed = (float)value;
					if (!mSuppressSpinbox)
						sig_Modified.invoke();
				}
			});
			connect(ui.repeatBox, &QCheckBox::toggled, [&](bool value) {
				auto item = reinterpret_cast<ClipItem*>(gAtlsUtil.clipList->currentItem());
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

				auto view = gProject.view<ClipframeHolder>();
				sizet i = 0;
				for (asset<ClipframeHolder> holder : view) {
					holder->canvas.batch();
					auto x = i * cReelFrameSize + cReelFrameSize;
					mReelShapes->line.batchline(line(x, 0, x, -cReelFrameSize), color8(255, 255, 255, 255));
					i++;
				}
				mReelPainter->paint();

				mReelShapes->line.batchline(line(0, -14, cReelFrameSize * i, -14), color8(255, 255, 255, 255));
				mReelShapes->fill.batchAABB(aabb(0, -14, cReelFrameSize * i, 0), color8(255, 255, 255, 80));
				if (mClipSprite.cell() != asset<Cell>() && view.size() > 0) {
					auto frame = mClip.frame();
					auto rect = aabb(frame * cReelFrameSize, -cReelFrameSize, frame * cReelFrameSize + cReelFrameSize, 0);
					rect.b = -14;
					mReelShapes->line.batchAABB(rect, color8(255, 0, 0, 255));
					mReelShapes->fill.batchAABB(rect, color8(255, 0, 0, 180));
				}

				for (asset<ClipframeHolder> holder : gProject.view<AtlasLastModifiedReelHolder>()) {
					auto rect = holder->canvas.bounds;
					auto color = gEditorColor.cell;
					color.a = 80.0f;
					mReelShapes->fill.batchAABB(rect, color);
				}

				if (mMoving) {
					auto rect = mMoving->canvas.bounds;
					auto color = gEditorColor.cellShadow;
					mReelShapes->line.batchAABB(rect, color);
					color.a = gEditorColor.cellFillAlpha;
					mReelShapes->fill.batchAABB(rect, color);

					if (mMovingSide > 0) {
						int fx = mMovingSide * cReelFrameSize;
						mReelShapes->line.batchline(line(fx, 0, fx, -cReelFrameSize), color8(255, 0, 0, 255));
						fx += 1;
						mReelShapes->line.batchline(line(fx, 0, fx, -cReelFrameSize), color8(255, 0, 0, 255));
					}

				} else if (mHovering) {
					color8 color = gEditorColor.cellHovered;

					switch (cursor().shape()) {
						case Qt::ArrowCursor:
							if (gMouse.state(1) == eInput::Hold || gMouse.state(1) == eInput::Once) {
								color = gEditorColor.cellSizing;
							} else if (gMouse.state(0) == eInput::Hold || gMouse.state(0) == eInput::Once) {
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
			}
		});

		ui.reel->sig_MousePress.connect([&]() {
			updateAllCanvasButton();
		});

		ui.reel->sig_MouseRelease.connect([&]() {
			updateAllCanvasButton();
			auto clip = mClip.clip();
			if (clip) {
				if (mMoving && mMovingSide >= 0) {
					auto index = mMoving->index;

					el_vector::swapshift(clip->cells, index, mMovingSide);
					recreateReel();
					mMoving = NullEntity;

					auto& view = mClip.clip()->cells;
					mHovering = (index < view.size()) ? view[index] : NullEntity;

					sig_Modified.invoke();
				}

				if (mRemovable) {
					auto index = mRemovable->index;
					auto& frames = clip->cells;
					assert(index < frames.size());

					frames.erase(frames.begin() + index);
					recreateReel();
					mRemovable = NullEntity;

					auto& view = mClip.clip()->cells;
					mHovering = (index < view.size()) ? view[index] : NullEntity;

					sig_Modified.invoke();
				}
			}
			});

		ui.reel->sig_MouseMove.connect(this, &ClipsWidget::onReelMouseMove);
		
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

	void ClipsWidget::onReelMouseMove() {
		mHovering = NullEntity;
		setCursor(Qt::ArrowCursor);
		updateAllCanvasButton();

		if (mMoving && mClip.clip()) {
			int fx = round(gMouse.currentPosition().x + ui.reel->width() / 2.0f + mReelCam->position().x);
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
	}

	void ClipsWidget::onHover(Entity self, Entity context) {
		assert(asset<ClipframeHolder>(self));
		mHovering = self;
		auto holder = asset<ClipframeHolder>(self);
		if (QApplication::queryKeyboardModifiers() & Qt::AltModifier)
			setCursor(Qt::OpenHandCursor);

		if (gMouse.state(0) == eInput::Once || gMouse.state(0) == eInput::Hold) {
			if (cursor().shape() == Qt::OpenHandCursor) {
				setCursor(Qt::ClosedHandCursor);
				if (gMouse.state(0) == eInput::Once)
					mMoving = self;
			}
		} else if (gMouse.state(0) == eInput::Lift) {
			if (cursor().shape() == Qt::ClosedHandCursor) {
				setCursor(Qt::OpenHandCursor);
				return;
			}

			if (cursor().shape() == Qt::OpenHandCursor)
				return;

			QDialog dialog(this);
			AtlasPalette palette(&dialog, true);
			palette.updateAtlas(gAtlsUtil.currentAtlas);
			palette.updateMaterial(gAtlsUtil.currentMaterial, gAtlsUtil.globalPalettePositon, gAtlsUtil.globalPaletteScale);

			palette.sig_Clicked.connect([&](asset<Cell> cell) {
				ClipItem* item = reinterpret_cast<ClipItem*>(gAtlsUtil.clipList->currentItem());
				auto clip = item->clip;
				auto& frames = clip->cells;
				assert(item && clip);
				assert(holder->index < frames.size());
				
				auto prev = frames.at(holder->index);
				if (prev != cell) {
					frames.at(holder->index) = cell;
					holder->reshape(clip);
					sig_Modified.invoke();
					ui.view->update();
				}
				gProject.clear<AtlasLastModifiedReelHolder>();
				holder.add<AtlasLastModifiedReelHolder>();

				dialog.close();
			});

			mPaused = true;
			dialog.exec();
			mPaused = false;
			gAtlsUtil.globalPalettePositon = palette.camPosition();
			gAtlsUtil.globalPaletteScale = palette.camScale();

		} else if (gMouse.state(1) == eInput::Lift) {
			mRemovable = self;
		}
	}

	void ClipsWidget::reorderClipsAccordingToList() {
		auto atlas = gAtlsUtil.currentAtlas;
		assert(gAtlsUtil.currentAtlas && gAtlsUtil.currentAtlas.has<AssetLoaded>());
		auto& list = gAtlsUtil.clipList;
		auto& meta = atlas.get<AtlasMeta>();

		meta.cliporder.clear();
		for (auto i = 0; i < list->count(); i++) {
			auto item = reinterpret_cast<ClipItem*>(list->item(i));
			meta.cliporder.emplace_back(item->clip);
			item->clip.get<SubAssetData>().index = i;
		}
	}

	void ClipsWidget::updateAllCanvasButton() {
		auto view = gProject.view<ClipframeHolder>();
		if (view.size() > 0) {
			auto pos = gMouse.currentPosition();
			pos.x += ui.reel->width() / 2.0f + mReelCam->position().x;
			pos.y -= ui.reel->height() / 2.0f; // Minused because of updateViewport in ui.reel->sig_Paint

			for (asset<ClipframeHolder> holder : view) {
				auto hit = holder->canvas.bounds.contains(pos);
				holder->button.update(holder, hit);
			}
		}
	}
	void ClipsWidget::loop() {
		if (isActiveWindow()) {
			if (mViewCam && mViewCamTween.progress() != 1.0f) {
				mViewCamTween.step(1);
				ui.view->update();
			}
			if (mReelCam && mReelCamTween.progress() != 1.0f) {
				mReelCamTween.step(1);
				onReelMouseMove();
				ui.reel->update();
			}
		}
	}


	void ClipsWidget::animLoop() {
		if (!mPaused) {
			if (mClip.clip())
				mClip.step(mClipSprite);
			ui.view->update();
		}
	}

	void ClipsWidget::createClip() {
		assert(gAtlsUtil.currentAtlas);

		if (gAtlsUtil.currentAtlas.has<AssetLoaded>()) {
			mSuppressSelect = true;
			auto& list = *gAtlsUtil.clipList;
			ClipItem* item = new ClipItem(&list);
			item->setFlags(item->flags() | Qt::ItemIsEditable);

			auto clipname = gAtlsUtil.currentAtlas.get<GUIAsset>().filePath.stem().generic_u8string();
			auto newname = list.getNoneConflictingName(QString::fromUtf8(clipname), false);
			item->setText(newname);
			clipname = newname.toUtf8();

			// CREATE CLIP
			auto& meta = gAtlsUtil.currentAtlas.get<AtlasMeta>();
			auto clip = item->clip = gProject.make<ClipMeta>().add<Clip>();
			clip.add<SubAssetData>(meta.cliporder.size(), clipname, item->clip);
			meta.cliporder.emplace_back(clip);
			gAtlsUtil.currentAtlas->clips.emplace(clipname, clip);

			list.addItem(item);
			mSuppressSelect = false;

			list.setCurrentItem(item);
			sig_Modified.invoke();
		}
	}

	void ClipsWidget::deleteClip() {
		assert(gAtlsUtil.currentAtlas);
		auto atlas = gAtlsUtil.currentAtlas;

		ClipItem* item = reinterpret_cast<ClipItem*>(gAtlsUtil.clipList->currentItem());
		if (item && item->clip && mClip.clip()) {
			auto& list = gAtlsUtil.clipList;
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
		if (gAtlsUtil.cellList->count() > 0 && mClip.clip()) {
			ClipItem* item = reinterpret_cast<ClipItem*>(gAtlsUtil.clipList->currentItem());
			if (item && item->clip) {
				asset<Cell> cell;
				auto& frames = item->clip->cells;
				auto size = frames.size();
				if (size > 0) {
					cell = frames.at(size - 1);
				} else if (gAtlsUtil.cellList->count() > 0) {
					cell = reinterpret_cast<CellItem*>(gAtlsUtil.cellList->item(0))->holder;
				}
				frames.emplace_back(cell);

				auto holder = gProject.make<ClipframeHolder>(gAtlsUtil.currentMaterial, mReelPainter, this);
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
		if (gAtlsUtil.currentAtlas.has<AssetLoaded>()) {
			mSuppressSelect = true;
			auto& list = *gAtlsUtil.clipList;
			auto row = list.currentRow();

			for (auto i = 0; i < list.count(); i++) {
				delete list.item(i);
			} list.clear();

			auto& clips = gAtlsUtil.currentAtlas.get<AtlasMeta>().cliporder;
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

	void ClipsWidget::recreateReel() {
		auto clip = mClip.clip();
		auto view = gProject.view<ClipframeHolder>();
		gProject.destroy(view.begin(), view.end());

		if (clip) {
			auto size = clip->cells.size();
			for (sizet i = 0; i < size; i++) {
				asset<ClipframeHolder> holder = gProject.make<ClipframeHolder>(gAtlsUtil.currentMaterial, mReelPainter, this);
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

		mClipSprite.material = gAtlsUtil.currentMaterial;
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
		connect(gAtlsUtil.clipList, &QListExtension::currentRowChanged, [&]() {
			if (isVisible() && !mSuppressSelect) {
				ClipItem* item = reinterpret_cast<ClipItem*>(gAtlsUtil.clipList->currentItem());
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

		connect(gAtlsUtil.clipList->model(), &QAbstractItemModel::rowsMoved, [&]() {
			if (isVisible() && !mSuppressSelect) {
				reorderClipsAccordingToList();
				sig_Modified.invoke();
			}
		});

		connect(gAtlsUtil.clipList->itemDelegate(), &QAbstractItemDelegate::commitData, [&](QWidget* pLineEdit) {
			auto atlas = gAtlsUtil.currentAtlas;
			if (isVisible() && atlas.has<AssetLoaded>()) {
				ClipItem* item = reinterpret_cast<ClipItem*>(gAtlsUtil.clipList->currentItem());

				auto& data = item->clip.get<SubAssetData>();
				auto name = gAtlsUtil.clipList->
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
		gAtlsUtil.clipList->show();
		recreateList();
		ui.reel->update();
		show();
	}

	void ClipsWidget::hideEditor() {
		gAtlsUtil.clipList->hide();
		ui.reel->update();
		hide();
	}
};