#include <elqtpch.h>
#include "ghost_dialog.h"

#include "util.h"
#include <tools/project.h>
#include <apparatus/asset_loader.h>
#include <tools/asset.h>
#include <tools/texture.h>
#include <tools/atlas.h>
#include <tools/material.h>

namespace el
{

	void ElangAtlasGhostData::createInternalAssets() {
		if (!mExternal)
			gAtlasUtil.makeEmptyMaterial("__editor_ghost_material_", mExternal, mExternalAtlas);
	}

	ElangAtlasGhostDialog::ElangAtlasGhostDialog(ElangAtlasGhostData & data, QWidget * parent)
		: QDialog(parent), mData(data) {
		ui.setupUi(this);
		setMinimumSize(100, 100);
		setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);

		ui.posBox->setDisabled(true);
		ui.indexBox->setDisabled(true);
		mData.createInternalAssets();

		switch (mData.type) {
			case ElangAtlasGhostData::eType::NONE: ui.noneRadio->setChecked(true); break;
			case ElangAtlasGhostData::eType::PREVIOUS: ui.prevRadio->setChecked(true); break;
			case ElangAtlasGhostData::eType::INDEXED: ui.indexRadio->setChecked(true); break;
			case ElangAtlasGhostData::eType::EXTERNAL: ui.externRadio->setChecked(true); break;
		}
		syncUIWithData();

		connect(ui.noneRadio, &QRadioButton::toggled, [&]() {
			mData.type = ElangAtlasGhostData::eType::NONE;
			mData.material = NullEntity;
			syncUIWithData();
			});

		connect(ui.prevRadio, &QRadioButton::toggled, [&]() {
			mData.type = ElangAtlasGhostData::eType::PREVIOUS;
			mData.material = gAtlasUtil.currentMaterial;
			mData.atlas = gAtlasUtil.currentAtlas;
			syncUIWithData();
			});

		connect(ui.indexRadio, &QRadioButton::toggled, [&]() {
			mData.type = ElangAtlasGhostData::eType::INDEXED;
			mData.material = gAtlasUtil.currentMaterial;
			mData.atlas = gAtlasUtil.currentAtlas;
			syncUIWithData();
			});

		connect(ui.externRadio, &QRadioButton::toggled, [&]() {
			mData.type = ElangAtlasGhostData::eType::EXTERNAL;
			mData.material = mData.mExternal;
			mData.atlas = mData.mExternalAtlas;
			syncUIWithData();
			});

		connect(ui.frontRadio, &QRadioButton::toggled, [&]() {
			mData.order = ElangAtlasGhostData::eOrder::FRONT;
			});

		connect(ui.backRadio, &QRadioButton::toggled, [&]() {
			mData.order = ElangAtlasGhostData::eOrder::BACK;
			});

		connect(ui.texButton, &QPushButton::clicked, [&]() {
			fio::path path =
				QFileDialog::getOpenFileName(this, "External Ghost Texture", gAtlasUtil.lastSearchHistory.generic_string().c_str(), "Texture (*.png)").toStdString();
			if (gAtlasUtil.openTexture(mData.mExternal, path, "The same ghost texture is already loaded"))
				syncUIWithData();
		});

		connect(ui.atlasButton, &QPushButton::clicked, [&]() {
			// was too specific to... specify in gAtlsUtil
			auto mat = mData.material;
			assert(mat && mat->textures[0]);
			auto atlas = mData.atlas;
			assert(atlas);
			auto& data = atlas.get<AssetData>();

			fio::path path =
				QFileDialog::getOpenFileName(this, "Open Atlas", gAtlasUtil.lastSearchHistory.generic_string().c_str(), "Atlas (*.atls)").toStdString();
			if (!path.empty() && data.inode != el_file::identifier(path)) {
				gAtlasUtil.recordLastDirectoryHistory(path);
				auto& meta = atlas.get<AtlasMeta>();
				if (atlas.has<AssetLoaded>())
					atlas->unload(meta);
				else
					atlas.add<AssetLoaded>();

				atlas->importFile(path, meta);
				atlas.get<GUIAsset>().filePath = path.filename();
				data = { el_file::identifier(path), path, fio::last_write_time(path) };

				if (atlas.has<AssetModified>())
					atlas.remove<AssetModified>();
				syncUIWithData();
			} else {
				cout << "Atlas file is already loaded" << endl;
			}
		});

		connect(ui.cellButton, &QPushButton::clicked, [&]() {
			QDialog dialog;
			AtlasPalette palette(&dialog, true);

			assert(mData.material && mData.material->hasTexture());
			palette.updateAtlas(mData.atlas);
			palette.updateMaterial(mData.material, gAtlasUtil.globalPalettePositon, gAtlasUtil.globalPaletteScale);

			palette.sig_Clicked.connect([&](asset<Cell> result) {
				mData.cell = result;
				dialog.close();
				syncUIWithData();
			});

			gAtlasUtil.globalPalettePositon = palette.camPosition();
			gAtlasUtil.globalPaletteScale = palette.camScale();
			dialog.exec();
		});
	}

	void ElangAtlasGhostDialog::syncUIWithData() {
		ui.posBox->setDisabled(mData.type == ElangAtlasGhostData::eType::NONE);
		ui.indexBox->setDisabled(mData.type == ElangAtlasGhostData::eType::NONE || mData.type == ElangAtlasGhostData::eType::PREVIOUS);

		if (ui.posBox->isEnabled() && mData.order == ElangAtlasGhostData::eOrder::FRONT) {
			ui.frontRadio->setChecked(true);
		} else ui.backRadio->setChecked(true);

		if (ui.indexBox->isEnabled()) {
			if (mData.type == ElangAtlasGhostData::eType::INDEXED) {
				auto tex = gAtlasUtil.currentMaterial->textures[0];
				ui.texLabel->setEnabled(false);
				ui.texButton->setEnabled(false);
				ui.atlasLabel->setEnabled(false);
				ui.atlasButton->setEnabled(false);
				ui.texLabel->setText(QString::fromUtf8(tex.get<AssetData>().filePath.filename().generic_u8string()));
				ui.atlasLabel->setText(QString::fromUtf8(gAtlasUtil.currentAtlas.get<AssetData>().filePath.filename().generic_u8string()));
			} else {
				auto & texdata = mData.mExternal->textures[0].get<AssetData>();
				ui.texLabel->setText(QString::fromUtf8(texdata.filePath.filename().generic_u8string()));
				ui.texLabel->setEnabled(true);
				ui.texButton->setEnabled(true);
				if (texdata.inode != -1) {
					ui.atlasLabel->setText(QString::fromUtf8(mData.mExternalAtlas.get<AssetData>().filePath.filename().generic_u8string()));
					ui.atlasLabel->setEnabled(true);
					ui.atlasButton->setEnabled(true);
				} else {
					ui.atlasLabel->setText("");
					ui.atlasLabel->setEnabled(false);
					ui.atlasButton->setEnabled(false);
				}
			} 
		} else {
			ui.texLabel->setText("");
			ui.atlasLabel->setText("");
		} syncCellOnly();
	}

	void ElangAtlasGhostDialog::syncCellOnly() {
		if (mData.cell) {
			auto& sub = mData.cell.get<SubAssetData>();
			auto atlas = mData.atlas;
			if (atlas == sub.parent)
				ui.cellLabel->setText(QString::fromUtf8(sub.name));
			else {
				mData.cell = asset<Cell>();
				ui.cellLabel->setText("");
			}
		}
	}
}