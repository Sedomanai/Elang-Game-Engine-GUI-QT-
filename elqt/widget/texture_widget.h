#pragma once

#include <QWidget>
#include <elements/basic.h>
#include <elements/sprite.h>
#include <uic/ui_texture_widget.h>

#include "../extension/view.h"
#include "../../elang_qt_globals.h"

namespace el
{
	class _ELANGQT_EXPORT QElangTextureWidget : public QWidget
	{
		Q_OBJECT

	public:
		QElangTextureWidget(QWidget* parent = Q_NULLPTR);

		void updateTick();
		void updateMaterial(asset<EditorMaterial>);
		QElangViewSignaled* view();


	protected:
		Ui::QElangTextureWidgetUI ui;
		virtual void onTextureUpdate() {};

		Qt::CursorShape mMoveCursor;
		int mWinWidth, mWinHeight;

		aabb mCamBounds;
		vec2 mMoveDelta;
		vec2 mMoveCenter;
		vec2 mScrollStep;
		float mResolution;

		Box mMainCamBox, mTextureBox;
		asset<Texture> mTexture;
		asset<EditorCamera> mMainCam;
		asset<EditorPainter> mPainter;
		obj<EditorSprite> mTexObj;

		bool mSuppressScroll;
		bool mMovingScreen;

		void connectMouseInput();
		void syncCamera(); // fixed
		void syncScrollBars(); // fixed
		void showEvent(QShowEvent* e) override;
	};

}





