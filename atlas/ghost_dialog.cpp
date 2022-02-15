#include "ghost_dialog.h"
#include <qbuttongroup.h>
#include <qfiledialog.h>

namespace el {

	ElangAtlasGhostDialog::ElangAtlasGhostDialog(QWidget *parent)
		: QDialog(parent)
	{
		ui.setupUi(this);
		ui.posBox->setDisabled(true);
		ui.customBox->setDisabled(true);
		if (!mExternal) {
			mExternal = gProject->makeSub<EditorProjectMaterial>();
		}

		connect(ui.typeGroup, qOverload<QAbstractButton*, bool>(&QButtonGroup::buttonToggled), [&](QAbstractButton* button, bool) {
			ui.posBox->setDisabled(button == ui.noneRadio);
			ui.customBox->setDisabled((button == ui.noneRadio || button == ui.prevRadio));
			if (ui.customBox->isEnabled()) {
				customize((button != ui.customRadio));
			}
		});

		connect(ui.noneRadio, &QRadioButton::toggled, [&]() {
			mData.type = ElangAtlasGhostData::eType::NONE;
			mData.material = NullEntity;
		});

		connect(ui.prevRadio, &QRadioButton::toggled, [&]() {
			mData.type = ElangAtlasGhostData::eType::PREVIOUS;
			mData.material = gAtlsUtil.currentMaterial;
		});

		connect(ui.customRadio, &QRadioButton::toggled, [&]() {
			mData.type = ElangAtlasGhostData::eType::CUSTOM;
			mData.material = gAtlsUtil.currentMaterial;
		});

		connect(ui.externRadio, &QRadioButton::toggled, [&]() {
			mData.type = ElangAtlasGhostData::eType::EXTERNAL;
			mData.material = mExternal;

			if (ui.texture->count() == 0) {
				for (auto it : gGUI.project.textures) {
					ui.texture->addItem(QString::fromStdString(it.first));
				}
			}
		});

		connect(ui.frontRadio, &QRadioButton::toggled, [&]() {
			mData.order = ElangAtlasGhostData::eOrder::FRONT;
			});

		connect(ui.backRadio, &QRadioButton::toggled, [&]() {
			mData.order = ElangAtlasGhostData::eOrder::BACK;
		});

		connect(ui.texture, &QComboBox::currentTextChanged, [&](const QString& text) {
			if (gProject->textures.contains(text.toStdString())) {
				assert(mData.material);
				mData.material->setTexture(gProject->textures[text.toStdString()]);
				ui.texLabel->setText(text);
			}
		});
		connect(ui.cellButton, &QPushButton::clicked, [&]() {
			QDialog dialog(this);
			AtlasPalette palette(&dialog, true);
			Stage stage;
			palette.view()->setStage(&stage);

			palette.view()->sig_Start.connect([&]() {
				palette.updateMaterial(mData.material);
				});
			palette.sig_Clicked.connect([&](CellResult result) {
				mData.cell = result.cell;
				assert(mData.material && mData.material->hasTexture());
				auto tex = mData.material->textures[0];
				assert(tex && tex->atlas);
				assert(tex->atlas->cells.contains(mData.cell));
				ui.cellLabel->setText(QString::fromUtf8(tex->atlas->cells[mData.cell]));
				dialog.close();
			});

			dialog.exec();
		});

		connect(ui.confirmButton, &QPushButton::clicked, [&]() {
			mConfirmed = true;
			close();
		});
		connect(ui.cancelButton, &QPushButton::clicked, [&]() {
			mConfirmed = false;
			close();
		});
	}

	void ElangAtlasGhostDialog::open() {
		mConfirmed = false;
		exec();	
	}

	//void ElangAtlasGhostDialog::alignCustomByHotkey(uint cell_) {
	//	if (cell_ != -1) {
	//		cell = cell_;
	//		ui.customRadio->toggle();
	//		ui.paletteEdit->setText(QString::fromStdString(gCells.nameAt(cell_)));
	//	}
	//}

	void ElangAtlasGhostDialog::customize(bool value) {
		ui.texLabel->setEnabled(value);
		ui.texture->setEnabled(value);

		//ui.cellButton->setEnabled(true);
		//ui.cellLabel->setEnabled(true);

		if (!value) {
			ui.cellLabel->setText("");
			//ui.texture->setText("");
		}
	}

	ElangAtlasGhostDialog::~ElangAtlasGhostDialog()
	{

	}
}