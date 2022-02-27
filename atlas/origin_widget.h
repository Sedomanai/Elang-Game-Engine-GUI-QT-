#pragma once
#include "cells_widget.h"
#include "ghost_dialog.h"

namespace el
{
	struct OriginView : public QElangView
	{
		Q_OBJECT

		enum CursorState
		{
			LEFT = 2,
			BOTTOM = 4,
			RIGHT = 8,
			TOP = 16,
		};

		enum eState {
			CREATING,
			MOVING,
			MOVING_HITBOX
		};

	public:
		OriginView(QWidget* parent = Q_NULLPTR);
		virtual ~OriginView();

		void onViewStart() override;
		void onViewPaint() override;
		void onViewMousePress() override;
		void onViewMouseRelease() override;
		void onViewMouseMove() override;
		void onViewScrollWheel() override;

		void updateTexture();
		void setGhost(ElangAtlasGhostData ghostData) { mGhostData = ghostData; }

		void showEditor();
		void hideEditor();

		void drawCellHitbox(bool batchOnly = false);
		void moveCellOrigin(int x, int y);
		void captureGhost();
		void shiftCell(int dir);

		void release() {
			if (mMainCam) {
				mMainCam.destroy();
				mPainter.destroy();
				delete mHighlighter;
			}
		}

		void safeCreateObjects();
		void onKeyPress(QKeyEvent*);
		void onKeyRelease(QKeyEvent*);

	private:
		asset<Texture> mTexture;
		asset<EditorCamera> mMainCam;
		asset<EditorProjectPainter> mPainter;

		obj<EditorProjectSprite> mCellObj;

		eState mCreateState;
		aabb mCreateRect;
		aabb mGrabRect;
		uint mCursorState;

		ElangAtlasGhostData mGhostData;

		EditorShapeDebug* mHighlighter;
		vec2 mGrabPos, mGrabUV;
		void connectList();
		void paintGhostCell();
	};
}