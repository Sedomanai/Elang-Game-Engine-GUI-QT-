#pragma once

#include <GL/glew.h>
#include <qevent.h>
#include <qwidget.h>
#include <qopenglwidget.h>
#include <qopenglcontext.h>

#include <tools/registry.h>
#include <tools/controls.h>
#include <common/signal.h>

#include "../../elang_qt_assets.h"
#include "../../elang_qt_builder.h"
#include "../../elang_qt_globals.h"

namespace el {

	/// <summary>
	/// An all purpose OpenGl widget view. Inherit. 
	/// </summary>
	class _ELANGQT_EXPORT QElangView : public QOpenGLWidget
	{
		Q_OBJECT

	public:
		QElangView(QWidget* parent = Q_NULLPTR);

		static signal<> sSig_GlobalGL;
		void bindStage() { bind(mStage); }
	protected:
		virtual void onViewStart() = 0;
		virtual void onViewPaint() = 0;
		virtual void onViewResize(int w, int h) {};
		virtual void onViewScrollWheel() {};
		virtual void onViewMousePress() {};
		virtual void onViewMouseRelease() {};
		virtual void onViewMouseMove() {};
		virtual void onViewKeyPress(QKeyEvent*) {};
		virtual void onViewKeyRelease(QKeyEvent*) {};
		virtual void onViewActivated() {};

		void initializeGL() override;
		void paintGL() override;
		void resizeGL(int w, int h) override;
		void mousePressEvent(QMouseEvent*) override; // Transform& camera);
		void mouseReleaseEvent(QMouseEvent*) override;
		void mouseMoveEvent(QMouseEvent*) override;
		void wheelEvent(QWheelEvent*) override;
		void keyPressEvent(QKeyEvent*) override;
		void keyReleaseEvent(QKeyEvent*) override;
		bool event(QEvent* e) override;

		static bool sInitialized;
		bool mInitialized;
		float mWidth, mHeight;
		Stage mStage;
	};

	/// <summary>
	/// View interface decoupled via signals. Inheritance allowed but usually not necessary.
	/// </summary>
	class _ELANGQT_EXPORT QElangViewSignaled : public QElangView
	{
		Q_OBJECT

	public:
		QElangViewSignaled(QWidget* parent = Q_NULLPTR) : QElangView(parent) {};

		signal<> sig_Start;
		signal<> sig_Paint;
		signal<int, int> sig_Resize;
		signal<> sig_ScrollWheel;
		signal<> sig_MousePress;
		signal<> sig_MouseRelease;
		signal<> sig_MouseMove;
		signal<QKeyEvent*> sig_KeyPress;
		signal<QKeyEvent*> sig_KeyRelease;

	protected:
		virtual void onViewStart() override { sig_Start.invoke(); };
		virtual void onViewPaint() override { sig_Paint.invoke(); };
		virtual void onViewResize(int w, int h) override { sig_Resize.invoke(w, h); };
		virtual void onViewMousePress() override { sig_MousePress.invoke(); };
		virtual void onViewMouseRelease() override { sig_MouseRelease.invoke(); };
		virtual void onViewMouseMove() override { sig_MouseMove.invoke(); };
		virtual void onViewScrollWheel() override { sig_ScrollWheel.invoke(); };
		virtual void onViewKeyPress(QKeyEvent* e) override { sig_KeyPress.invoke(e); };
		virtual void onViewKeyRelease(QKeyEvent* e) override { sig_KeyRelease.invoke(e); };
	};
}