#pragma once
#include "cells_widget.h"
#include "aepalette.h"
#include "atlas_util.h"

namespace il
{
	struct ReelFrame
	{
		uint index;
		string cell;
		color col;

		ReelFrame() : col(0, 255, 0, 255) {};
	};

	struct ReelEvent : IButtonEvent
	{
		signal<uint> sig_Select;
		signal<uint> sig_Move;
		signal<uint> sig_Erase;
		void onGrab(uint buttonIndex, ButtonData& data) override;
		void onRelease(uint buttonIndex, ButtonData& data) override;
	};

	struct ClipsView : public QIlangView
	{
		Q_OBJECT
	public:
		ClipsView(QWidget* parent = Q_NULLPTR);
		virtual ~ClipsView();
		void importAtlasBegin();
		void importAtlasEnd();
		void showEditor();
		void hideEditor();

		void init();
		void loop();
		void updateTexture();
		void recreateReel();

		uint safeFrame();
		void addFrame();
		
		signal<uint> sig_ChangeBar;
	signals:
		void sig_WheelEvent(float);
		void sig_Initialized();
	private:
		
		pptr<ReelEvent> revent;
		QListExtension* mClipList;
		
		vector<Entity> mEnts;
		EditorClip* mSelect;
		float mCurrFrame, mSpeed;
		float mDuration;

		Batch mSprite;
		bool mMovingCell;

		Scene mScene;
		prop<Camera> mMainView, mReelView;
		prop<Material> mTexmat;

		void connectMouseInput();
		void connectList();
		void recreateList();
		void reorder();
	};
}

