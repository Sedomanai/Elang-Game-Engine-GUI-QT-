#pragma once

#include <QDialog>
#include <common/string.h>

#include "util.h"
#include "../elqt/widget/palette.h"

#include <uic/ui_ghost_dialog.h>

namespace el {

	struct ElangAtlasGhostData
	{
		enum class eType
		{
			NONE,
			PREVIOUS,
			CUSTOM,
			EXTERNAL
		};

		enum class eOrder
		{
			BACK,
			FRONT
		};

		ElangAtlasGhostData() : type(eType::NONE), order(eOrder::BACK), cell(NullEntity), material(NullEntity) {};

		eType type;
		eOrder order;
		asset<Cell> cell;
		asset<EditorProjectMaterial> material;
	};

	class ElangAtlasGhostDialog : public QDialog
	{
		Q_OBJECT

	public:
		ElangAtlasGhostDialog(QWidget* parent = Q_NULLPTR);
		~ElangAtlasGhostDialog();

		void open();
		bool confirmed() { return mConfirmed; }
		ElangAtlasGhostData data() { return mData; }

	private:
		asset<EditorProjectMaterial> mExternal;
		ElangAtlasGhostData mData;
		bool mConfirmed;
		void customize(bool);
		Ui::GhostDialogUI ui;
	};
}