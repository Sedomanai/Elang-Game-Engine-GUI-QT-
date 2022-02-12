#include "elang_qt_assets.h"

namespace el
{
	void EditorPainter::setCamera() {
		asset<EditorCamera> cam = camera;
		if (cam) {
			setUniformMatrix(mVert->shader(), ((projection == Projection::ePerspective) ? gPerspective : gOrtho), "uView");
			//setUniformMatrix(mVert->shader(), ((projection == Projection::ePerspective) ? gPerspective : gOrtho) * cam->inversed().matrix(), "uView");
		} else {
			setUniformMatrix(mVert->shader(), ((projection == Projection::ePerspective) ? gPerspective : gOrtho), "uView");
		}
	}

	void EditorPainter::bindMaterial(uint32 material) {
		asset<EditorMaterial> mate(material);
		if (mate) {
			auto& mat = *mate;
			bindMaterialBody(mat);
		}
	}

	void updateAllEditorButtons(asset<EditorCamera> camera) {
		auto pos = *camera * gMouse.currentPosition();
		for (auto e : gStage->view<Button>()) {
			obj<Button> button = e;
			auto rect = obj<Box>(e);
			auto circ = obj<Radius>(e);

			bool hit = ((rect.has() && rect->contains(pos)) ||
				(circ.has() && circ->contains(pos)));
			button->update(button, hit);
		}
	}
}

