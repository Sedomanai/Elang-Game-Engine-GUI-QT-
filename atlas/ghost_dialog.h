#pragma once
#include <tools/project.h>
#include <uic/ui_q_ghost_dialog.h>

namespace el
{
	struct Cell;
	struct Material;
	struct Atlas;
	class ElangAtlasGhostDialog;
	struct ElangAtlasGhostData
	{
		friend class ElangAtlasGhostDialog;

		enum class eType
		{
			NONE,
			PREVIOUS,
			INDEXED,
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
		asset<Material> material;
		asset<Atlas> atlas;

	private:
		void createInternalAssets();
		asset<Material> mExternal;
		asset<Atlas> mExternalAtlas;
	};

	class ElangAtlasGhostDialog : public QDialog
	{
		Q_OBJECT

	public:
		ElangAtlasGhostDialog(ElangAtlasGhostData& data, QWidget* parent = Q_NULLPTR);
	private:
		void syncUIWithData();
		void syncCellOnly();
		bool suppressSelect;
		ElangAtlasGhostData& mData;
		bool mConfirmed;
		Ui::GhostDialogUI ui;
	};
}