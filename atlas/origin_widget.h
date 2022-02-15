#pragma once
#include "cells_widget.h"
#include "ghost_dialog.h"

namespace el
{
	struct OriginView : public QElangView
	{
		Q_OBJECT

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

	private:
		asset<Texture> mTexture;
		asset<EditorCamera> mMainCam;
		asset<EditorProjectPainter> mPainter;

		obj<EditorProjectSprite> mCellObj;

		ElangAtlasGhostData mGhostData;

		EditorShapeDebug* mHighlighter;
		vec2 mGrabPos, mGrabUV;
		void connectList();
		void paintGhostCell();
	};
}