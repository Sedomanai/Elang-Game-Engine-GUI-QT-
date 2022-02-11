#include "ghost_dialog.h"
#include <qbuttongroup.h>
#include <qfiledialog.h>

namespace il {
	GhostDialog::GhostDialog(QWidget *parent)
		: QDialog(parent), mConfirmed(false), type(eAtlasGhostType::NONE), order(eAtlasGhostOrder::BACK), cell(-1)
	{
		ui.setupUi(this);
		ui.posBox->setDisabled(true);
		ui.customBox->setDisabled(true);
		connect(ui.typeGroup, qOverload<QAbstractButton*, bool>(&QButtonGroup::buttonToggled), [&](QAbstractButton* button, bool) {
			ui.posBox->setDisabled(button == ui.noneRadio);
			ui.customBox->setDisabled((button == ui.noneRadio || button == ui.prevRadio));
			if (ui.customBox->isEnabled()) {
				customize((button != ui.customRadio));
			}
		});

		connect(ui.noneRadio, &QRadioButton::toggled, [&]() {
			type = eAtlasGhostType::NONE;
			});

		connect(ui.prevRadio, &QRadioButton::toggled, [&]() {
			type = eAtlasGhostType::PREVIOUS;
			});

		connect(ui.customRadio, &QRadioButton::toggled, [&]() {
			type = eAtlasGhostType::CUSTOM;
			});

		connect(ui.externRadio, &QRadioButton::toggled, [&]() {
			type = eAtlasGhostType::EXTERNAL;
			});

		connect(ui.frontRadio, &QRadioButton::toggled, [&]() {
			order = eAtlasGhostOrder::FRONT;
			});

		connect(ui.backRadio, &QRadioButton::toggled, [&]() {
			order = eAtlasGhostOrder::BACK;
			});

		connect(ui.textureButton, &QPushButton::clicked, [&]() {
			auto filePath = QFileDialog::getOpenFileName(this, tr("External Texture"), gAssetsDir.path(), tr("PNG (*.png)"));
			mExternTexKey = gAssetsDir.relativeFilePath(filePath).toStdString();
			gTextures.load(filePath.toStdString(), mExternTexKey);
			ui.textureEdit->setText(QString::fromStdString(mExternTexKey));
		});
		connect(ui.atlasButton, &QPushButton::clicked, [&]() {
			auto filePath = QFileDialog::getOpenFileName(this, tr("External Atlas"), gAssetsDir.path(), tr("ATLAS (*.atls)"));
			mExternAtlasKey = gAssetsDir.relativeFilePath(filePath).toStdString();
			ui.atlasEdit->setText(QString::fromStdString(mExternAtlasKey));
			mExternAtlasKey = filePath.toStdString();
		});

		connect(ui.paletteButton, &QPushButton::clicked, [&]() {
			switch (type) {
			case eAtlasGhostType::CUSTOM:
				
				gPalette->open(gTexKey, &gCells);
				cell = gPalette->selectedCellIndex;
				if (cell != -1)
					ui.paletteEdit->setText(QString::fromStdString(gCells.nameAt(cell)));
				else ui.paletteEdit->setText("");
				break;

			case eAtlasGhostType::EXTERNAL:
				mExternCells.clear();

				string data;
				loadFile(mExternAtlasKey, data);
				loadAtlasToCells(data, &mExternCells);
				gPalette->open(mExternTexKey, &mExternCells);
				cell = gPalette->selectedCellIndex;

				if (cell != -1)
					ui.paletteEdit->setText(QString::fromStdString(mExternCells.nameAt(cell)));
				else ui.paletteEdit->setText("");
				break;
			}
		});

		connect(ui.confirmButton, &QPushButton::clicked, [&]() {
			mConfirmed = true;
			close();
		});
	}

	GhostDialog::~GhostDialog()
	{

	}

	void GhostDialog::open() {
		mConfirmed = false;
		exec();	
	}

	void GhostDialog::alignCustomByHotkey(uint cell_) {
		if (cell_ != -1) {
			cell = cell_;
			ui.customRadio->toggle();
			ui.paletteEdit->setText(QString::fromStdString(gCells.nameAt(cell_)));
		}
	}

	void GhostDialog::customize(bool value) {
		ui.atlasButton->setEnabled(value);
		ui.atlasEdit->setEnabled(value);
		ui.textureButton->setEnabled(value);
		ui.textureEdit->setEnabled(value);

		if (!value) {
			ui.atlasEdit->setText("");
			ui.textureEdit->setText("");
		}
	}
}