#pragma once
#include "ghost_dialog.h"
#include "../elqt/extension/view.h"

#include <tools/camera.h>
#include <elements/basic.h>
#include <elements/sprite.h>
#include <elements/button.h>

#include <tweeny/tween.h>

namespace el
{
	struct ShapeDebug2d;
	struct PivotView : public QElangView
	{
		Q_OBJECT

		enum CursorState
		{
			eLeft = 2,
			eBottom = 4,
			eRight = 8,
			eTop = 16,
		};

		enum eState {
			Creating,
			Moving,
			MovingHitbox
		};

	public:
		PivotView(QWidget* parent = Q_NULLPTR);
		virtual ~PivotView();

		void onViewStart() override;
		void onViewPaint() override;
		void onViewMousePress() override;
		void onViewMouseRelease() override;
		void onViewMouseMove() override;
		void onViewScrollWheel() override;

		void showEditor();
		void hideEditor();

		void moveCurrentCell();
		void drawCellHitbox(bool batchOnly = false);
		void incrementPivot(int x, int y);
		void shiftCell(int dir);
		void setPreviousGhost();
		void captureGhost();
		void ghostPalette();

		void setGhostPosition(bool front);

		void release();
		void loop();
		void safeCreateObjects();
		void execGhostDialog();
		void snapCamera();
		void recenterCamera();

		void onKeyPress(QKeyEvent*);
		void onKeyRelease(QKeyEvent*);

		signal<> sig_Modified;
	private:
		asset<Camera> mMainCam;
		tweeny::tween<vec3> mMainCamTween;
		Camera mMainCamTarget;

		asset<Painter> mPainter;

		Sprite mCellSprite, mGhostSprite;
		Position mCellPos;

		eState mCreateState;
		aabb mCreateRect;
		aabb mGrabRect;
		uint mCursorState;

		ElangAtlasGhostData mGhostData;
		ShapeDebug2d* mHighlighter;
		vec2 mGrabPos, mGrabUV;
		void connectList();
		void paintGhostCell();
	};
}