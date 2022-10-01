#pragma once
#include "../elqt/extension/list.h"
#include "../elqt/widget/palette.h"

namespace el
{
	struct Clip;
	struct CellItem : QListWidgetItem
	{
		CellItem(QListWidget* parent) : QListWidgetItem(parent) {};
		asset<CellHolder> holder;
	};

	struct ClipItem : QListWidgetItem
	{
		ClipItem(QListWidget* parent) : QListWidgetItem(parent) {};
		asset<Clip> clip;
	};

	struct AtlasUtility
	{
		asset<Material> currentMaterial, ghostMaterial;
		asset<Atlas> currentAtlas, ghostAtlas;
		QListExtension* cellList, * clipList;
		fio::path lastSearchHistory, backupDirectory;
		vec2 globalPalettePositon;
		float globalPaletteScale;

		AtlasUtility() : cellList(0), clipList(0), globalPalettePositon(-65532.0f, 65532.0f), globalPaletteScale(1.0f) {}

	};

	enum class AtlasViewMode
	{
		Cells,
		Pivot,
		Clips
	};

	struct AtlasCurrentCell {};
	struct AtlasSelectedCell {};
	struct AtlasMovingCell { aabb capturedRect; };
	struct AtlasLastModifiedReelHolder {};
	inline AtlasUtility gAtlsUtil;
}
