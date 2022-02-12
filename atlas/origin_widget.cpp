#include "origin_widget.h"

namespace el
{
	OriginView::OriginView(QWidget* parent)
		: QElangView(parent) //, mClicked(false), mSelect(0), mGhost(0), mGhostRow(-1)
	{
		//setFocusPolicy(Qt::StrongFocus);
		setMouseTracking(true);
		//mGhostDialog = new GhostDialog(this);


		//	asset<Texture> ghosttex;
		//	switch (mGDialog->type) {
		//	case eAtlasGhostType::PREVIOUS:
		//	case eAtlasGhostType::CUSTOM:
		//		ghosttex = tex;
		//		break;
		//	case eAtlasGhostType::EXTERNAL:
		//		ghosttex = gTextures.find(string(mGDialog->externTexKey()));
		//		break;
		//	}

		//	if (ghosttex) {
		//		switch (mGDialog->order) {
		//		case eAtlasGhostOrder::BACK:
		//			batch(mGhost, mGhostBatch, "Ghost Painter Back", ghosttex);
		//			break;
		//		case eAtlasGhostOrder::FRONT:
		//			batch(mGhost, mGhostBatch, "Ghost Painter Front", ghosttex);
		//			break;
		//		}
		//	}
	}

	OriginView::~OriginView() {
		//if (mInitialized) {
		//	mTexMat->reset();
		//	mExtMat->reset();
		//	free(mCellBatch.vertices);
		//	free(mGhostBatch.vertices);
		//}
	}

	void OriginView::onViewStart() {
		connectList();

		glClearColor(0.2f, 0.2f, 0.5f, 1.0f);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		mMainCam = gProject->cameras["Main Camera"];
		mMainCam->to(vec3(0.0f, 0.0f, -1000.0f));

		mSpritePainter = gProject->painters["Sprite Painter"];

		static bool makeOnce = true;
		if (makeOnce) {
			mTexObj = gStage->make<Sprite>(gAtlsUtil.currentMaterial, mSpritePainter, "");
			mTexObj.add<Position>().update();
			makeOnce = false;
		}
	}

	void OriginView::onViewPaint()
	{		
		glClearDepth(1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		updateViewport(-width() / 2, width() / 2, -height() / 2, height() / 2);

		if (mTexture && mTexture->atlas) {
			if (gAtlsUtil.cellList->count() > 0) {
				CellItem* item = reinterpret_cast<CellItem*>(gAtlsUtil.cellList->currentItem());
				mTexObj->setCell(item->text().toStdString());
				mTexObj->update(mTexObj); // move to on cell change, maybe
				mTexObj->batch();
				mSpritePainter->paint();
			}
		}

	}

	void OriginView::connectList() {
		connect(gAtlsUtil.cellList, &QListExtension::currentRowChanged, [&]() {
			//bind(mStage);
			if (isVisible()) {
				//CellItem* item = reinterpret_cast<CellItem*>(gAtlsUtil.cellList->currentItem());
				//mTexObj->setCell(item->text().toStdString());
				//mTexObj->update(mTexObj);
				////if (row >= 0) {
				////	mSelect = &gCells.at(row);
				////	if (mGDialog->type == eAtlasGhostType::PREVIOUS) {
				////		if (row > 0) {
				////			mGhost = &gCells.at(row - 1);
				////		} else mGhost = 0;
				////	}
				////} else {
				////	mSelect = 0;
				////	mGhost = 0;
				////}
				setFocus();
				update();
			}
		});
	}

	void OriginView::updateTexture() {
		auto mat = gAtlsUtil.currentMaterial;
		if (mat && mat->hasTexture()) {
			mTexture = mat->textures[0];
		}
	}
	void OriginView::hideEditor() {
		//gCellRow = gCellList->currentRow();
		//switch (mGDialog->type) {
		//case eAtlasGhostType::CUSTOM:
		//case eAtlasGhostType::PREVIOUS:
		//	if (mGhost)
		//		mGhostRow = gCellList->row(mGhost->item);
		//	mGhost = 0;
		//}

		//mSelect = 0;
		//gCellList->clearSelection();
		//gCellList->setCurrentRow(-1);
		//gCellList->hide();
		hide();
	}

	//void OriginView::setGhost() {
	//	mGDialog->open();
	//	
	//	if (mGDialog->confirmed()) {
	//		switch (mGDialog->type) {
	//		case eAtlasGhostType::NONE:
	//			mGhost = 0;
	//			break;
	//		case eAtlasGhostType::PREVIOUS:
	//			mGhostBatch.material = mTexMat;
	//		case eAtlasGhostType::CUSTOM:
	//			if (mGDialog->cell != -1) {
	//				mGhost = &(gCells.at(mGDialog->cell));
	//			}
	//			break;
	//		case eAtlasGhostType::EXTERNAL:
	//			mGhostBatch.material = mExtMat;
	//			if (mGDialog->cell != -1) {
	//				auto tex = gTextures.find(string(mGDialog->externTexKey()));
	//				if (tex) {
	//					mExtMat->textures[0] = tex;
	//					mGhost = &(mGDialog->externCells()->at(mGDialog->cell));
	//				} else cout << "External texture does not exist. Check its URL";
	//			}
	//			break;
	//		}
	//	}

	//	// For SOME unknowable reason, touching external ghosts interferes with the current texture and blackens it out
	//	// This reloads the existing texture should that hapen
	//	emit sig_SafeTexture();
	//}

	void OriginView::showEditor() {
		bind(mStage);
		auto& list = *gAtlsUtil.cellList;
		list.setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
		list.setDragDropMode(QAbstractItemView::DragDropMode::NoDragDrop);
		list.setDefaultDropAction(Qt::DropAction::IgnoreAction);
		list.show();
		show();

		//if (!mGhost) {
		//	if (gCellList->count() > 0)
		//		mGhostRow = clamp(mGhostRow, 0, gCells.count() - 1);
		//	else mGhostRow = -1;
		//	if (mGhostRow != -1)
		//		mGhost = &gCells.at(mGhostRow);
		//}

		//if (gCellList->count() > 0) {
		//	gCellRow = clamp(gCellRow, 0, gCellList->count() - 1);
		//} else gCellRow = -1;
		//gCellList->setCurrentRow(gCellRow);
	}



	//void OriginView::moveCellOrigin(int x, int y) {
	//	if (mSelect) {
	//		mSelect->ox += x;
	//		mSelect->oy += y;
	//	} update();
	//}

	//void OriginView::captureGhost() {
	//	mGhostBatch.material = mTexMat;

	//	int row = gCellList->currentRow();
	//	if (row != -1) {
	//		mGDialog->alignCustomByHotkey(row);
	//		if (mGDialog->cell != -1) {
	//			mGhost = &(gCells.at(mGDialog->cell));
	//		}
	//	}
	//	update();
	//}

	//void OriginView::shiftCell(int dir) {
	//	int row = gCellList->currentRow() + dir;
	//	row = clamp(row, 0, gCells.count() - 1);
	//	gCellList->setCurrentRow(row);
	//	update();
	//}

	//void OriginView::batch(EditorCell* ecell, const Batch& batch, strview painter, asset<Texture> tex) {
	//	if (ecell) {
	//		SpriteVertex* verts = (SpriteVertex*)batch.vertices;
	//		auto& data = *ecell;
	//		auto& cell = Cell(data.x, data.y, data.w, data.h, data.ox, data.oy, tex->width, tex->height);

	//		//safety net
	//		auto left = round(cell.left);
	//		auto right = round(cell.right);
	//		auto up = round(cell.up);
	//		auto down = round(cell.down);

	//		verts[0].pos = vec2(left, up);
	//		verts[0].uv = vec2(cell.uvLeft, cell.uvUp);
	//		verts[1].pos = vec2(right, up);
	//		verts[1].uv = vec2(cell.uvRight, cell.uvUp);
	//		verts[2].pos = vec2(right, down);
	//		verts[2].uv = vec2(cell.uvRight, cell.uvDown);
	//		verts[3].pos = vec2(left, down);
	//		verts[3].uv = vec2(cell.uvLeft, cell.uvDown);

	//		mScene.atelier.get(string(painter))->batch(SpriteVertexData::sVertexDataIndex, batch);
	//	}
	//}


	//void OriginView::drawMidLines() {
	//	auto w = 100000.0f;
	//	auto h = 100000.0f;
	//	Primitive2DVertex* midlines = (Primitive2DVertex*)malloc(sizeof(Primitive2DVertex) * 4);
	//	uint* midind = (uint*)malloc(sizeof(uint) * 4);
	//	midlines[0].pos = vec2(-w, 0);
	//	midlines[0].col = color(255, 255, 255, 255);
	//	midlines[1].pos = vec2(w, 0);
	//	midlines[1].col = color(255, 255, 255, 255);
	//	midlines[2].pos = vec2(0, -h);
	//	midlines[2].col = color(255, 255, 255, 255);
	//	midlines[3].pos = vec2(0, h);
	//	midlines[3].col = color(255, 255, 255, 255);
	//	midind[0] = 0;
	//	midind[1] = 1;
	//	midind[2] = 2;
	//	midind[3] = 3;
	//	mScene.debug.line.batch(Primitive2DVertexData::sVertexDataIndex, Batch(midlines, 4, midind, 4, -1, 0, E_DANGLING_VERTICES));
	//}

	//void OriginView::connectMouseInput() {
	//	connect(this, &OriginView::sig_MousePress, [&]() {
	//		if (gMouse.state(0) == eInput::ONCE && cursor() == Qt::OpenHandCursor) {
	//			mScene.makeCurrent();
	//			setCursor(Qt::ClosedHandCursor);
	//			mGrabPos = gMouse.currentPosition();
	//			mGrabUV = vec2(mSelect->ox, mSelect->oy);
	//			update();
	//		}
	//	});
	//	connect(this, &OriginView::sig_MouseRelease, [&]() {
	//		if (gMouse.state(0) == eInput::LIFT && cursor() == Qt::ClosedHandCursor) {
	//			setCursor(Qt::OpenHandCursor);
	//			update();
	//		}
	//	});
	//	connect(this, &OriginView::sig_MouseMove, [&]() {
	//		mScene.makeCurrent();
	//		Camera& viewport = *mFuckingViewport;

	//		vec2 mpos = gMouse.currentPosition();
	//		if (cursor() == Qt::ClosedHandCursor) {
	//			auto delta = viewport * (mpos - mGrabPos);
	//			mSelect->ox = int(delta.x + mGrabUV.x);
	//			mSelect->oy = int(-delta.y + mGrabUV.y);
	//			update();
	//		} else {
	//			auto tex = gTextures.find(gTexKey);
	//			aabb rect;
	//			if (mSelect) {
	//				auto& data = *mSelect;
	//				auto& cell = Cell(data.x, data.y, data.w, data.h, data.ox, data.oy, tex->width, tex->height);
	//				rect = aabb(cell.left, cell.down, cell.right, cell.up);
	//			}

	//			setCursor(Qt::ArrowCursor);
	//			if (rect.contains(viewport * mpos)) {
	//				setCursor(Qt::OpenHandCursor);
	//			}
	//		}
	//	});
	//	connect(this, &OriginView::sig_ScrollWheel, [&]() {
	//		if (cursor() != Qt::ClosedHandCursor) {
	//			float val = (5.0f / 6.0f);
	//			val = pow(val, gMouse.wheel());
	//			if (val != 0) {
	//				val = (val > 0) ? val : -1.0f / val; // change
	//				mFuckingViewport->scale(vec3(val, val, 1));
	//			};
	//		}
	//	});
	//}
}