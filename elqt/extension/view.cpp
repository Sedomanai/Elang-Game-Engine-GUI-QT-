#include "view.h"

namespace el 
{
	bool QElangView::sInitialized = false;
	signal<> QElangView::sSig_GlobalGL;

	QElangView::QElangView(QWidget* parent) : QOpenGLWidget(parent), mInitialized(false) { bind(mStage); }

	void QElangView::initializeGL() {
		if (!sInitialized) {
			if (glewInit() != GLEW_OK)
				throw "GLEW could not initialize.";
			sSig_GlobalGL.invoke();
			sInitialized = true;
		}

		if (sInitialized) { // if GLEW successfully initiated
			if (!mInitialized) {
				bind(mStage);
				makeCurrent();
				onViewStart();
				mInitialized = true;
			}
		}
	}                
	void QElangView::paintGL() { if (sInitialized) { bind(mStage); makeCurrent(); onViewPaint(); } }
	void QElangView::resizeGL(int w, int h) { mWidth = w; mHeight = h; 
		if (sInitialized) { bind(mStage); makeCurrent(); onViewResize(w, h); } }


	void QElangView::mousePressEvent(QMouseEvent* me) {
		bind(mStage);
		makeCurrent();

		auto mpos = vec2(me->localPos().x() - mWidth * 0.5f, -me->localPos().y() + mHeight * 0.5f);
		gMouse.updateKeys(mpos);
		switch (me->button()) {
		case Qt::MouseButton::LeftButton:
			gMouse.onPress(mpos, MouseSym::LEFT);
			break;
		case Qt::MouseButton::RightButton:
			gMouse.onPress(mpos, MouseSym::RIGHT);
			break;
		case Qt::MouseButton::MiddleButton:
			gMouse.onPress(mpos, MouseSym::MIDDLE);
			break;
		} 

		onViewMousePress();
		update();
	}

	void QElangView::mouseReleaseEvent(QMouseEvent* me) {
		bind(mStage);
		makeCurrent();

		auto mpos = vec2(me->localPos().x() - mWidth * 0.5f, -me->localPos().y() + mHeight * 0.5f);
		gMouse.updateKeys(mpos);
		switch (me->button()) {
		case Qt::MouseButton::LeftButton:
			gMouse.onRelease(mpos, MouseSym::LEFT);
			break;
		case Qt::MouseButton::RightButton:
			gMouse.onRelease(mpos, MouseSym::RIGHT);
			break;
		case Qt::MouseButton::MiddleButton:
			gMouse.onRelease(mpos, MouseSym::MIDDLE);
			break;
		} 

		onViewMouseRelease();
		update();
	}

	void QElangView::mouseMoveEvent(QMouseEvent* me) {
		bind(mStage);
		makeCurrent();

		auto mpos = vec2(me->localPos().x() - mWidth * 0.5f, -me->localPos().y() + mHeight * 0.5f);
		gMouse.updateKeys(mpos);

		onViewMouseMove();
		update();
	}

	void QElangView::wheelEvent(QWheelEvent* me) {
		bind(mStage);
		makeCurrent();

		float temp = me->angleDelta().y() / 120.0f;
		gMouse.updateWheel(temp);

		onViewScrollWheel();
		update();
	}

	void QElangView::keyPressEvent(QKeyEvent* e) {
		bind(mStage);
		makeCurrent();
		onViewKeyPress(e);
	}
	void QElangView::keyReleaseEvent(QKeyEvent* e) {
		bind(mStage);
		makeCurrent();
		onViewKeyRelease(e);
	}
	bool QElangView::event(QEvent* e) {
		if (e->type() == QEvent::WindowActivate) {
			gMouse.reset();
			gKey.reset();
			bind(mStage);
			makeCurrent();
			onViewActivated();
		} return QWidget::event(e);
	}
}