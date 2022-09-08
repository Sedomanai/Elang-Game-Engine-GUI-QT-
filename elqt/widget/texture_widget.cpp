#include <elqtpch.h>
#include "texture_widget.h"

#include <apparatus/ui.h>
#include <apparatus/asset_loader.h>
#include <tools/asset_database.h>
#include <tools/painter.h>
#include <tools/project.h>
#include <tools/controls.h>
#include <tools/material.h>
#include <tools/texture.h>
#include <tools/atlas.h>

namespace el
{
	//QElangViewSignaled* QElangTextureWidget::view(){ return ui.view; }

	QElangTextureWidget::QElangTextureWidget(QWidget* parent, bool internalLoop)
		: QWidget(parent), mMoveCursor(Qt::SizeAllCursor), mSuppressScroll(false), mMovingScreen(false), mTextureBox(0, 0, 0, 0), mMainCamBox(0, 0, 0, 0)
	{
		ui.setupUi(this);

		// test debug
		if (internalLoop) {
			QTimer* timer = new QTimer(this);
			connect(timer, &QTimer::timeout, [&]() {
				loop();
				});
			timer->start(1000.0f / 60.0f);
		}
		//

		ui.view->sig_Start.connect([&]() {
			connectMouseInput();

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			glClearColor(0.2f, 0.3f, 0.2f, 1.0f);
			if (!mPainter) {
				mPainter = gProject.make<Painter>("__el_editor_/shader/basic_sprite.vert", "__el_editor_/shader/texture_uv.frag",
					100000, mMainCam, Projection::eOrtho,
					ePainterFlags::DEPTH_SORT | ePainterFlags::MULTI_MATERIAL | ePainterFlags::Z_CLEAR).add<EditorAsset>();
				mPainter->init();
			}

			safeCreateObjects();
		});

		
		ui.view->sig_Resize.connect([&](int w, int h) {
			syncCameraTarget();
			snapCamera();
			syncScrollBars();
		});

		ui.view->sig_Paint.connect([&]() {
			glClearDepth(1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			mSprite.batch();
			mPainter->paint();
		});

		connect(ui.hori, &QScrollBar::valueChanged, [&](int value) {
			if (!mSuppressScroll) {
				mMainCamTarget.toX(value + mMainCamBox.width() / 2);
				snapCamera();
				ui.view->update();
			}
		});
		connect(ui.verti, &QScrollBar::valueChanged, [&](int value) {
			if (!mSuppressScroll) {
				mMainCamTarget.toY(-value - mMainCamBox.height() /2);
				snapCamera();
				ui.view->update();
			}
		});
	}

	vec2 QElangTextureWidget::camPosition() { if (mMainCam) return vec2(mMainCamTarget.position()); return vec2(-65332.0f, 65332.0f); }
	float QElangTextureWidget::camScale() { if (mMainCam) return mMainCamTarget.scale().x; return 1.0f; }

	void QElangTextureWidget::release() {
		if (mMainCam) {
			ui.view->makeCurrent();
			mMainCam.destroy();
			mPainter.destroy();
		}
	}

	void QElangTextureWidget::safeCreateObjects() {
		ui.view->makeCurrent();
		if (!mMainCam) {
			mMainCam = gProject.make<Camera>().add<EditorAsset>();
			mMainCam->to(vec3(0.0f, 0.0f, -1000.0f));
			if (mPainter)
				mPainter->camera = mMainCam; 
			snapCamera();
		}

		if (!mSprite.painter) {
			mSprite.painter = mPainter;
			mSpritePosition = vec2(0, 0);
		}
	}

	void QElangTextureWidget::connectMouseInput() {
		ui.view->sig_MousePress.connect([&]() {
			if (gMouse.state(1) == eInput::Once) {
				setCursor(Qt::CursorShape::SizeAllCursor);
				mMoveCenter = gMouse.currentPosition();
				syncScrollBarPositionToCam();
			} else if (gMouse.state(2) == eInput::Once) {
				auto pos = *mMainCam * gMouse.currentPosition();
				mMainCamTarget.toX(pos.x);
				mMainCamTarget.toY(pos.y);
				syncCameraTarget();
				syncScrollBarPositionToCam();
				tweenCameraInput(mMainCamTween, *mMainCam, mMainCamTarget);
			}
		});

		ui.view->sig_MouseMove.connect([&]() {
			if (gMouse.state(1) == eInput::Hold) {
				mMoveDelta = (gMouse.currentPosition() - mMoveCenter) * mMainCam->scale().x;
				syncScrollBarPositionToCam();
			}
		});

		ui.view->sig_MouseRelease.connect([&]() {
			if (gMouse.state(1) == eInput::Lift) {
				mMoveDelta = vec2(0, 0);
				setCursor(QCursor(Qt::CursorShape::ArrowCursor));
				syncScrollBarPositionToCam();
			}
		});

		ui.view->sig_ScrollWheel.connect([&]() {
			if (zoomCamera(mMainCamTarget, true, 1.25f)) {
				syncCameraTarget();
				syncScrollBars();
				tweenCameraInput(mMainCamTween, *mMainCam, mMainCamTarget);
			}
		}); 
	}

	void QElangTextureWidget::snapCamera() {
		*mMainCam = mMainCamTarget;
		setupCameraTween(mMainCamTween);
	}

	void QElangTextureWidget::loop() {
		if (isActiveWindow() && mMainCam) { 
			if (gMouse.state(1) == eInput::Hold) {
				mMainCamTarget.move(vec3(mMoveDelta * 0.5, 0));
				syncCameraTarget();
				tweenCameraInput(mMainCamTween, *mMainCam, mMainCamTarget);
				//snapCamera();
				ui.view->update();
			}

			if (mMainCamTween.progress() != 1.0f) {
				mMainCamTween.step(1);
				ui.view->update();
			}
		}
	}

	void QElangTextureWidget::updateMaterial(asset<Material> texmat, const vec2& startPosition, float startScale) {
		if (texmat && texmat->hasTexture() && texmat->textures[0].has<AssetLoaded>()) {
			gMouse.reset();
			safeCreateObjects();
			mSprite.material = mMaterial = texmat;
			mSprite.recalc(mSpritePosition);

			auto tex = mMaterial->textures[0];
			auto w = (int)tex->width();
			auto h = -(int)tex->height();
			mTextureBox = Box(0, h, w, 0);

			mMainCamTarget.to(vec3(startPosition, -1000.0f));
			mMainCamTarget.resizeX(startScale);
			mMainCamTarget.resizeY(startScale);
			
			syncCameraTarget();
			snapCamera();
			syncScrollBars();

			ui.view->update();
		}
	}
	
	void QElangTextureWidget::syncCameraTarget() {
		if (mMainCam) {
			auto& cam = mMainCamTarget;
			auto w = round(ui.view->width());
			auto h = round(ui.view->height());

			mMainCamBox = cam * aabb(-w / 2.0f, -h / 2.0f, w / 2.0f, h / 2.0f);
			mTextureBox.trap(mMainCamBox);
			cam.to(vec3(mMainCamBox.center(), -1000.0f));
		}
	}

	void QElangTextureWidget::syncScrollBars() {
		if (mMainCam) {
			mSuppressScroll = true;
			if (mMaterial && mMaterial->hasTexture() && mMaterial->textures[0].has<AssetLoaded>()) {
				auto& cam = mMainCamTarget;
				float ix = ui.view->width() * cam.scale().x;
				float iy = ui.view->height() * cam.scale().y;

				auto tw = mTextureBox.width();
				auto mw = mMainCamBox.width();

				if (tw > mw) {
					ui.hori->setEnabled(true);
					ui.hori->setRange(0, tw - mw);
				} else {
					ui.hori->setEnabled(false);
				}

				auto th = mTextureBox.height();
				auto mh = mMainCamBox.height();

				if (th > mh) {
					ui.verti->setEnabled(true);
					ui.verti->setRange(0, th - mh);
				} else {
					ui.verti->setEnabled(false);
				}
			} else {
				ui.hori->setEnabled(false);
				ui.verti->setEnabled(false);
			}
			mSuppressScroll = false;
			syncScrollBarPositionToCam();
		}
	}

	void QElangTextureWidget::syncScrollBarPositionToCam() {
		mSuppressScroll = true;
		ui.hori->setValue(round(mMainCamBox.l - mTextureBox.l));
		ui.verti->setValue(round(mTextureBox.t - mMainCamBox.t));
		mSuppressScroll = false;
	}
}