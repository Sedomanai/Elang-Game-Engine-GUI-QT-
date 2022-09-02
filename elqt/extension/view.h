/*****************************************************************//**
 * @file   tree.h
 * @brief  Custom extension for QOpenGLWidget.
 *		   The functionalities of some of the built-in QT widgets is either not enough or requires a workaround.
 *		   This extension not only helps extend those functionalities, it also provides a catch-all interface
 *		   that does not require one inheriting from either the base QT class or this class.
 *
 * @author Sedomanai
 * @date   August 2022
 *********************************************************************/
#pragma once
#include <common/signal.h>
#include <tools/stage.h>

namespace el {
	/**
	 * Custom extension for QOpenGLWidget.
	 * 
	 * The name was changed to QElangView because this is an all purpose class for anything that requires graphic rendering.
	 * This type does not include much signals and should be inherited. 
	 * It is good for extensive viewport with their own complicated logic built-in the inherited class. 
	 */
	class QElangView : public QOpenGLWidget
	{
		Q_OBJECT
	public:
		QElangView(QWidget* parent = Q_NULLPTR);

		static signal<> sSig_GlobalGL;
		void bindStage() { gStage = mStage; }
		void setStage(asset<Stage> stage) { mStage = stage; }
		float width() { return mWidth; }
		float height() { return mHeight; }

	protected:
		virtual void onViewStart() {};
		virtual void onViewPaint() {};
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
		asset<Stage> mStage;
	};
	/**
	 * Custom extension for QOpenGLWidget with extra signals.
	 *
	 * The name was changed to QElangViewSignaled because this is an all purpose class for anything that requires graphic rendering.
	 * The is for simple graphic views that don't require much logic and receives graphic info from external sources via signals.
	 *
	 * The interface is built around signals and slots.
	 * Custom signals and slots were used over QSignals and QSlots.
	 * The reason for that is actually consistency - there were some cases where QSignals and QSlots cannot satisfy.
	 */
	class QElangViewSignaled : public QElangView
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