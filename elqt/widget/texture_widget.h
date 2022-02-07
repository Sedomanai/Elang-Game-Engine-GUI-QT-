#pragma once

#include <QWidget>
#include <elements/basic.h>
#include <elements/sprite.h>
#include <uic/ui_texture_widget.h>

#include "../extension/view.h"
#include "../../elang_qt_globals.h"
#include "../../elang_qt_builder.h"

namespace el
{
	class _ELANGQT_EXPORT QElangTextureWidget : public QWidget
	{
		Q_OBJECT

	public:
		QElangTextureWidget(QWidget* parent = Q_NULLPTR);

		void updateTick();
		void updateMaterial(asset<Material>);
		QElangViewSignaled* view();


	protected:
		Ui::QElangTextureWidgetUI ui;
		virtual void onTextureUpdate(asset<Texture>) {};
		virtual void refresh();

		Qt::CursorShape mMoveCursor;

		Stage mStage;
		int mWinWidth, mWinHeight;

		aabb mCamBounds;
		vec2 mMoveDelta;
		vec2 mMoveCenter;
		vec2 mScrollStep;
		float mResolution;

		asset<Camera> mMainCam;
		asset<Painter> mSpritePainter;

		obj<Sprite> mTexture;

		bool mSuppressScroll;
		bool mMovingScreen;

		void connectMouseInput();
		void syncCamera(); // fixed
		void syncScrollBars(); // fixed
		void showEvent(QShowEvent* e) override;
	};

}





