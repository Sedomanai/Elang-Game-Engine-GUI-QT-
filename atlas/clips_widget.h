#pragma once
#include "cells_widget.h"
#include <uic/ui_clips_widget.h>
#include <elements/clip.h>
#include <common/random.h>
//#include "aepalette.h"
//#include "atlas_util.h"

namespace el
{

	#define cReelFrameSize 100

	struct ClipframeHolder
	{
		EditorProjectCanvas canvas;
		sizet index;

		ClipframeHolder(
			asset<EditorProjectMaterial> material, asset<EditorProjectPainter> painter
		) : canvas(material, painter) {}

		void reshape(asset<Clip> clip) {
			assert(index < clip->size());
			auto cell = clip->at(index);
			if (cell)
				canvas.update(cell);
		}

		void reorder(sizet index_) {
			index = index_;
			canvas.bounds = aabb(index * cReelFrameSize, -cReelFrameSize, index * cReelFrameSize + cReelFrameSize, -15);
		}
	};

	struct ClipsWidget : public QWidget, public IButtonEvent
	{
		Q_OBJECT
	public:
		ClipsWidget(QWidget* parent = Q_NULLPTR);

		void updateTexture();
		void showEditor();
		void hideEditor();
		void loop();
		void clearAllViews();

		void addClip();
		void removeClip();
		void addFrame();

		void onKeyPress(QKeyEvent*);
		void onKeyRelease(QKeyEvent*);

		QElangView* view() { return ui.view; }
		QElangView* reel() { return ui.reel; }
	private:
		void addReel(asset<Clip>);
		void recreateReel(asset<Clip>);

		int mViewWidth, mViewHeight, mFramesWidth, mFramesHeight;
		int mMovingSide;

		rng crng;
		Ui::ClipsWidgetUI ui;

		asset<EditorCamera> mViewCam;
		asset<EditorProjectPainter> mViewPainter;
		EditorShapeDebug* mViewShapes;


		asset<Texture> mTexture;
		obj<EditorProjectSprite> mClipObj;
		obj<ClipframeHolder> mHovering, mMoving, mRemovable;

		asset<EditorCamera> mReelCam;
		asset<EditorProjectPainter> mReelPainter;
		EditorShapeDebug* mReelShapes;
		QLabel* mLabel;

		bool mPaused;
		uint32 mFrame;

		void updateAllCanvasButton();
		void safeCreateViewObjects();
		void safeCreateFrameObjects();
		void connectList();
		void syncScroll();

		// Inherited via IButtonEvent
		void onEnter(Entity self, Entity context) override {};
		void onHover(Entity self, Entity context) override;
		void onExit(Entity self, Entity context) override {};
		void postUpdate(Entity self, Entity context) override {};
	};
}

