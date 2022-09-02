#include <elqtpch.h>
#include "q_elang_atlas_editor.h"

#include <tools/atlas.h>
#include <tools/texture.h>
#include <tools/material.h>
#include <atlas/cells_widget.h>
#include <atlas/clips_widget.h>
#include <apparatus/asset_loader.h>

namespace el
{
	QElangAtlasEditor::QElangAtlasEditor(QWidget* parent)
		: AtlasSetup(parent), mSuppressClose(false) {
		cout << "Connecting Atlas Editor..." << endl;

		auto tex = gProject.make<AssetData>(-1, fio::path(), fio::file_time_type())
			.add<TextureMeta>().add<Texture>().add<EditorAsset>().add<GUIAsset>("");

		AssetLoader loader;
		gAtlsUtil.currentMaterial = loader.createNativeGUIAsset<Material>("__editor_atlas_material_", gMaterials);
		gAtlsUtil.currentMaterial->setTexture(tex, 0);

		auto atlas = gProject.make<AssetData>(-1, fio::path(), fio::file_time_type())
			.add<AtlasMeta>().add<Atlas>().add<EditorAsset>().add<GUIAsset>("");
		tex.get<Texture>().atlas = atlas;
		gAtlsUtil.currentAtlas = atlas;

		connectActions();
		debugTexture();
		debugAtlas();
	}

	void QElangAtlasEditor::showEvent(QShowEvent* e) {
		AtlasSetup::showEvent(e);
		QTimer* timer = new QTimer(this);
		timer->singleShot(500, [&]() { sig_Shown.invoke(); });
	}

	void QElangAtlasEditor::connectActions() {
		connect(ui.actionNewAtlas, &QAction::triggered, this, &QElangAtlasEditor::newAtlas);
		connect(ui.actionOpenTexture, &QAction::triggered, this, &QElangAtlasEditor::openTexture);
		connect(ui.actionOpenAtlas, &QAction::triggered, this, &QElangAtlasEditor::openAtlas);
		connect(ui.actionSaveAtlas, &QAction::triggered, this, &QElangAtlasEditor::saveAtlas);

		connect(ui.actionSaveAtlasAs, &QAction::triggered, [&]() {
			auto atlas = gAtlsUtil.currentAtlas;
			auto& data = atlas.get<AssetData>();
			fio::path path =
				QFileDialog::getSaveFileName(this, "Save Atlas", "D:/Programming/_Elang/SimpleTest/Assets", "Atlas (*.atls)").toStdString();

			if (!path.empty()) {
				if (path == data.filePath) {
					saveAtlas();
				} else {
					beginWaitProcess();
					data.filePath = path;
					atlas->exportFile(data.filePath, atlas.get<AtlasMeta>());
					data.lastWriteTime = fio::last_write_time(data.filePath);
					data.inode = el_file::identifier(path);
					atlas.get<GUIAsset>().filePath = path.filename();
					if (atlas.has<AssetModified>())
						atlas.remove<AssetModified>();
					updateEditorTitle(atlas);
					endWaitProcess();
				}
			}
		});
	}

	void QElangAtlasEditor::newAtlas() {
		assert(gAtlsUtil.currentMaterial && gAtlsUtil.currentMaterial->hasTexture());
		auto& texdata = gAtlsUtil.currentMaterial->textures[0].get<AssetData>();

		if (texdata.inode != -1) {
			auto atlas = gAtlsUtil.currentMaterial->textures[0]->atlas;
			auto& data = atlas.get<AssetData>();

			fio::path path = QFileDialog::getSaveFileName(this, "New Atlas", "D:/Programming/_Elang/SimpleTest/Assets", "Atlas (*.atls)").toStdString();

			bool newAtlas = false;
			if (!path.empty()) {
				if (data.inode != el_file::identifier(path)) {
					newAtlas = (askSaveMessage() != QMessageBox::Cancel);
				} else {
					newAtlas = QMessageBox::question(this, "Confirm New Atlas Overwrite",
						tr("This will completely rewrite and reload the current atlas file.<br>Are you sure you want to do this?"),
						QMessageBox::Yes | QMessageBox::No,
						QMessageBox::Yes) == QMessageBox::Yes;
				}
			}

			if (newAtlas) {
				beginWaitProcess();
				data.filePath = path;
				atlas.get<GUIAsset>().filePath = path.filename();
				auto& meta = atlas.get<AtlasMeta>();
				atlas->unload(meta);
				clearLists();
				mCellsWidget->autoNewGenAtlas(atlas, 0, 10);
				atlas->exportFile(path, meta);
				gProject.get_or_emplace<AssetLoaded>(atlas);
				data.inode = el_file::identifier(path);
				data.lastWriteTime = fio::last_write_time(path);

				if (atlas.has<AssetModified>())
					atlas.remove<AssetModified>();
				updateEditorTitle(atlas);
				endWaitProcess();
			}
		}
	}

	void QElangAtlasEditor::openTexture() {
		assert(gAtlsUtil.currentMaterial && gAtlsUtil.currentMaterial->hasTexture());
		auto tex = gAtlsUtil.currentMaterial->textures[0];
		auto& data = tex.get<AssetData>();

		fio::path path =
			QFileDialog::getOpenFileName(this, "Open Texture", "D:/Programming/_Elang/SimpleTest/Assets", "PNG (*.png)").toStdString();

		beginWaitProcess();
		if (!path.empty() && data.inode != el_file::identifier(path)) {
			auto& meta = tex.get<TextureMeta>();
			if (tex.has<AssetLoaded>())
				tex->unload(meta);
			else {
				tex.add<AssetLoaded>();
			}

			tex->importFile(path, meta);
			tex.get<GUIAsset>().filePath = path.filename();
			data = { el_file::identifier(path), path, fio::last_write_time(path) };

			mCellsWidget->updateMaterial(gAtlsUtil.currentMaterial);
		} else {
			cout << "Texture file is already loaded" << endl;
		}
		endWaitProcess();
	}

	void QElangAtlasEditor::openAtlas() {
		assert(gAtlsUtil.currentMaterial && gAtlsUtil.currentMaterial->hasTexture());
		auto atlas = gAtlsUtil.currentMaterial->textures[0]->atlas;
		auto& data = atlas.get<AssetData>();

		fio::path path =
			QFileDialog::getOpenFileName(this, "Open Atlas", "D:/Programming/_Elang/SimpleTest/Assets", "Atlas (*.atls)").toStdString();

		if (!path.empty()) {
			if (data.inode != el_file::identifier(path)) {
				if (askSaveMessage() != QMessageBox::Cancel) {
					beginWaitProcess();
					auto& meta = atlas.get<AtlasMeta>();
					if (atlas.has<AssetLoaded>()) {
						clearLists();
						atlas->unload(meta);
					} else {
						atlas.add<AssetLoaded>();
					}

					atlas->importFile(path, meta);
					atlas.get<GUIAsset>().filePath = path.filename();
					data = { el_file::identifier(path), path, fio::last_write_time(path) };

					mCellsWidget->updateAtlas(atlas);
					mClipsWidget->updateOnAtlasLoad();

					if (atlas.has<AssetModified>())
						atlas.remove<AssetModified>();
					updateEditorTitle(atlas);
					endWaitProcess();
				}
			} else {
				cout << "Atlas file is already loaded" << endl;
			}
		}
	}

	void QElangAtlasEditor::saveAtlas() {
		auto atlas = gAtlsUtil.currentAtlas;
		if (atlas.has<AssetModified>()) {
			beginWaitProcess();
			auto& data = atlas.get<AssetData>();
			atlas->exportFile(data.filePath, atlas.get<AtlasMeta>());
			data.lastWriteTime = fio::last_write_time(data.filePath);
			atlas.remove<AssetModified>();
			updateEditorTitle(atlas);
			endWaitProcess();
		}
	}

	void QElangAtlasEditor::debugTexture() {
		fio::path path = "D:/Programming/_Elang/SimpleTest/Assets/link.png";
		auto tex = gAtlsUtil.currentMaterial->textures[0];
		auto& meta = tex.get<TextureMeta>();
		tex->importFile(path, meta);
		tex.get<AssetData>() = { el_file::identifier(path), path, fio::last_write_time(path) };
		tex.add<AssetLoaded>();
		tex.get<GUIAsset>().filePath = path.filename();
		mCellsWidget->updateMaterial(gAtlsUtil.currentMaterial);
	}

	void QElangAtlasEditor::debugAtlas() {
		fio::path path = "D:/Programming/_Elang/SimpleTest/Assets/link.atls";
		auto atlas = gAtlsUtil.currentAtlas;
		auto& meta = atlas.get<AtlasMeta>();
		atlas->importFile(path, meta);
		atlas.get<AssetData>() = { el_file::identifier(path), path, fio::last_write_time(path) };
		atlas.add<AssetLoaded>();
		atlas.get<GUIAsset>().filePath = path.filename();
		mCellsWidget->updateAtlas(gAtlsUtil.currentAtlas);
	}

	void QElangAtlasEditor::debugEditorView(bool anim) {
		if (anim) {
			ui.actionClipsView->trigger();
		} else
			ui.actionPivotView->trigger();
	}

	void QElangAtlasEditor::beginWaitProcess() {
		QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
		QApplication::processEvents();
		mClipsTimer->stop();
		setEnabled(false);
		mSuppressClose = true;
	}

	void QElangAtlasEditor::endWaitProcess() {
		setEnabled(true);
		QApplication::restoreOverrideCursor();
		QApplication::processEvents();
		mClipsTimer->start();
		mSuppressClose = false;
	}

	QMessageBox::StandardButton QElangAtlasEditor::askSaveMessage() {
		if (gAtlsUtil.currentAtlas.has<AssetModified>()) {
			auto btn = QMessageBox::question(this, "Save Message",
				tr("You're closing your modified atlas without saving.\t<br>Save before closing?"),
				QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
				QMessageBox::Yes);

			if (btn == QMessageBox::Yes) {
				saveAtlas();
			} return btn;
		} else return QMessageBox::No;
	}

	void QElangAtlasEditor::clearLists() {
		for (auto e : gProject.view<CellItem*>()) {
			delete gProject.get<CellItem*>(e);
		}; gAtlsUtil.cellList->clear();

		for (auto e : gProject.view<ClipItem*>()) {
			delete gProject.get<ClipItem*>(e);
		}; gAtlsUtil.clipList->clear();
	}

	void QElangAtlasEditor::keyPressEvent(QKeyEvent* e) {
		switch (mViewMode) {
		case AtlasViewMode::Cells:
		mCellsWidget->onKeyPress(e);
		break;
		case AtlasViewMode::Pivot:
		//mOriginView->onKeyPress(e);
		break;
		case AtlasViewMode::Clips:
		//mClipsWidget->onKeyPress(e);
		break;
		}
	}

	void QElangAtlasEditor::keyReleaseEvent(QKeyEvent* e) {
		switch (mViewMode) {
		case AtlasViewMode::Cells:
		mCellsWidget->onKeyRelease(e);
		break;
		case AtlasViewMode::Pivot:
		//mOriginView->onKeyRelease(e);
		break;
		case AtlasViewMode::Clips:
		//ClipsWidget->onKeyRelease(e);
		break;
		}
	}

	void QElangAtlasEditor::closeEvent(QCloseEvent* ev) {
		auto atlas = gAtlsUtil.currentAtlas;

		if (mSuppressClose) {
			ev->ignore();
		}

		auto btn = askSaveMessage();
		if (btn == QMessageBox::Cancel) {
			ev->ignore();
		} else {
			ev->accept();
		}
	}
}