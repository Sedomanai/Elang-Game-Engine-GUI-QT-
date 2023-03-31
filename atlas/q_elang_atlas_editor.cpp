#include <elqtpch.h>
#include "q_elang_atlas_editor.h"

#include <atlas/cells_widget.h>
#include <atlas/pivot_widget.h>
#include <atlas/clips_widget.h>
#include <common/string_algorithm.h>

namespace el
{
	QElangAtlasEditor::QElangAtlasEditor(QWidget* parent)
		: AtlasSetup(parent), mSuppressClose(false) {
		cout << "Connecting Atlas Editor..." << endl;

		string in;
		if (el_file::load("../___gui/dat/data.config", in) == 0) {
			el_string::iterate(in, '\n', [&](strview div, sizet index) {
				el_string::tokenize(div, '=', [&](strview head, strview tail) {
					if (head == "Last Search History") {
						gAtlasUtil.lastSearchHistory = tail;
					}
				}); return false;
			});
		}

		if (gAtlasUtil.lastSearchHistory.empty()) {
			gAtlasUtil.lastSearchHistory = "D:/Programming/_Elang/SimpleTest/Assets";
		}

		gAtlasUtil.backupDirectory = "D:/Programming/_Elang/___gui/dat/backup";
		fio::create_directories(gAtlasUtil.backupDirectory);

		gAtlasUtil.makeMaterial();

		if (qApp)
			qApp->installEventFilter(this);

		setFocusPolicy(Qt::FocusPolicy::TabFocus);
		connectActions();
		//debugTexture();
		//debugAtlas();
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
			auto atlas = gAtlasUtil.currentAtlas;
			auto& data = atlas.get<AssetData>();
			fio::path path =
				QFileDialog::getSaveFileName(this, "Save Atlas", gAtlasUtil.lastSearchHistory.generic_string().c_str(), "Atlas (*.atls)").toStdString();

			if (!path.empty()) {
				gAtlasUtil.recordLastDirectoryHistory(path);
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
					backupAtlas();
					endWaitProcess();
				}
			}
		});
	}

	void QElangAtlasEditor::newAtlas() {
		assert(gAtlasUtil.currentMaterial);
		if (gAtlasUtil.currentMaterial->hasTexture()) {

			auto& texdata = gAtlasUtil.currentMaterial->textures[0].get<AssetData>();

			if (texdata.inode != -1) {
				auto atlas = gAtlasUtil.currentMaterial->textures[0]->atlas;
				auto& data = atlas.get<AssetData>();

				fio::path path = QFileDialog::getSaveFileName(this, "New Atlas", gAtlasUtil.lastSearchHistory.generic_string().c_str(), "Atlas (*.atls)").toStdString();
				
				bool newAtlas = false;
				if (!path.empty()) {
					gAtlasUtil.recordLastDirectoryHistory(path);
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
					gProject.get_or_emplace<AssetLoaded>(atlas);
					mCellsWidget->autoNewGenAtlas(atlas, 0, 10);
					atlas->exportFile(path, meta);
					data.inode = el_file::identifier(path);
					data.lastWriteTime = fio::last_write_time(path);

					if (atlas.has<AssetModified>())
						atlas.remove<AssetModified>();
					updateEditorTitle(atlas);
					backupAtlas();
					endWaitProcess();
				}
			}
		} else {
			QMessageBox::warning(this, "No texture to generate atlas.", "Please open a reference background texture first<br>before creating a new atlas.");

		}
	}

	void QElangAtlasEditor::openTexture() {
		assert(gAtlasUtil.currentMaterial && gAtlasUtil.currentMaterial->textures[0]);
		
		fio::path path =
			QFileDialog::getOpenFileName(this, "Open Texture", gAtlasUtil.lastSearchHistory.generic_string().c_str(), "PNG (*.png)").toStdString();

		beginWaitProcess();
		if (gAtlasUtil.openTexture(gAtlasUtil.currentMaterial, path))
			mCellsWidget->updateMaterial(gAtlasUtil.currentMaterial);
		endWaitProcess();
	}

	void QElangAtlasEditor::openAtlas() {
		assert(gAtlasUtil.currentAtlas);

		fio::path path =
			QFileDialog::getOpenFileName(this, "Open Atlas", gAtlasUtil.lastSearchHistory.generic_string().c_str(), "Atlas (*.atls)").toStdString();

		auto atlas = gAtlasUtil.currentAtlas;
		auto& data = atlas.get<AssetData>();
		if (!path.empty() && data.inode != el_file::identifier(path)) {
			if (askSaveMessage() != QMessageBox::Cancel) {
				gAtlasUtil.recordLastDirectoryHistory(path);
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
				if (atlas.has<AssetModified>())
					atlas.remove<AssetModified>();

				data = { el_file::identifier(path), path, fio::last_write_time(path) };

				mCellsWidget->updateAtlas(atlas);
				mClipsWidget->updateOnAtlasLoad();

				updateEditorTitle(atlas);
				endWaitProcess();
				}
		} else {
			cout << "Atlas file is already loaded" << endl;
		}
	}

	void QElangAtlasEditor::saveAtlas() {
		auto atlas = gAtlasUtil.currentAtlas;
		if (atlas.has<AssetModified>()) {
			beginWaitProcess();
			auto& data = atlas.get<AssetData>();
			atlas->exportFile(data.filePath, atlas.get<AtlasMeta>());
			data.lastWriteTime = fio::last_write_time(data.filePath);
			atlas.remove<AssetModified>();
			updateEditorTitle(atlas);
			backupAtlas();
			endWaitProcess();
		}
	}

#define MAX_BACKUP 20

	void QElangAtlasEditor::backupAtlas() {
		string prefix = "backup_";
		auto psize = prefix.size();
		
		sizet count = 0;
		for (auto e : fio::directory_iterator(gAtlasUtil.backupDirectory)) {
			if (e.is_regular_file() && e.path().extension() == ".atls")
				count++;
		}

		for (auto e : fio::directory_iterator(gAtlasUtil.backupDirectory)) {
			if (e.is_regular_file() && e.path().extension() == ".atls") {
				if (count >= MAX_BACKUP) {
					auto file = e.path().filename();

					auto gstr = file.generic_string();
					int a = gstr[psize] - '0';
					int b = gstr[psize + 1] - '0';
					int index = 10 * a + b;

					if (index != 0) {
						index--;
						gstr[psize] = index / 10 + '0';
						gstr[psize + 1] = index % 10 + '0';
						fio::rename(e.path(), e.path().parent_path() / gstr);
					};
				}
			}
		}

		if (count >= MAX_BACKUP)
			count = 19;

		auto atlas = gAtlasUtil.currentAtlas;
		auto name = prefix;

		if (count < 10)
			name += '0';

		name += std::to_string(count) + "_" + atlas.get<AssetData>().filePath.filename().generic_string();
		atlas->exportFile(gAtlasUtil.backupDirectory / name, atlas.get<AtlasMeta>());
	}

	void QElangAtlasEditor::refresh() {
		assert(gAtlasUtil.currentMaterial && gAtlasUtil.currentMaterial->textures[0]);
		auto tex = gAtlasUtil.currentMaterial->textures[0];
		if (tex.has<AssetLoaded>()) {
			auto& data = tex.get<AssetData>();
			if (fio::exists(data.filePath)) {
				data.inode = el_file::identifier(data.filePath);
				auto lwt = fio::last_write_time(data.filePath);
				if (lwt > data.lastWriteTime) {
					auto& meta = tex.get<TextureMeta>();
					tex->unload(meta);
					tex->importFile(data.filePath, meta);
					data.lastWriteTime = lwt;
				}
			} else { // Original texture file has been deleted
				//auto btn = QMessageBox::question(this, "Texture Warning",
				//	tr("The original texture file has been deleted. Keep texture?"),
				//	QMessageBox::Yes | QMessageBox::No,
				//	QMessageBox::Yes);
			}
		}
	}

	void QElangAtlasEditor::debugTexture() {
		fio::path path = "D:/Programming/_Elang/SimpleTest/Assets/link.png";
		auto tex = gAtlasUtil.currentMaterial->textures[0];
		auto& meta = tex.get<TextureMeta>();
		tex->importFile(path, meta);
		tex.get<AssetData>() = { el_file::identifier(path), path, fio::last_write_time(path) };
		tex.add<AssetLoaded>();
		tex.get<GUIAsset>().filePath = path.filename();
		mCellsWidget->updateMaterial(gAtlasUtil.currentMaterial);
	}

	void QElangAtlasEditor::debugAtlas() {
		fio::path path = "D:/Programming/_Elang/SimpleTest/Assets/link.atls";
		auto atlas = gAtlasUtil.currentAtlas;
		auto& meta = atlas.get<AtlasMeta>();
		atlas->importFile(path, meta);
		atlas.get<AssetData>() = { el_file::identifier(path), path, fio::last_write_time(path) };
		atlas.add<AssetLoaded>();
		atlas.get<GUIAsset>().filePath = path.filename();
		mCellsWidget->updateAtlas(gAtlasUtil.currentAtlas);
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
		if (gAtlasUtil.currentAtlas.has<AssetModified>()) {
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
		}; gAtlasUtil.cellList->clear();

		for (auto e : gProject.view<ClipItem*>()) {
			delete gProject.get<ClipItem*>(e);
		}; gAtlasUtil.clipList->clear();
	}

	void QElangAtlasEditor::keyPressEvent(QKeyEvent* e) {
		switch (mViewMode) {
			case AtlasViewMode::Cells:
				mCellsWidget->onKeyPress(e);
				break;
			case AtlasViewMode::Pivot:
				mPivotView->onKeyPress(e);
				break;
			case AtlasViewMode::Clips:
				mClipsWidget->onKeyPress(e);
				break;
		}
	}

	void QElangAtlasEditor::keyReleaseEvent(QKeyEvent* e) {
		switch (mViewMode) {
			case AtlasViewMode::Cells:
				mCellsWidget->onKeyRelease(e);
				break;
			case AtlasViewMode::Pivot:
				mPivotView->onKeyRelease(e);
				break;
			case AtlasViewMode::Clips:
				mClipsWidget->onKeyRelease(e);
				break;
		}
	}

	void QElangAtlasEditor::closeEvent(QCloseEvent* ev) {
		auto atlas = gAtlasUtil.currentAtlas;

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
	
	bool QElangAtlasEditor::eventFilter(QObject* watched, QEvent* event) {
		if (watched == qApp && event->type() == QEvent::ApplicationActivate) {
			refresh();
		}

		return false;
	}
}