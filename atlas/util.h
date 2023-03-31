#pragma once
#include "../elqt/extension/list.h"
#include "../elqt/widget/palette.h"
#include <tools/atlas.h>
#include <tools/texture.h>
#include <tools/material.h>
#include <apparatus/asset_loader.h>

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
		asset<Material> currentMaterial;
		asset<Atlas> currentAtlas;
		QListExtension* cellList, * clipList;
		fio::path lastSearchHistory, backupDirectory;
		vec2 globalPalettePositon;
		float globalPaletteScale;

		AtlasUtility() : cellList(0), clipList(0), globalPalettePositon(-65532.0f, 65532.0f), globalPaletteScale(1.0f) {}
		
		void makeMaterial() {
			makeEmptyMaterial("__editor_atlas_material_", currentMaterial, currentAtlas);
		}

		void makeEmptyMaterial(string name, asset<Material>& mat, asset<Atlas>& atlas) {
			auto tex = gProject.make<AssetData>(-1, fio::path(), fio::file_time_type())
				.add<TextureMeta>().add<Texture>().add<EditorAsset>().add<GUIAsset>("");

			AssetLoader loader;
			mat = loader.createNativeGUIAsset<Material>(name, gMaterials);
			mat->setTexture(tex, 0);

			atlas = gProject.make<AssetData>(-1, fio::path(), fio::file_time_type())
				.add<AtlasMeta>().add<Atlas>().add<EditorAsset>().add<GUIAsset>("");
			tex.get<Texture>().atlas = atlas;
			atlas.get<AtlasMeta>().self = atlas;
		}

		void recordLastDirectoryHistory(el::fio::path& path) {
			lastSearchHistory = path.parent_path();
			string out = "Last Search History=";
			out.append(lastSearchHistory.generic_string());
			el_file::save("../___gui/dat/data.config", out);
		}

		bool openTexture(asset<Material> mat, el::fio::path& path, string debugMessage = "The same texture file is already loaded") {
			auto tex = mat->textures[0];
			auto& data = tex.get<AssetData>();
			if (!path.empty() && data.inode != el_file::identifier(path)) {
				recordLastDirectoryHistory(path);

				auto& meta = tex.get<TextureMeta>();
				if (tex.has<AssetLoaded>())
					tex->unload(meta);
				else {
					tex.add<AssetLoaded>();
				}

				tex->importFile(path, meta);
				tex.get<GUIAsset>().filePath = path.filename();
				data = { el_file::identifier(path), path, fio::last_write_time(path) };
				return true;
			}
			else {
				cout << debugMessage << endl;
			} return false;
		}
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
	inline AtlasUtility gAtlasUtil;
}
