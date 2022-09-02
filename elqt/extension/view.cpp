#include <elqtpch.h>
#include "view.h"

#include <tools/controls.h>
#include <tools/camera.h>

namespace el 
{
	bool QElangView::sInitialized = false;
	signal<> QElangView::sSig_GlobalGL;

	QElangView::QElangView(QWidget* parent) : QOpenGLWidget(parent), mInitialized(false) { }


	void QElangView::initializeGL() {
		if (!mInitialized) {
			mInitialized = true;
			bindStage();
			makeCurrent();
			onViewStart();
		}
	}                
	
	void QElangView::paintGL() { 
		bindStage();
		makeCurrent();
		updateViewport(-mWidth / 2.0f, mWidth / 2.0f, -mHeight / 2.0f, mHeight / 2.0f);
		onViewPaint(); 
	}
	
	void QElangView::resizeGL(int w, int h) { 
		bindStage();
		mWidth = w; mHeight = h;
		makeCurrent();
		updateViewport(-mWidth / 2.0f, mWidth / 2.0f, -mHeight / 2.0f, mHeight / 2.0f);
		onViewResize(w, h); 
	}

	void QElangView::mousePressEvent(QMouseEvent* me) {
		bindStage();
		makeCurrent();

		auto mpos = vec2(me->localPos().x() - mWidth * 0.5f, -me->localPos().y() + mHeight * 0.5f);
		gMouse.updateKeys(mpos);
		switch (me->button()) {
		case Qt::MouseButton::LeftButton:
			gMouse.onPress(mpos, MouseSym::Left);
			break;
		case Qt::MouseButton::RightButton:
			gMouse.onPress(mpos, MouseSym::Right);
			break;
		case Qt::MouseButton::MiddleButton:
			gMouse.onPress(mpos, MouseSym::Middle);
			break;
		} 

		onViewMousePress();
		update();
	}

	void QElangView::mouseReleaseEvent(QMouseEvent* me) {
		bindStage();
		makeCurrent();

		auto mpos = vec2(me->localPos().x() - mWidth * 0.5f, -me->localPos().y() + mHeight * 0.5f);
		gMouse.updateKeys(mpos);
		switch (me->button()) {
		case Qt::MouseButton::LeftButton:
			gMouse.onRelease(mpos, MouseSym::Left);
			break;
		case Qt::MouseButton::RightButton:
			gMouse.onRelease(mpos, MouseSym::Right);
			break;
		case Qt::MouseButton::MiddleButton:
			gMouse.onRelease(mpos, MouseSym::Middle);
			break;
		} 

		onViewMouseRelease();
		update();
	}

	void QElangView::mouseMoveEvent(QMouseEvent* me) {
		bindStage();
		makeCurrent();

		auto mpos = vec2(me->localPos().x() - mWidth * 0.5f, -me->localPos().y() + mHeight * 0.5f);
		gMouse.updateKeys(mpos);

		onViewMouseMove();
		update();
	}

	void QElangView::wheelEvent(QWheelEvent* me) {
		bindStage();
		makeCurrent();

		bool alt = QApplication::queryKeyboardModifiers() & Qt::AltModifier;
		float temp = (alt ? me->angleDelta().x() : me->angleDelta().y()) / 120.0f;
		gMouse.updateWheel(temp);

		onViewScrollWheel();
		update();
	}

	void QElangView::keyPressEvent(QKeyEvent* e) {
		bindStage();
		makeCurrent();
		onViewKeyPress(e);
	}
	void QElangView::keyReleaseEvent(QKeyEvent* e) {
		bindStage();
		makeCurrent();
		onViewKeyRelease(e);
	}
	bool QElangView::event(QEvent* e) {
		if (e->type() == QEvent::WindowActivate) {
			gMouse.reset();
			gKey.reset();
			bindStage();
			makeCurrent();
			onViewActivated();
		} return QWidget::event(e);
	}
}