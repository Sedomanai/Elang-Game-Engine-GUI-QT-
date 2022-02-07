#include "auto_gen.h"


namespace el
{
	AtlasAutoGenerator::AtlasAutoGenerator(QWidget* parent)
		: QDialog(parent), gen(false) {
		ui.setupUi(this);

		connect(ui.generateButton, &QPushButton::clicked, [&]() {
			sortorder = ui.ltrButton->isChecked() ? 0 : 1;
			margin = ui.marginBox->value();
			gen = true;
			close();
		});
	}

	AtlasAutoGenerator::~AtlasAutoGenerator() {}

	void AtlasAutoGenerator::closeEvent(QCloseEvent*) {
		
	}
}