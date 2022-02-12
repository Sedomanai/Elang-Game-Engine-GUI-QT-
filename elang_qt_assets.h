#pragma once

#include <common/define.h>
#include <apparatus/atelier.h>
#include <apparatus/ui.h>

namespace el
{

	struct EditorCamera : Camera {
		EL_USING_BASE_CLASS(EditorCamera, Camera);
	};
	struct EditorMaterial : Material { 
		EL_USING_BASE_CLASS(EditorMaterial, Material); 
	};
	struct EditorPainter : Painter
	{
		EL_USING_BASE_CLASS(EditorPainter, Painter);
	protected:
		void setCamera() override;
		void bindMaterial(uint32 material) override;
	};

	void updateAllEditorButtons(asset<EditorCamera> camera);

	using EditorSprite = SpriteImpl<EditorMaterial, EditorPainter>;
}

