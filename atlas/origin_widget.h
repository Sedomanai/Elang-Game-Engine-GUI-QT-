#pragma once
#include "cells_widget.h"
#include "ghost_dialog.h"

namespace il
{
	struct OriginView : public QIlangView
	{
		Q_OBJECT

	signals:
		void sig_WheelEvent(float);
		void sig_SafeTexture();
	public:
		OriginView(QWidget* parent = Q_NULLPTR);
		virtual ~OriginView();

		void updateTexture();
		void setGhost();

		void showEditor();
		void hideEditor();


		void moveCellOrigin(int x, int y);
		void captureGhost();
		void shiftCell(int dir);

	private:
		prop<Camera> mFuckingViewport;
		prop<Material> mTexMat, mExtMat;

		uint mGhostRow;
		Scene mScene;
		EditorCell* mSelect, *mGhost;
		GhostDialog* mGDialog;
		bool mClicked;

		Batch mCellBatch, mGhostBatch;
		vec2 mGrabPos, mGrabUV;
		void connectList();
		void connectMouseInput();
		void drawMidLines();
		void batch(EditorCell*, const Batch&, strview painter, asset<Texture> tex);
	};
}