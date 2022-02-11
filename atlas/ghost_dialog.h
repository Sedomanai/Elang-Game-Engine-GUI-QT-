#pragma once

#include <QDialog>
#include "ui_ghost_dialog.h"
#include "aepalette.h"
#include <common/string.h>
#include "atlas_util.h"

namespace il {
	enum class eAtlasGhostType
	{
		NONE,
		PREVIOUS,
		CUSTOM,
		EXTERNAL
	};

	enum class eAtlasGhostOrder
	{
		BACK,
		FRONT
	};

	class GhostDialog : public QDialog
	{
		Q_OBJECT

	public:
		GhostDialog(QWidget* parent = Q_NULLPTR);
		~GhostDialog();

		void open();

		strview externTexKey() { return mExternTexKey; }
		listmap<EditorCell>* externCells() { return &mExternCells; }

		bool confirmed() { return mConfirmed; }
		eAtlasGhostType type;
		eAtlasGhostOrder order;
		uint cell;

		void alignCustomByHotkey(uint cell);
	private:
		bool mConfirmed;
		string mExternTexKey, mExternAtlasKey;
		listmap<EditorCell> mExternCells;
		void customize(bool);
		Ui::GhostDialog ui;
	};
}