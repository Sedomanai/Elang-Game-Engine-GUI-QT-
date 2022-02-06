#pragma once

#include <elements/button.h>

#include "texture_widget.h"

namespace el {

	class _ELANGQT_EXPORT QElangPaletteWidget : public QElangTextureWidget, public IButtonEvent
	{
		Q_OBJECT

	public:
		QElangPaletteWidget(QWidget *parent = Q_NULLPTR);

		single_signal<asset<Cell>> sig_Clicked;
		asset<Atlas> atlas;
	private:
		void onTextureUpdate(asset<Texture>) override;
		void onHover(Entity self, Entity context) override;
		void onEnter(Entity self, Entity context) override {};
		void onExit(Entity self, Entity context) override {};
		void postUpdate(Entity self, Entity context) override {};
	};
} 
