#pragma once
#include "cells_widget.h"
//#include "ghost_dialog.h"

namespace el
{
	struct OriginView : public QElangView
	{
		Q_OBJECT

	//signals:
	//	void sig_WheelEvent(float);
	//	void sig_SafeTexture();
	public:
		OriginView(QWidget* parent = Q_NULLPTR);
		virtual ~OriginView();

		void onViewStart() override;
		void onViewPaint() override;


		void updateTexture();
		//void setGhost();

		void showEditor();
		void hideEditor();


		//void moveCellOrigin(int x, int y);
		//void captureGhost();
		//void shiftCell(int dir);

	private:
		Stage mStage;
		asset<Camera> mMainCam;
		asset<Painter> mSpritePainter;
		asset<Texture> mTexture;
		obj<Sprite> mTexObj;
		//prop<Camera> mFuckingViewport;
		//prop<Material> mTexMat, mExtMat;

		//uint mGhostRow;
		//Scene mScene;
		//EditorCell* mSelect, *mGhost;
		//GhostDialog* mGDialog;
		//bool mClicked;

		//Batch mCellBatch, mGhostBatch;
		//vec2 mGrabPos, mGrabUV;
		void connectList();
		//void connectMouseInput();
		//void drawMidLines();
		//void batch(EditorCell*, const Batch&, strview painter, asset<Texture> tex);
	};
}