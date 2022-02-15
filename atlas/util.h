#pragma once
#include <common/string.h>
#include <common/shape2d.h>
#include <qlistwidget.h>
#include <qdir.h>
#include <tools/material.h>

#include "../elqt/extension/list.h"
#include "../elqt/widget/palette.h"

namespace el
{
	struct CellItem : QListWidgetItem
	{
		CellItem(QListWidget* parent) : QListWidgetItem(parent) {};
		obj<CellHolder> holder;
	};

	struct ClipItem : QListWidgetItem
	{
		ClipItem(QListWidget* parent) : QListWidgetItem(parent) {};
		asset<Clip> clip;
	};

	struct AtlasUtility
	{
		asset<EditorProjectMaterial> currentMaterial;
		asset<EditorProjectMaterial> ghostMaterial;
		// Entity must be obj<CellHolder>
		//hashmap<Entity, CellItem*> cellItems;
		QListExtension* cellList, * clipList;
		float resolution = 1.0f;
	};

	inline AtlasUtility gAtlsUtil;

	//using EditorClip = vector<string>;
	//inline string gTexKey;
	//inline QDir gProjectDir;


	// TODO: Completely deprecate
	//inline void loadAtlasToCells(strview data, listmap<EditorCell>* cells, listmap<EditorClip>* clips = 0) {
	//	int x, y, w, h, ox, oy;
	//	float aw, ah;
	//	string strname;
	//	string act;
	//	string framename;
	//	EditorClip* clip = 0;
	//	iterate(data, '\n', [&](strview line, uint) {
	//		iterate(line, ' ', [&](strview word, uint iter) {
	//			switch (iter) {
	//			case 0: act = word; break;
	//			case 1:
	//				if (act == "width") {
	//					aw = toInt(word) / 2.0f;
	//				} else if (act == "height") {
	//					ah = toInt(word) / 2.0f;
	//				} else if (act == "\t@a") {
	//					if (clips) {
	//						clips->emplace(string(word));
	//						clip = &clips->at(string(word));
	//					}
	//					return;
	//				}
	//				break;
	//			default:
	//				if (act == "\t@c") {
	//					switch (iter) {
	//					case 2: x = toInt(word); break;
	//					case 3: y = toInt(word); break;
	//					case 4: w = toInt(word); break;
	//					case 5: h = toInt(word); break;
	//					case 6: ox = toInt(word); break;
	//					case 7: oy = toInt(word); break;
	//					case 9: strname = word;
	//						cells->emplace(strname, aabb(x, -y, x + w, -(y + h)), strname);
	//						auto& cell = cells->back();
	//						cell.setAbsData();
	//						cell.rect.normalize();
	//						cell.ox = ox;
	//						cell.oy = oy;
	//						break;
	//					}
	//				} else if (act == "\t@a") {
	//					if (clips) {
	//						if (iter % 2 == 0) {
	//							framename = cells->nameAt(toUint(word));
	//						} else {
	//							for (uint i = 0; i < toUint(word); i++) {
	//								clip->push_back(framename);
	//							}
	//						}
	//					}
	//				}
	//				break;
	//			}
	//		});
	//	});
	//};
}
