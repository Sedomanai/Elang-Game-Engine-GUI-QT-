#pragma once

#include <elements/button.h>

#include "texture_widget.h"

namespace el {

	class _ELANGQT_EXPORT QElangPaletteWidget : public QElangTextureWidget, public IButtonEvent
	{
		Q_OBJECT

	public:
		QElangPaletteWidget(QWidget* parent = Q_NULLPTR);
		~QElangPaletteWidget() {
			delete mCellShapes;
			delete mHighlight;
		}

		asset<Atlas> atlas;
		single_signal<asset<Cell>> sig_Clicked;
	private:
		ShapeDebug* mCellShapes, * mHighlight;
		void coloring(Box& box);
		void onTextureUpdate(asset<Texture>) override;
		void onHover(Entity self, Entity context) override;
		void onEnter(Entity self, Entity context) override {};
		void onExit(Entity self, Entity context) override {};
		void postUpdate(Entity self, Entity context) override {};
	};
} 
