#include "origin_widget.h"

namespace el
{
	OriginView::OriginView(QWidget* parent)
		: QElangView(parent) //, mClicked(false), mSelect(0), mGhost(0), mGhostRow(-1)
	{
		setFocusPolicy(Qt::StrongFocus);
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

		mMainCam = gProject->makeSub<EditorCamera>();
		mMainCam->to(vec3(0.0f, 0.0f, -1000.0f));

		mPainter = gProject->makeSub<EditorProjectPainter>("basic_sprite", "texture_uv", 32, mMainCam, Projection::eOrtho,
			ePainterFlags::DEPTH_SORT | ePainterFlags::MULTI_MATERIAL | ePainterFlags::Z_CLEAR);
		mPainter->init();

		mHighlighter = new EditorShapeDebug;
		mHighlighter->init(mMainCam);

		mCellObj = gStage->make<EditorProjectSprite>(gAtlsUtil.currentMaterial, mPainter, "");
		mCellObj.add<Position>();

	}

	void OriginView::paintGhostCell() {
		switch (mGhostData.type) {
		case ElangAtlasGhostData::eType::NONE:
		none_label:
			mCellObj->material = NullEntity;
			mCellObj->setCell(asset<Cell>());
			break;
		case ElangAtlasGhostData::eType::PREVIOUS:
			{
				CellItem* gitem = reinterpret_cast<CellItem*>
					(gAtlsUtil.cellList->item(gAtlsUtil.cellList->currentRow() - 1));
				if (gitem) {
					mCellObj->material = gAtlsUtil.currentMaterial;
					mCellObj->setCell(gitem->text().toStdString());
				}
				else goto none_label;
			}
			break;
		default:
			mCellObj->material = mGhostData.material;
			mCellObj->setCell(mGhostData.cell);	
			break;
		}

		mCellObj->update(mCellObj);
		mCellObj->batch();
		mPainter->color = vec4(1.0f, 1.0f, 1.0f, 0.35f);
		mPainter->paint();
	}

	void OriginView::onViewPaint()
	{		
		glClearDepth(1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		updateViewport(-width() / 2, width() / 2, -height() / 2, height() / 2);

		if (mTexture && mTexture->atlas) {
			if (gAtlsUtil.cellList->count() > 0) {
				CellItem* item = reinterpret_cast<CellItem*>(gAtlsUtil.cellList->currentItem());
				if (item) {
					if (mGhostData.order == ElangAtlasGhostData::eOrder::BACK)
						paintGhostCell();

					mCellObj->material = gAtlsUtil.currentMaterial;
					mCellObj->setCell(item->text().toStdString());
					mCellObj->update(mCellObj); // move to on cell change, maybe
					mCellObj->batch();
					mPainter->color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
					mPainter->paint();

					aabb rect;
					mCellObj->sync(rect);
					mHighlighter->line.batchAABB(rect, color8(255, 255, 255, 255));

					if (mGhostData.order == ElangAtlasGhostData::eOrder::FRONT)
						paintGhostCell();

					mHighlighter->line.batchline(line(-1000.0f, 0.0f, 1000.0f, 0.0f), color8(255, 255, 255, 255));
					mHighlighter->line.batchline(line(0.0f, -1000.0f, 0.0f, 1000.0f), color8(255, 255, 255, 255));
					mHighlighter->draw();

				}
			}
		}

	}

	void OriginView::onViewMousePress() {	
		if (gMouse.state(0) == eInput::ONCE && cursor() == Qt::OpenHandCursor) {
			CellItem* item = reinterpret_cast<CellItem*>(gAtlsUtil.cellList->currentItem());
			assert(item && item->holder);
			setCursor(Qt::ClosedHandCursor);
			mGrabPos = *mMainCam * gMouse.currentPosition();
			mGrabUV = vec2(item->holder->cell->oX, item->holder->cell->oY);
			update();
		}
	}

	void OriginView::onViewMouseRelease() {
		if (gMouse.state(0) == eInput::LIFT && cursor() == Qt::ClosedHandCursor) {
			setCursor(Qt::OpenHandCursor);
			update();
		}
	}

	void OriginView::onViewMouseMove()
	{	
		CellItem* item = reinterpret_cast<CellItem*>(gAtlsUtil.cellList->currentItem());
		if (item && item->holder) {
			vec2 mpos = *mMainCam * gMouse.currentPosition();

			if (cursor() == Qt::ClosedHandCursor) {
				auto delta = (mpos - mGrabPos);//*mMainCam * (mpos - mGrabPos);
				item->holder->cell->oX = int(delta.x + mGrabUV.x);
				item->holder->cell->oY = int(-delta.y + mGrabUV.y);
				item->holder->moldCellFromRect(mTexture->width(), mTexture->height(), item->holder->cell->index);
				update();
			} else {
				setCursor(Qt::ArrowCursor);
				aabb rect;
				mCellObj->sync(rect);
				if (rect.contains(mpos)) {
					setCursor(Qt::OpenHandCursor);
				}
			}
		}
	}

	void OriginView::onViewScrollWheel() {
		float val = (5.0f / 6.0f);
		val = pow(val, gMouse.wheel());
		if (val != 0) {
			val = (val > 0) ? val : -1.0f / val; // scroll up: 5/6, down: 6/5

			auto mpos = *mMainCam * gMouse.currentPosition();
			auto cpos = mMainCam->position();
			auto delta = mpos - cpos;
			delta *= (1 - val);

			mMainCam->scale(vec3(val, val, 1));
			mMainCam->move(vec3(delta, 0));
		}
	}

	void OriginView::connectList() {
		connect(gAtlsUtil.cellList, &QListExtension::currentRowChanged, [&]() {
			setFocus();
			update();
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



	void OriginView::moveCellOrigin(int x, int y) {
		if (mTexture && mTexture->atlas && cursor() != Qt::ClosedHandCursor) {
			CellItem* item = reinterpret_cast<CellItem*>(gAtlsUtil.cellList->currentItem());
			if (item && item->holder) {
				item->holder->cell->oX += x;
				item->holder->cell->oY += y;
				item->holder->moldCellFromRect(mTexture->width(), mTexture->height(), item->holder->cell->index);
			} update();
		}
	}

	void OriginView::shiftCell(int dir) {
		if (cursor() != Qt::ClosedHandCursor) {
			auto& list = *gAtlsUtil.cellList;
			int row = list.currentRow() + dir;
			row = clamp(row, 0, list.count() - 1);
			list.setCurrentRow(row);
		} update();
	}

	void OriginView::captureGhost() {
		CellItem* item = reinterpret_cast<CellItem*>(gAtlsUtil.cellList->currentItem());
		if (item) {
			assert(item->holder);
			mGhostData.material = gAtlsUtil.currentMaterial;
			mGhostData.type = ElangAtlasGhostData::eType::CUSTOM;
			mGhostData.order = ElangAtlasGhostData::eOrder::BACK;
			mGhostData.cell = item->holder->cell;
		} update();
	}

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
	//}
}