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
		if (!mExternal) {
			mExternal = gProject.make<Material>().add<EditorAsset>();
			auto tex = gProject.make<AssetData>(-1, "", fio::file_time_type()).add<TextureMeta>().add<EditorAsset>().add<Texture>();
			mExternalAtlas = tex->atlas = gProject.make<AssetData>(-1, "", fio::file_time_type()).add<AtlasMeta>().add<EditorAsset>().add<Atlas>();
			mExternal->setTexture(tex);
		}
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
			mData.material = gAtlsUtil.currentMaterial;
			syncUIWithData();
			});

		connect(ui.indexRadio, &QRadioButton::toggled, [&]() {
			mData.type = ElangAtlasGhostData::eType::INDEXED;
			mData.material = gAtlsUtil.currentMaterial;
			syncUIWithData();
			});

		connect(ui.externRadio, &QRadioButton::toggled, [&]() {
			mData.type = ElangAtlasGhostData::eType::EXTERNAL;
			mData.material = mData.mExternal;
			syncUIWithData();
			});

		connect(ui.frontRadio, &QRadioButton::toggled, [&]() {
			mData.order = ElangAtlasGhostData::eOrder::FRONT;
			});

		connect(ui.backRadio, &QRadioButton::toggled, [&]() {
			mData.order = ElangAtlasGhostData::eOrder::BACK;
			});

		connect(ui.texButton, &QPushButton::clicked, [&]() {
			// open texture here
			});

		connect(ui.atlasButton, &QPushButton::clicked, [&]() {
			// open atlas here
			});

		connect(ui.cellButton, &QPushButton::clicked, [&]() {
			QDialog dialog;
			AtlasPalette palette(&dialog, true);

			assert(mData.material && mData.material->hasTexture());
			palette.updateAtlas(mData.material->textures[0]->atlas);
			palette.updateMaterial(mData.material, gAtlsUtil.globalPalettePositon, gAtlsUtil.globalPaletteScale);

			palette.sig_Clicked.connect([&](asset<Cell> result) {
				mData.cell = result;
				ui.cellLabel->setText(QString::fromUtf8(result.get<SubAssetData>().name));
				dialog.close();
				});

			gAtlsUtil.globalPalettePositon = palette.camPosition();
			gAtlsUtil.globalPaletteScale = palette.camScale();
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
				auto tex = gAtlsUtil.currentMaterial->textures[0];
				ui.texLabel->setEnabled(false);
				ui.texButton->setEnabled(false);
				ui.atlasLabel->setEnabled(false);
				ui.atlasButton->setEnabled(false);
				ui.texLabel->setText(QString::fromUtf8(tex.get<AssetData>().filePath.filename().generic_u8string()));
				ui.atlasLabel->setText(QString::fromUtf8(gAtlsUtil.currentAtlas.get<AssetData>().filePath.filename().generic_u8string()));
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
			if (mData.cell)
				ui.cellLabel->setText(QString::fromUtf8(mData.cell.get<SubAssetData>().name));
		} else {
			ui.texLabel->setText("");
			ui.atlasLabel->setText("");
			ui.cellLabel->setText("");
		}
	}
}