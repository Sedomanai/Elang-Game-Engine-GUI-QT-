#pragma once
#include "cells_widget.h"
#include <uic/ui_clips_widget.h>
#include <common/random.h>
//#include "aepalette.h"
//#include "atlas_util.h"

namespace el
{
	struct ClipframeHolder
	{
		sizet index;
		aabb rect;
	};

	struct ClipsWidget : public QElangView, public IButtonEvent
	{
		Q_OBJECT

	public:
		ClipsWidget(QWidget* parent = Q_NULLPTR);

		void updateTexture();
		void showEditor();
		void hideEditor();
		void loop();

		void addClip();
		void addFrame();

		QElangView* view() { return ui.view; }
		QElangView* reel() { return ui.frames; }
	private:
		void addReel(Clipframe);

		int mViewWidth, mViewHeight, mFramesWidth, mFramesHeight;

		rng crng;
		Ui::ClipsWidgetUI ui;

		asset<EditorCamera> mMainCamView;
		asset<EditorProjectPainter> mPainterView;
		EditorShapeDebug* mDebugView;

		asset<Texture> mTexture;
		obj<EditorProjectSprite> mClipObj;

		asset<EditorCamera> mMainCamFrames;
		asset<EditorProjectPainter> mPainterFrames;
		EditorShapeDebug* mDebugFrames;
		obj<EditorProjectSprite> mTestObj;

		bool mPaused;
		sizet mFrame;
	
		void updateAllCanvasButton();
		void safeCreateViewObjects();
		void safeCreateFrameObjects();
		void connectList();
		void recreateReel();
		void syncScroll();

		// Inherited via IButtonEvent
		void onEnter(Entity self, Entity context) override;
		void onHover(Entity self, Entity context) override;
		void onExit(Entity self, Entity context) override;
		void postUpdate(Entity self, Entity context) override;

	//	Q_OBJECT
	//public:
	//	ClipsWidget(QWidget* parent = Q_NULLPTR);
	//	virtual ~ClipsWidget();
	//	void importAtlasBegin();
	//	void importAtlasEnd();

	//	void init();
	//	void loop();
	//	void updateTexture();
		

	//	uint safeFrame();
	//	void addFrame();
	//	
	//	signal<uint> sig_ChangeBar;
	//signals:
	//	void sig_WheelEvent(float);
	//	void sig_Initialized();
	//private:
	//	
	//	pptr<ReelEvent> revent;
	//	QListExtension* mClipList;
	//	
	//	vector<Entity> mEnts;
	//	EditorClip* mSelect;
	//	float mCurrFrame, mSpeed;
	//	float mDuration;

	//	Batch mSprite;
	//	bool mMovingCell;

	//	Scene mScene;
	//	prop<Camera> mMainView, mReelView;
	//	prop<Material> mTexmat;


	

		//	void connectMouseInput();
	//	void recreateList();
	//	void reorder();
	};
}

