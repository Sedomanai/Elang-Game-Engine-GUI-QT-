
#include "texture_widget.h"

#include <apparatus/loader.h>
#include <apparatus/atelier.h>
#include <apparatus/ui.h>

#include <qtimer.h>
#include <qopenglcontext.h>

namespace el
{
	QElangViewSignaled* QElangTextureWidget::view(){ return ui.view; }

	QElangTextureWidget::QElangTextureWidget(QWidget* parent, bool internalLoop)
		: QWidget(parent), mMoveCursor(Qt::SizeAllCursor), mSuppressScroll(false), mMovingScreen(false) 
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

			//glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			glClearColor(0.2f, 0.3f, 0.2f, 1.0f);
			safeCreateObjects();
		});

		
		ui.view->sig_Resize.connect([&](int w, int h) {
			syncCamera();
			syncScrollBars();
		});

		ui.view->sig_Paint.connect([&]() {
			glClearDepth(1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			mTexObj->batch();
			mPainter->paint();

		});

		connect(ui.hori, &QScrollBar::valueChanged, [&](int value) {
			if (!mSuppressScroll) {
				mMainCam->setPosX(value + mMainCamBox.width() / 2);
				ui.view->update();
			}
		});
		connect(ui.verti, &QScrollBar::valueChanged, [&](int value) {
			if (!mSuppressScroll) {
				mMainCam->setPosY(-value - mMainCamBox.height() /2);
				ui.view->update();
			}
		});
	}

	void QElangTextureWidget::safeCreateObjects()
	{
		ui.view->makeCurrent();
		if (!mMainCam) {
			mMainCam = gProject->makeSub<EditorCamera>();
			mMainCam->to(vec3(0.0f, 0.0f, -1000.0f));
			mPainter = gProject->makeSub<EditorProjectPainter>("basic_sprite", "texture_uv", 
				100000, mMainCam, Projection::eOrtho,
				ePainterFlags::DEPTH_SORT | ePainterFlags::MULTI_MATERIAL | ePainterFlags::Z_CLEAR);
			mPainter->init();
		}

		if (!mTexObj) {
			mTexObj = gStage->make<EditorProjectSprite>(NullEntity, mPainter, "");
			mTexObj.add<Position>().update();
		}
	}

	void QElangTextureWidget::connectMouseInput() {
		ui.view->sig_MousePress.connect([&]() {
			if (gMouse.state(1) == eInput::ONCE) {
				setCursor(QCursor(mMoveCursor));
				mMoveCenter = gMouse.currentPosition();
			}
		});

		ui.view->sig_MouseMove.connect([&]() {
			if (gMouse.state(1) == eInput::HOLD) {
				mMoveDelta = (gMouse.currentPosition() - mMoveCenter) * mMainCam->scale().x;
			}
		});

		ui.view->sig_MouseRelease.connect([&]() {
			if (gMouse.state(1) == eInput::LIFT) {
				mMoveDelta = vec2(0, 0);
				setCursor(QCursor(Qt::CursorShape::ArrowCursor));
			}
		});

		ui.view->sig_ScrollWheel.connect([&]() {
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
		}); 
	}

	void QElangTextureWidget::loop() {
		//ui.view->bindStage();
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

	void QElangTextureWidget::updateMaterial(asset<EditorProjectMaterial> texmat) {
		//ui.view->bindStage();
		if (texmat && texmat->hasTexture()) {
			mTexture = texmat->textures[0];

			safeCreateObjects();
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
	
	void QElangTextureWidget::syncCamera() {
		if (mMainCam && mTexture) {
			auto& cam = *mMainCam;

			auto w = round(ui.view->width());
			auto h = round(ui.view->height());

			vec2 lb = cam * vec2(-w /2.0f, -h / 2.0f);
			vec2 rt = cam * vec2(w / 2.0f, h / 2.0f);
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
		if (mMainCam && mTexture) {
			auto& cam = *mMainCam;
			float ix = ui.view->width() * cam.scale().x;
			float iy = ui.view->height() * cam.scale().y;

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