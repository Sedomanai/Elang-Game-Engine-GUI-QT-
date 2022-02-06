#include "palette_widget.h"

namespace el {

	QElangPaletteWidget::QElangPaletteWidget(QWidget* parent)
		: QElangTextureWidget(parent)
	{
		//ui.setupUi(this);
		ui.view->sig_Start.connect([&]() {
			mStage.storage<asset<Cell>>().reserve(300);
			mStage.storage<Box>().reserve(300);
			mStage.storage<Button>().reserve(300);
		});
	}

	void QElangPaletteWidget::onTextureUpdate(asset<Texture> tex) {
		bind(mStage);
		for (auto e : gStage->view<Button>()) {
			gStage->destroy(e);
		}

		if (tex->atlas) {
			atlas = tex->atlas;
			auto& cells = atlas->cells;
			for (auto it : cells) {
				auto& cell = *asset<Cell>(it.second);
				Box rect;
				rect.l = cell.uvLeft * tex->width();
				rect.r = cell.uvRight * tex->width();
				rect.b = -cell.uvDown * tex->height();
				rect.t = -cell.uvUp * tex->height();

				auto obj = gStage->make<Box>(rect);
				obj.add<asset<Cell>>(it.second);
			}
		}
	}

	void QElangPaletteWidget::onHover(Entity self, Entity context) {
		bind(mStage);
		if (gMouse.state(0) == eInput::ONCE) {
			auto celldata = obj<asset<Cell>>(self);
			if (celldata) {
				sig_Clicked.invoke(*celldata);
			}
		}
	}

} // namespace il
