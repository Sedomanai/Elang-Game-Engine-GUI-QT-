#include "origin_widget.h"

namespace il
{
	OriginView::OriginView(QWidget* parent)
		: QIlangView(parent), mClicked(false), mSelect(0), mGhost(0), mGhostRow(-1)
	{
		//setFocusPolicy(Qt::StrongFocus);
		setMouseTracking(true);
		mGDialog = new GhostDialog(this);

		connect(this, &OriginView::sig_InitializeGL, [&]() {
			makeCurrent();
			mScene.makeCurrent();

			mScene.cameras.clear();
			mFuckingViewport = mScene.cameras.create("Main Cam");
			mFuckingViewport->move(vec3(0, 0, -1000));
			
			auto ghost = mScene.atelier.create("Ghost Painter Back");
			ghost->init("basic_sprite.vert", "raw_pass.frag", 12, mFuckingViewport, 0, 0);
			ghost->color = vec4(0.8, 0.8, 1, 0.5);
			auto textu = mScene.atelier.create("Texture Painter");
			textu->init("basic_sprite.vert", "raw_pass.frag", 12, mFuckingViewport, 0, 0);
			auto ghfr = mScene.atelier.create("Ghost Painter Front");
			ghfr->init("basic_sprite.vert", "raw_pass.frag", 12, mFuckingViewport, 0, 0);
			ghfr->color = vec4(0.8, 0.8, 1, 0.5);
			mScene.debug.init(mFuckingViewport);

			mTexMat = mScene.materials.create("Texture");
			mTexMat->addTexture("");

			mExtMat = mScene.materials.create("External");
			mExtMat->addTexture("");

			mCellBatch = Batch(malloc(sizeof(SpriteVertex) * 4), 4, &gBox2dFillIndices[0], 6, mTexMat, 0, 0);
			mGhostBatch = Batch(malloc(sizeof(SpriteVertex) * 4), 4, &gBox2dFillIndices[0], 6, mExtMat, 0, 0);
			auto tex = gTextures.find(gTexKey);
			if (tex) {
				mTexMat->textures[0] = tex;
			}

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glClearColor(0.2f, 0.3f, 0.2f, 1.0f);
			connectMouseInput();
		});


		connect(this, &OriginView::sig_PaintGL, [&]() {
			makeCurrent();
			mScene.makeCurrent();
			glClearDepth(1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			updateViewport(-width() / 2.0f, width() / 2.0f, -height() / 2.0f, height() / 2.0f);

			// I give up. I don't know why this fucking viewport keeps resetting here but I'm just fixing it this way.
			//mFuckingViewport->setPosX(0);
			//mFuckingViewport->setPosY(0);

			auto tex = gTextures.find(gTexKey);
			if (tex) {
				batch(mSelect, mCellBatch, "Texture Painter", tex);
			}

			asset<Texture> ghosttex;
			switch (mGDialog->type) {
			case eAtlasGhostType::PREVIOUS:
			case eAtlasGhostType::CUSTOM:
				ghosttex = tex;
				break;
			case eAtlasGhostType::EXTERNAL:
				ghosttex = gTextures.find(string(mGDialog->externTexKey()));
				break;
			}

			if (ghosttex) {
				switch (mGDialog->order) {
				case eAtlasGhostOrder::BACK:
					batch(mGhost, mGhostBatch, "Ghost Painter Back", ghosttex);
					break;
				case eAtlasGhostOrder::FRONT:
					batch(mGhost, mGhostBatch, "Ghost Painter Front", ghosttex);
					break;
				}
			}

			mScene.paint();
			drawMidLines();

			mScene.debug.draw();
			});


		connectList();
	}

	OriginView::~OriginView() {
		if (mInitialized) {
			mTexMat->reset();
			mExtMat->reset();
			free(mCellBatch.vertices);
			free(mGhostBatch.vertices);
		}
	}

	void OriginView::updateTexture() {
		auto tex = gTextures.find(gTexKey);
		if (tex && mTexMat) {
			mTexMat->textures[0] = tex;
			update();
		}
	}

	void OriginView::setGhost() {
		mGDialog->open();
		
		if (mGDialog->confirmed()) {
			switch (mGDialog->type) {
			case eAtlasGhostType::NONE:
				mGhost = 0;
				break;
			case eAtlasGhostType::PREVIOUS:
				mGhostBatch.material = mTexMat;
			case eAtlasGhostType::CUSTOM:
				if (mGDialog->cell != -1) {
					mGhost = &(gCells.at(mGDialog->cell));
				}
				break;
			case eAtlasGhostType::EXTERNAL:
				mGhostBatch.material = mExtMat;
				if (mGDialog->cell != -1) {
					auto tex = gTextures.find(string(mGDialog->externTexKey()));
					if (tex) {
						mExtMat->textures[0] = tex;
						mGhost = &(mGDialog->externCells()->at(mGDialog->cell));
					} else cout << "External texture does not exist. Check its URL";
				}
				break;
			}
		}

		// For SOME unknowable reason, touching external ghosts interferes with the current texture and blackens it out
		// This reloads the existing texture should that hapen
		emit sig_SafeTexture();
	}

	void OriginView::showEditor() {
		mScene.makeCurrent();
		gCellList->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
		gCellList->setDragDropMode(QAbstractItemView::DragDropMode::NoDragDrop);
		gCellList->setDefaultDropAction(Qt::DropAction::IgnoreAction);
		gCellList->show();
		show();
		updateTexture();

		if (!mGhost) {
			if (gCellList->count() > 0)
				mGhostRow = clamp(mGhostRow, 0, gCells.count() - 1);
			else mGhostRow = -1;
			if (mGhostRow != -1)
				mGhost = &gCells.at(mGhostRow);
		}

		if (gCellList->count() > 0) {
			gCellRow = clamp(gCellRow, 0, gCellList->count() - 1);
		} else gCellRow = -1;
		gCellList->setCurrentRow(gCellRow);
	}

	void OriginView::hideEditor() {
		gCellRow = gCellList->currentRow();
		switch (mGDialog->type) {
		case eAtlasGhostType::CUSTOM:
		case eAtlasGhostType::PREVIOUS:
			if (mGhost)
				mGhostRow = gCellList->row(mGhost->item);
			mGhost = 0;
		}

		mSelect = 0;
		gCellList->clearSelection();
		gCellList->setCurrentRow(-1);
		gCellList->hide();
		hide();
	}

	void OriginView::moveCellOrigin(int x, int y) {
		if (mSelect) {
			mSelect->ox += x;
			mSelect->oy += y;
		} update();
	}

	void OriginView::captureGhost() {
		mGhostBatch.material = mTexMat;

		int row = gCellList->currentRow();
		if (row != -1) {
			mGDialog->alignCustomByHotkey(row);
			if (mGDialog->cell != -1) {
				mGhost = &(gCells.at(mGDialog->cell));
			}
		}
		update();
	}

	void OriginView::shiftCell(int dir) {
		int row = gCellList->currentRow() + dir;
		row = clamp(row, 0, gCells.count() - 1);
		gCellList->setCurrentRow(row);
		update();
	}

	void OriginView::batch(EditorCell* ecell, const Batch& batch, strview painter, asset<Texture> tex) {
		if (ecell) {
			SpriteVertex* verts = (SpriteVertex*)batch.vertices;
			auto& data = *ecell;
			auto& cell = Cell(data.x, data.y, data.w, data.h, data.ox, data.oy, tex->width, tex->height);

			//safety net
			auto left = round(cell.left);
			auto right = round(cell.right);
			auto up = round(cell.up);
			auto down = round(cell.down);

			verts[0].pos = vec2(left, up);
			verts[0].uv = vec2(cell.uvLeft, cell.uvUp);
			verts[1].pos = vec2(right, up);
			verts[1].uv = vec2(cell.uvRight, cell.uvUp);
			verts[2].pos = vec2(right, down);
			verts[2].uv = vec2(cell.uvRight, cell.uvDown);
			verts[3].pos = vec2(left, down);
			verts[3].uv = vec2(cell.uvLeft, cell.uvDown);

			mScene.atelier.get(string(painter))->batch(SpriteVertexData::sVertexDataIndex, batch);
		}
	}

	void OriginView::connectList() {
		connect(gCellList, &QListExtension::currentRowChanged, [&]() {
			if (isVisible()) {
				auto row = gCellList->currentRow();
				if (row >= 0) {
					mSelect = &gCells.at(row);
					if (mGDialog->type == eAtlasGhostType::PREVIOUS) {
						if (row > 0) {
							mGhost = &gCells.at(row - 1);
						} else mGhost = 0;
					}
				} else {
					mSelect = 0;
					mGhost = 0;
				}
				setFocus();
				update();
			}
		});
	}

	void OriginView::drawMidLines() {
		auto w = 100000.0f;
		auto h = 100000.0f;
		Primitive2DVertex* midlines = (Primitive2DVertex*)malloc(sizeof(Primitive2DVertex) * 4);
		uint* midind = (uint*)malloc(sizeof(uint) * 4);
		midlines[0].pos = vec2(-w, 0);
		midlines[0].col = color(255, 255, 255, 255);
		midlines[1].pos = vec2(w, 0);
		midlines[1].col = color(255, 255, 255, 255);
		midlines[2].pos = vec2(0, -h);
		midlines[2].col = color(255, 255, 255, 255);
		midlines[3].pos = vec2(0, h);
		midlines[3].col = color(255, 255, 255, 255);
		midind[0] = 0;
		midind[1] = 1;
		midind[2] = 2;
		midind[3] = 3;
		mScene.debug.line.batch(Primitive2DVertexData::sVertexDataIndex, Batch(midlines, 4, midind, 4, -1, 0, E_DANGLING_VERTICES));
	}

	void OriginView::connectMouseInput() {
		connect(this, &OriginView::sig_MousePress, [&]() {
			if (gMouse.state(0) == eInput::ONCE && cursor() == Qt::OpenHandCursor) {
				mScene.makeCurrent();
				setCursor(Qt::ClosedHandCursor);
				mGrabPos = gMouse.currentPosition();
				mGrabUV = vec2(mSelect->ox, mSelect->oy);
				update();
			}
		});
		connect(this, &OriginView::sig_MouseRelease, [&]() {
			if (gMouse.state(0) == eInput::LIFT && cursor() == Qt::ClosedHandCursor) {
				setCursor(Qt::OpenHandCursor);
				update();
			}
		});
		connect(this, &OriginView::sig_MouseMove, [&]() {
			mScene.makeCurrent();
			Camera& viewport = *mFuckingViewport;

			vec2 mpos = gMouse.currentPosition();
			if (cursor() == Qt::ClosedHandCursor) {
				auto delta = viewport * (mpos - mGrabPos);
				mSelect->ox = int(delta.x + mGrabUV.x);
				mSelect->oy = int(-delta.y + mGrabUV.y);
				update();
			} else {
				auto tex = gTextures.find(gTexKey);
				aabb rect;
				if (mSelect) {
					auto& data = *mSelect;
					auto& cell = Cell(data.x, data.y, data.w, data.h, data.ox, data.oy, tex->width, tex->height);
					rect = aabb(cell.left, cell.down, cell.right, cell.up);
				}

				setCursor(Qt::ArrowCursor);
				if (rect.contains(viewport * mpos)) {
					setCursor(Qt::OpenHandCursor);
				}
			}
		});
		connect(this, &OriginView::sig_ScrollWheel, [&]() {
			if (cursor() != Qt::ClosedHandCursor) {
				float val = (5.0f / 6.0f);
				val = pow(val, gMouse.wheel());
				if (val != 0) {
					val = (val > 0) ? val : -1.0f / val; // change
					mFuckingViewport->scale(vec3(val, val, 1));
				};
			}
		});
	}
}