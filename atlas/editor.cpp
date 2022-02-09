#include "editor.h"
#include <qopenglcontext>

namespace el
{
	AtlasEditor::AtlasEditor(QWidget* parent)
		: QMainWindow(parent), mSuppressSignal(false)
	{
		ui.setupUi(this);

		if (gGUI.rc.painters.count() == 0) {
			QElangView::sSig_GlobalGL.connect([&]() {
				bind(gGUI.rc);
				loadElangProject((gGUI.enginePath() + "src/gui.elang").c_str(), true);
				bind(gGUI.project);
			});
		}
		
		if (gGUI.open()) {
			ui.actionNewProject->setVisible(false);
			ui.actionLoadProject->setVisible(false);
			ui.actionSaveProject->setVisible(false);
			ui.actionSaveProjectAs->setVisible(false);
			// change mode
		}

		// DO NOT CHANGE THE ORDER
		setupLayout(); // do not touch
		setupList(); // do not touch
		setupCellMode();
		setupOriginMode();
		setupClipMode();
		setupActions();

		mAutoGen = new AtlasAutoGenerator(this);
		setupInitView();

		gAtlsUtil.cellList->setEnabled(false);
		gAtlsUtil.clipList->setEnabled(false);
	}

	void AtlasEditor::setupLayout() {
		QHBoxLayout* layout = new QHBoxLayout();
		layout->setContentsMargins(QMargins(0, 0, 0, 0));
		mListLayout = new QVBoxLayout();
		mListLayout->setSizeConstraint(QLayout::SizeConstraint::SetFixedSize);
		layout->addLayout(mListLayout);
		mViewLayout = new QVBoxLayout();
		layout->addLayout(mViewLayout);
		ui.frame->setLayout(layout);
	}

	void AtlasEditor::setupList() {
		gAtlsUtil.cellList = new QListExtension(this);
		auto& cells = *gAtlsUtil.cellList;
		cells.setSelectionMode(QAbstractItemView::SelectionMode::ExtendedSelection);
		cells.setDragDropMode(QAbstractItemView::DragDropMode::DragDrop);
		cells.setDefaultDropAction(Qt::DropAction::MoveAction);
		cells.setEditTriggers(QAbstractItemView::EditTrigger::EditKeyPressed);
		mListLayout->addWidget(&cells);

		gAtlsUtil.clipList = new QListExtension(this);
		auto& clips = *gAtlsUtil.clipList;
		clips.setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
		clips.setDragDropMode(QAbstractItemView::DragDropMode::DragDrop);
		clips.setDefaultDropAction(Qt::DropAction::MoveAction);
		clips.setEditTriggers(QAbstractItemView::EditTrigger::EditKeyPressed);
		mListLayout->addWidget(&clips);
	}

	void AtlasEditor::setupInitView() {
		mViewMode = ViewMode::CELL;
		//mCellsWidget->showEditor();
		gAtlsUtil.clipList->hide();
		//mOriginView->hide();
		mOriginToolbar->hide();
		//mClipsView->hide();
		mClipsToolbar->hide();
	}

	void AtlasEditor::setupCellMode() {
		mCellToolbar = new QToolBar(this);
		QLabel* label = new QLabel("  Set Texture  ", mCellToolbar);
		mTextureBox = new QComboBox(mCellToolbar);
		mTextureBox->setMinimumWidth(100);
		connect(mTextureBox, &QComboBox::currentTextChanged, [&](const QString& text) {
			if (gProject->textures.contains(text.toStdString())) {
				gAtlsUtil.currentMaterial->setTexture(gProject->textures[text.toStdString()]);
				mCellsWidget->updateMaterial(gAtlsUtil.currentMaterial);
			}
		});

		mCellToolbar->addWidget(label);
		mCellToolbar->addWidget(mTextureBox);
		mCellToolbar->addSeparator();

		auto autogen = mCellToolbar->addAction("Auto Atlas", [&]() {
			mAutoGen->exec();
			if (mAutoGen->gen) {
				mCellsWidget->autoGenCells(mAutoGen->sortorder, mAutoGen->margin);
				mAutoGen->gen = false;
			}
		}); autogen->setShortcut(QKeySequence(Qt::Key_F));

		//auto sort = mCellToolbar->addAction("Sort List", [&]() {
		//	mAutoGen->exec();
		//	if (mAutoGen->gen) {
		//		mCellsWidget->autoGenCells(mAutoGen->sortorder, mAutoGen->margin);
		//		mAutoGen->gen = false;
		//	}
		//}); autogen->setShortcut(QKeySequence(Qt::Key_R));

		auto autocell = mCellToolbar->addAction("Auto Cell", [&]() {
			mCellsWidget->autoCreateCell();
		}); autocell->setShortcut(QKeySequence(Qt::Key_R));


		mCellToolbar->addSeparator();


		auto combine = mCellToolbar->addAction("Combine Cells", [&]() {
			mCellsWidget->combineCells();
			});
		combine->setShortcut(QKeySequence(Qt::Key_C));


		auto remove = mCellToolbar->addAction("Remove Cells", [&]() {
			mCellsWidget->deleteSelected();
			});
		remove->setShortcut(QKeySequence(Qt::Key_Delete));

		mCellToolbar->addSeparator();
		
		mCellsWidget = new CellsWidget(this);
		mCellsWidget->setMinimumWidth(750);
		mViewLayout->addWidget(mCellsWidget);

		addToolBarBreak();
		addToolBar(Qt::ToolBarArea::TopToolBarArea, mCellToolbar);
	}

	void AtlasEditor::setupOriginMode()
	{
		mOriginToolbar = new QToolBar(this);
		/*auto act = mOriginToolbar->addAction("Set Ghost", [&]() {
			mOriginView->setGhost();
			});

		mOriginToolbar->addSeparator();
		auto left = mOriginToolbar->addAction("Cell Left", [&]() {
			mOriginView->moveCellOrigin(-1, 0);
			});
		left->setShortcut(QKeySequence(Qt::Key_A));
		auto right = mOriginToolbar->addAction("Cell Right", [&]() {
			mOriginView->moveCellOrigin(1, 0);
			});
		right->setShortcut(QKeySequence(Qt::Key_D));
		auto up = mOriginToolbar->addAction("Cell Up", [&]() {
			mOriginView->moveCellOrigin(0, -1);
			});
		up->setShortcut(QKeySequence(Qt::Key_W));
		auto down = mOriginToolbar->addAction("Cell Down", [&]() {
			mOriginView->moveCellOrigin(0, 1);
			});
		down->setShortcut(QKeySequence(Qt::Key_S));

		mOriginToolbar->addSeparator();

		auto prevCell = mOriginToolbar->addAction("Prev Cell", [&]() {
			mOriginView->shiftCell(-1);
			});
		prevCell->setShortcut(QKeySequence(Qt::Key_Q));
		auto nextCell = mOriginToolbar->addAction("Next Cell", [&]() {
			mOriginView->shiftCell(1);
			});
		nextCell->setShortcut(QKeySequence(Qt::Key_E));

		mOriginToolbar->addSeparator();
		auto ghost = mOriginToolbar->addAction("Capture Ghost", [&]() {
			mOriginView->captureGhost();
			});
		ghost->setShortcut(QKeySequence(Qt::Key_C));
		
		mOriginView = new OriginView(this);
		mOriginView->setMinimumWidth(750);
		mViewLayout->addWidget(mOriginView);

		connect(mOriginView, &OriginView::sig_SafeTexture, [&]() {
			gTextures.load(gProjectDir.path().toStdString() + "/" + gTexKey, gTexKey);
			mCellsWidget->updateTexture(gTexKey);
			mOriginView->updateTexture();
			mClipsView->updateTexture();
			});
			*/

		addToolBarBreak();
		addToolBar(Qt::ToolBarArea::TopToolBarArea, mOriginToolbar);

	}

	void AtlasEditor::setupClipMode()
	{
		mClipsToolbar = new QToolBar(this);
		//mClipsToolbar->addAction("Add Frame", [&]() {
		//	//mClipsView->addFrame();
		//	});

		//mClipsView = new ClipsView(this);
		//mClipsView->setMinimumWidth(750);
		////mClipsView->showEditor();
		//mViewLayout->addWidget(mClipsView);

		addToolBar(Qt::ToolBarArea::TopToolBarArea, mClipsToolbar);
	}


	void AtlasEditor::refresh() {
		gAtlsUtil.cellList->setEnabled(true);
		gAtlsUtil.clipList->setEnabled(true);
		gAtlsUtil.currentMaterial = gProject->make<Material>(gProject->materials, "_EL_AtlasTextureCurrentMaterial");
		gAtlsUtil.currentMaterial->textures.clear();
		mTextureBox->clear();
		for (auto it : gGUI.project.textures) {
			mTextureBox->addItem(QString::fromStdString(it.first));
		} 
	}

	void AtlasEditor::keyPressEvent(QKeyEvent* e) {
		if (gGUI.open()) {
			switch (mViewMode) {
			case ViewMode::CELL:
				mCellsWidget->onKeyPress(e);
				break;
			}
		}
	}

	void AtlasEditor::keyReleaseEvent(QKeyEvent* e) {
		if (gGUI.open()) {
			switch (mViewMode) {
			case ViewMode::CELL:
				mCellsWidget->onKeyRelease(e);
				break;
			}
		}
	}

	void AtlasEditor::setupActions() {
		mViewActions = new QActionGroup(this);
		mViewActions->addAction(ui.actionCellMode);
		mViewActions->addAction(ui.actionOriginMode);
		mViewActions->addAction(ui.actionClipMode);
		mViewActions->setExclusionPolicy(QActionGroup::ExclusionPolicy::Exclusive);

		connect(ui.actionNewProject, &QAction::triggered, [&]() {
			gGUI.makeNewProject(this);
			if (gGUI.open()) {
				refresh();
			}
		});
		connect(ui.actionSaveProject, &QAction::triggered, [&]() {
			gGUI.saveCurrentProject();
		});
		connect(ui.actionSaveProjectAs, &QAction::triggered, [&]() {
			gGUI.saveCurrentProjectAs(this);
		});
		connect(ui.actionLoadProject, &QAction::triggered, [&]() {
			mCellsWidget->view()->makeCurrent();
			gGUI.loadCurrentProject(this);
			if (gGUI.open()) {
				refresh();
			}
		});

		mDebugLoad = new QAction(this);
		addAction(mDebugLoad);
		mDebugLoad->setShortcut(QKeySequence(Qt::Key::Key_P));

		auto alt = new QAction(this);
		addAction(alt);
		alt->setShortcut(QKeySequence(Qt::Key::Key_Alt));

		connect(mDebugLoad, &QAction::triggered, [&]() {
			if (gGUI.rc.painters.count() > 0) {
				mCellsWidget->view()->makeCurrent();
				gGUI.loadDebugProject();
				if (gGUI.open()) {
					refresh();
				}
			}
		});

		connect(mViewActions, &QActionGroup::triggered, [&](QAction* action) {
			switch (mViewMode) {
			case ViewMode::CELL:
				mCellToolbar->hide();
				//mCellsWidget->hideEditor();
				gMouse.reset();
				break;
			case ViewMode::ORIGIN:
				mOriginToolbar->hide();
				//mOriginView->hideEditor();
				gMouse.reset();
				break;
			case ViewMode::CLIP:
				mClipsToolbar->hide();
				//mClipsView->hideEditor();
				gMouse.reset();
				break;
			}

			if (action == ui.actionCellMode) {
				mViewMode = ViewMode::CELL;
				//mCellsWidget->showEditor();
				mCellToolbar->show();
			}
			else if (action == ui.actionOriginMode) {
				mViewMode = ViewMode::ORIGIN;
				//mOriginView->showEditor();
				mOriginToolbar->show();
			}
			else if (action == ui.actionClipMode) {
				mViewMode = ViewMode::CLIP;
				//mClipsView->showEditor();
				mClipsToolbar->show();
			}
		});
	}

}
