
#include "texture_widget.h"

#include <apparatus/loader.h>
#include <apparatus/atelier.h>
#include <apparatus/ui.h>

#include <qtimer.h>
#include <qopenglcontext.h>

namespace el
{
	QElangViewSignaled* QElangTextureWidget::view(){ return ui.view; }

	QElangTextureWidget::QElangTextureWidget(QWidget* parent)
		: QWidget(parent), mMoveCursor(Qt::SizeAllCursor), mSuppressScroll(false), mMovingScreen(false) 
	{
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

			//glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			mWinWidth = ui.view->width();
			mWinHeight = ui.view->height();
			
			glClearColor(0.2f, 0.3f, 0.2f, 1.0f);

			mMainCam = gProject->makeSub<EditorCamera>();
			mMainCam->to(vec3(0.0f, 0.0f, -1000.0f));
			
			mPainter = gProject->makeSub<EditorPainter>("basic_sprite", "texture_uv", 100000, mMainCam, Projection::eOrtho,
				Painter::DEPTH_SORT | Painter::MULTI_MATERIAL | Painter::Z_CLEAR);
			mPainter->init();
			
			mTexObj = gStage->make<EditorSprite>(NullEntity, mPainter, "");
			mTexObj.add<Position>().update();
		});

		
		ui.view->sig_Resize.connect([&](int w, int h) {
			mWinWidth = w;
			mWinHeight = h;
			updateViewport(-mWinWidth / 2, mWinWidth / 2, -mWinHeight / 2, mWinHeight / 2);

			if (gGUI.open()) {
				syncCamera();
				syncScrollBars();
			}
		});

		ui.view->sig_Paint.connect([&]() {
			glClearDepth(1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			if (gGUI.open()) {
				mTexObj->batch();
				mPainter->paint();
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
				mMainCam->setPosX(value + mMainCamBox.width() / 2);
				ui.view->update();
			}
		});
		connect(ui.verti, &QScrollBar::valueChanged, [&](int value) {
			if (gGUI.open() && !mSuppressScroll) {
				mMainCam->setPosY(-value - mMainCamBox.height() /2);
				ui.view->update();
			}
		});
	}

	void QElangTextureWidget::showEvent(QShowEvent* e) {
		updateViewport(-mWinWidth / 2, mWinWidth / 2, -mWinHeight / 2, mWinHeight / 2);
	}

	void QElangTextureWidget::connectMouseInput() {
		ui.view->sig_MousePress.connect([&]() {
			if (gGUI.open()) {
				if (gMouse.state(1) == eInput::ONCE) {
					setCursor(QCursor(mMoveCursor));
					mMoveCenter = gMouse.currentPosition();
				} updateAllEditorButtons(mMainCam);
			}
		});

		ui.view->sig_MouseMove.connect([&]() {
			if (gGUI.open()) {
				if (gMouse.state(1) == eInput::HOLD) {
					mMoveDelta = (gMouse.currentPosition() - mMoveCenter) * mMainCam->scale().x;
				} updateAllEditorButtons(mMainCam);
			}
		});

		ui.view->sig_MouseRelease.connect([&]() {
			if (gGUI.open()) {
				if (gMouse.state(1) == eInput::LIFT) {
					mMoveDelta = vec2(0, 0);
					setCursor(QCursor(Qt::CursorShape::ArrowCursor));
				} updateAllEditorButtons(mMainCam);
			}
		});

		ui.view->sig_ScrollWheel.connect([&]() {
			if (gGUI.open()) {
				float val = (5.0f / 6.0f);
				val = pow(val, gMouse.wheel());
				if (val != 0) {
					val = (val > 0) ? val : -1.0f / val; // scroll up: 5/6, down: 6/5

					auto mpos = *mMainCam * gMouse.currentPosition();
					auto cpos = mMainCam->position();
					auto delta = mpos - cpos;
					delta *= (1 - val);

					mMainCam->scale(vec3(val, val, 1));
					mMainCam->move(vec3(delta, 0));

					syncCamera();
					syncScrollBars();
				}
			}
		}); 
	}

	void QElangTextureWidget::updateTick() {
		if (gGUI.open()) {
			ui.view->bindStage();
			if (isActiveWindow() && (gMouse.state(1) == eInput::HOLD) && mMainCam) {
				auto& cam = *mMainCam;
				cam.move(vec3(mMoveDelta * 0.5, 0));

				mSuppressScroll = true;
				ui.hori->setValue(round(mMainCamBox.l - mTextureBox.l));
				ui.verti->setValue(round(mTextureBox.t - mMainCamBox.t));
				mSuppressScroll = false;

				syncCamera();
				syncScrollBars();
				ui.view->update();
			}
		}
	}

	void QElangTextureWidget::updateMaterial(asset<EditorMaterial> texmat) {
		if (gGUI.open()) {
			ui.view->bindStage();
			if (texmat && texmat->hasTexture()) {
				mTexture = texmat->textures[0];
				mTexObj->material = texmat;
				mTexObj->update(mTexObj);

				auto w = (int)mTexture->width() + 2;
				auto h = -(int)mTexture->height() - 2;
				mTextureBox = Box(-2, h, w, 2);
				onTextureUpdate();
				syncCamera();
				syncScrollBars();
				ui.view->update();
			}
		}
	}
	
	void QElangTextureWidget::syncCamera() {
		assert(gGUI.open());

		if (mMainCam && mTexture) {
			auto& cam = *mMainCam;

			vec2 lb = cam * vec2(-mWinWidth/2, -mWinHeight / 2);
			vec2 rt = cam * vec2(mWinWidth/2, mWinHeight /2);
			mMainCamBox = Box(lb.x, lb.y, rt.x, rt.y);

			vec2 move;

			if (mMainCamBox.width() < mTextureBox.width()) {
				if (mMainCamBox.l < mTextureBox.l)
					move.x = mTextureBox.l - mMainCamBox.l;
				if (mMainCamBox.r > mTextureBox.r)
					move.x = mTextureBox.r - mMainCamBox.r;
			} else {
				mMainCamBox.r = mMainCamBox.width();
				mMainCamBox.l = 0;
			}

			if (mMainCamBox.height() < mTextureBox.height()) {
				if (mMainCamBox.b < mTextureBox.b)
					move.y = mTextureBox.b - mMainCamBox.b;
				if (mMainCamBox.t > mTextureBox.t)
					move.y = mTextureBox.t - mMainCamBox.t;
			}
			else {
				mMainCamBox.b = -mMainCamBox.height();
				mMainCamBox.t = 0;
			}

			mMainCamBox.move(move);
			cam.to(vec3(mMainCamBox.center(), -1000.0f));
		}
	}
	void QElangTextureWidget::syncScrollBars() {
		assert(gGUI.open());

		if (mMainCam && mTexture) {
			auto& cam = *mMainCam;
			float ix = mWinWidth * cam.scale().x;
			float iy = mWinHeight * cam.scale().y;

			auto tw = mTextureBox.width();
			auto mw = mMainCamBox.width();

			if (tw > mw) {
				ui.hori->setEnabled(true);
				ui.hori->setRange(0, tw - mw);
			}
			else {
				ui.hori->setEnabled(false);
			}

			auto th = mTextureBox.height();
			auto mh = mMainCamBox.height();

			if (th > mh) {
				ui.verti->setEnabled(true);
				ui.verti->setRange(0, th - mh);
			}
			else {
				ui.verti->setEnabled(false);
			}
		}
	}
}