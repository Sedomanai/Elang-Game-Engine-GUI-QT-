#pragma once

#include <QWidget>
#include <elements/basic.h>
#include <elements/sprite.h>

#include "../elang_qt_builder.h"
#include "view_extension.h"

#include <uic/ui_texture_widget.h>

namespace el
{
	class _ELANGQT_EXPORT QElangTextureWidget : public QWidget
	{
		Q_OBJECT

	public:
		QElangTextureWidget(QWidget* parent = Q_NULLPTR);
		~QElangTextureWidget();

		void updateTick();
		void updateMaterial(asset<Material>);
		QElangViewSignaled* view();
	protected:
		Ui::QElangTextureWidgetUI ui;
		virtual void onTextureUpdate(asset<Texture>) {};

		Qt::CursorShape mMoveCursor;

		Stage mStage;
		obj<Sprite> mTexSprite;
		int mWinWidth, mWinHeight;

		aabb mCamBounds;
		vec2 mMoveDelta;
		vec2 mMoveCenter;
		vec2 mScrollStep;
		float mResolution;

		asset<Camera> mMainCam;
		asset<Painter> mSpritePainter;
		asset<Material> mTexMat;
		ShapeDebug* mGadgetPainter;

		bool mSuppressScroll;
		bool mMovingScreen;

		void connectMouseInput();
		void syncCamera();
		void syncScrollBars();
		void showEvent(QShowEvent* e) override;
	};

}





