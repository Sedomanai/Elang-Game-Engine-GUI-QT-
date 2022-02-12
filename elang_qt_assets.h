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


	struct EditorShapeDebug
	{
		EditorPainter point;
		EditorPainter line;
		EditorPainter fill;

		EditorShapeDebug() : mInit(false),
			point("debug2d", "debug", 100000, NullEntity, Projection::eOrtho, Painter::Z_CLEAR),
			line("debug2d", "debug", 100000, NullEntity, Projection::eOrtho, Painter::Z_CLEAR),
			fill("debug2d", "debug", 100000, NullEntity, Projection::eOrtho, Painter::Z_CLEAR)
		{};
		~EditorShapeDebug() {
			if (mInit) {
				point.destroy();
				line.destroy();
				fill.destroy();
			}
		}

		void init(asset<EditorCamera> camera_) {
			if (!mInit) {
				mInit = true;
				fill.camera = point.camera = fill.camera = line.camera = point.camera = asset<Camera>(camera_);
				fill.color = vec4(1, 1, 1, 0.2f);
				point.drawtype = GL_POINTS;
				line.drawtype = GL_LINES;
				point.init();
				line.init();
				fill.init();
			}
		}

		void draw() {
			// order matters
			point.paint();
			line.paint();
			fill.paint();
		}

	private:
		bool mInit;
	};
}

