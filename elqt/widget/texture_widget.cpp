
#include "texture_widget.h"

#include <apparatus/loader.h>
#include <apparatus/atelier.h>
#include <apparatus/ui.h>

#include <qtimer.h>

namespace el
{
	QElangViewSignaled* QElangTextureWidget::view(){ return ui.view; }

	QElangTextureWidget::QElangTextureWidget(QWidget* parent)
		: QWidget(parent), mMoveCursor(Qt::SizeAllCursor), mSuppressScroll(false), mMovingScreen(false) {
		ui.setupUi(this);

		// test debug
		QTimer* timer = new QTimer(this);
		connect(timer, &QTimer::timeout, [&]() {
			updateTick();
			});
		timer->start(1000.0f / 60.0f);
		//

		ui.view->sig_Start.connect([&]() {
			connectMouseInput();

			glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			mWinWidth = ui.view->width();
			mWinHeight = ui.view->height();
		});

		
		ui.view->sig_Resize.connect([&](int w, int h) {
			if (gGUI.open()) {
				bind(mStage);
				mWinWidth = w;
				mWinHeight = h;
				updateViewport(-w / 2.0f, w / 2.0f, -h / 2.0f, h / 2.0f);
				syncCamera();
				syncScrollBars();
			}
		});

		ui.view->sig_Paint.connect([&]() {
			glClearDepth(1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			if (gGUI.open()) {
				bind(mStage);
				glClearColor(0.2f, 0.3f, 0.2f, 1.0f);
				updateViewport(-mWinWidth / 2.0f, mWinWidth / 2.0f, -mWinHeight / 2.0f, mWinHeight / 2.0f); // safety net
				mTexture->batch();
				mSpritePainter->paint();
			}
			

			/*gScene->paint();

			for (auto e : mScene.objects.view<HitAABB>()) {
				auto& btn = mScene.objects.get<HitAABB>(e);
				auto pos = mScene.objects.get<Object2d>(e).local().position();
				btn.copy(pos).debug(&mScene.debug, color(0, 255, 0, 255), true);
			}
			mScene.debug.draw();

			bindGizmoObjects(true);
			for (const auto& e : mGizmos.view<Sprite>()) {
				auto& obj = mGizmos.get<Object2d>(e);
				if (obj.active()) {
					auto& spr = mGizmos.get<Sprite>(e);
					spr.update(obj.world());
					spr.batch();
				}
			} 
			
			mScene.atelier.draw();*/
		});

		connect(ui.hori, &QScrollBar::valueChanged, [&](int value) {
			if (gGUI.open() && !mSuppressScroll) {
				mMainCam->setPosX(mCamBounds.l + value * mScrollStep.x);
				ui.view->update();
			}
		});
		connect(ui.verti, &QScrollBar::valueChanged, [&](int value) {
			if (gGUI.open() && !mSuppressScroll) {
				mMainCam->setPosY(mCamBounds.t - value * mScrollStep.y);
				ui.view->update();
			}
		});
	}

	void QElangTextureWidget::connectMouseInput() {
		ui.view->sig_MousePress.connect([&]() {
			if (gGUI.open()) {
				bind(mStage);
				if (gMouse.state(1) == eInput::ONCE) {
					setCursor(QCursor(mMoveCursor));
					mMoveCenter = gMouse.currentPosition();
					vec2 center = *mMainCam * mMoveCenter;
				} updateAllButtons(mMainCam);
			}
		});

		ui.view->sig_MouseMove.connect([&]() {
			if (gGUI.open()) {
				bind(mStage);
				if (gMouse.state(1) == eInput::HOLD) {
					mMoveDelta = (gMouse.currentPosition() - mMoveCenter) * mMainCam->scale().x;
				} updateAllButtons(mMainCam);
			}
			});

		ui.view->sig_MouseRelease.connect([&]() {
			if (gGUI.open()) {
				bind(mStage);
				if (gMouse.state(1) == eInput::LIFT) {
					mMoveDelta = vec2(0, 0);
					setCursor(QCursor(Qt::CursorShape::ArrowCursor));
				} updateAllButtons(mMainCam);
			}
			});

		ui.view->sig_ScrollWheel.connect([&]() {
			if (gGUI.open()) {
				bind(mStage);
				float val = (5.0f / 6.0f);
				val = pow(val, gMouse.wheel());
				if (val != 0) {
					val = (val > 0) ? val : -1.0f / val; // change
					mMainCam->scale(vec3(val, val, 1));
					syncCamera();
					syncScrollBars();
				}
			}
		}); 
	}

	void QElangTextureWidget::updateTick() {
		if (gGUI.open()) {
			bind(mStage);
			if (isVisible() && (gMouse.state(1) == eInput::HOLD) && mMainCam) {
				mWinWidth = ui.view->width();
				mWinHeight = ui.view->height();
				syncCamera();
				syncScrollBars();

				auto& cam = *mMainCam;
				cam.move(vec3(mMoveDelta * 0.5, 0));

				vec2 pos = cam.position();
				mCamBounds.trap(pos);
				cam.to(vec3(pos, -1000));

				float ratioX = (cam.position().x - mCamBounds.l) / mScrollStep.x;
				float ratioY = -(cam.position().y - mCamBounds.t) / mScrollStep.y;

				mSuppressScroll = true;
				ui.hori->setValue(round(ratioX));
				ui.verti->setValue(round(ratioY));
				mSuppressScroll = false;

				ui.view->update();
			}
		}
	}

	void QElangTextureWidget::updateMaterial(asset<Material> texmat) {
		if (gGUI.open()) {
			refresh();
			if (texmat && texmat->hasTexture()) {
				auto& tex = texmat->textures[0];
				mTexture->material = texmat;
				auto& pos = mTexture.get<Position>();
				pos.x = tex->width() / 2;
				pos.y = -(int32)tex->height() / 2;
				mTexture->update(mTexture);

				onTextureUpdate(tex);
				syncCamera();
				syncScrollBars();
				ui.view->update();

				cout << "tex pos " << pos << endl;
				cout <<" cam pos " << mMainCam->position() << endl;
			}
		}
	}

	void QElangTextureWidget::refresh() {
		bind(mStage);

		mMainCam = gProject->cameras["Main Camera"];
		mMainCam->to(vec3(0.0f, 0.0f, -1000.0f));

		mSpritePainter = gProject->painters["Sprite Painter"];

		gStage->clear();
		mTexture = gStage->make<Sprite>(asset<Material>(), mSpritePainter, "");
		mTexture.add<Position>();
	}

	void QElangTextureWidget::syncCamera() {
		auto material = mTexture->material;
		if (mMainCam && material && material->hasTexture()) {
			auto& cam = *mMainCam;
			float ix = cam.scale().x;
			float iy = cam.scale().y;

			vec2 prevOffset;
			prevOffset.x = (cam.position().x - mCamBounds.l);
			prevOffset.y = -(cam.position().y - mCamBounds.t);

			auto tex = material->textures[0];
			float bandX = max(0, tex->width() - mWinWidth * ix);
			float bandY = max(0, tex->height() - mWinHeight * iy);

			mCamBounds.l = mWinWidth * ix * 0.5f;
			mCamBounds.t = -(mWinHeight * iy * 0.5f);
			mCamBounds.r = mCamBounds.l + bandX;
			mCamBounds.b = mCamBounds.t - bandY;

			prevOffset.x = clamp(prevOffset.x, 0, bandX);
			prevOffset.y = clamp(prevOffset.y, 0, bandY);

			cam.to(vec3(mCamBounds.l + prevOffset.x, mCamBounds.t - prevOffset.y, -1000.0f));
		}
	}
	void QElangTextureWidget::syncScrollBars() {
		auto material = mTexture->material;
		if (mMainCam && material && material->hasTexture()) {
			auto& cam = *mMainCam;
			float ix = mWinWidth * cam.scale().x;
			float iy = mWinHeight * cam.scale().y;

			auto tex = material->textures[0];
			if (tex->width() > ix) {
				ui.hori->setEnabled(true);
				int count = tex->width() / ix;
				ui.hori->setRange(0, count);
				mScrollStep.x = (tex->width() - ix) / (float)count;
			}
			else {
				ui.hori->setEnabled(false);
			}

			if (tex->height() > iy) {
				ui.verti->setEnabled(true);
				int count = tex->height() / iy;
				ui.verti->setRange(0, count);
				mScrollStep.y = (tex->height() - iy) / (float)count;
			}
			else {
				ui.verti->setEnabled(false);
			}
		}
	}
	void QElangTextureWidget::showEvent(QShowEvent* e) {
		updateViewport(-mWinWidth / 2.0f, mWinWidth / 2.0f, -mWinHeight / 2.0f, mWinHeight / 2.0f);
	}

}