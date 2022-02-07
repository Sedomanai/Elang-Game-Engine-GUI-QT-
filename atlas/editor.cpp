#include "editor.h"

namespace el
{
	AtlasEditor::AtlasEditor(QWidget *parent, Project* gui)
		: QMainWindow(parent)
	{
		ui.setupUi(this);

		if (!gui) {
			gui = new Project();
			QElangView::sSig_GlobalGL.connect([&]() {
				bind(*gui);
				loadElangProject("gui.elang");
			});
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
		gAtlasEditorData.cellList = new QListExtension(this);
		auto& cells = *gAtlasEditorData.cellList;
		cells.setSelectionMode(QAbstractItemView::SelectionMode::ExtendedSelection);
		cells.setDragDropMode(QAbstractItemView::DragDropMode::DragDrop);
		cells.setDefaultDropAction(Qt::DropAction::MoveAction);
		cells.setEditTriggers(QAbstractItemView::EditTrigger::EditKeyPressed);
		mListLayout->addWidget(&cells);

		gAtlasEditorData.clipList = new QListExtension(this);
		auto& clips = *gAtlasEditorData.clipList;
		clips.setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
		clips.setDragDropMode(QAbstractItemView::DragDropMode::DragDrop);
		clips.setDefaultDropAction(Qt::DropAction::MoveAction);
		clips.setEditTriggers(QAbstractItemView::EditTrigger::EditKeyPressed);
		mListLayout->addWidget(&clips);

		//test
		cells.addItem("testCell");
		cells.addItem("testCell2");
		clips.addItem("testClip");
		clips.addItem("testClip2");
		clips.addItem("testClip3");
	}

	void AtlasEditor::setupInitView() {
		mViewMode = ViewMode::CELL;
		//mCellsWidget->showEditor();
		gAtlasEditorData.clipList->hide();
		//mOriginView->hide();
		mOriginToolbar->hide();
		//mClipsView->hide();
		mClipsToolbar->hide();
	}

	void AtlasEditor::setupCellMode() {
		mCellToolbar = new QToolBar(this);

		QLabel* label = new QLabel("  Set Texture  ", mCellToolbar);
		QComboBox* box = new QComboBox(mCellToolbar);
		box->setMinimumWidth(100);
		mCellToolbar->addWidget(label);
		mCellToolbar->addWidget(box);


		//for (auto it : gProject->textures) {
		//	box->addItem(QString::fromStdString(it.first));
		//}

		mCellToolbar->addSeparator();

		auto autogen = mCellToolbar->addAction("Auto Generate", [&]() {
			mAutoGen->exec();
			if (mAutoGen->gen) {
				//mCellsWidget->autoGenCells(mAutoGen->sortorder, mAutoGen->margin);
				mAutoGen->gen = false;
			}
		});


		/*auto setTexture = mCellToolbar->addAction("Set Texture", [&]() {
			auto fileName = QFileDialog::getOpenFileName(this, tr("Background Texture"), gProjectDir.path(), tr("PNG (*.png)"));

			if (!fileName.isEmpty()) {
				gTexKey = gProjectDir.relativeFilePath(fileName).toStdString();
				gTextures.load(fileName.toStdString(), gTexKey);

				mCellsWidget->updateTexture(gTexKey);
			}
		}); setTexture->setShortcut(QKeySequence(Qt::Key_Space));

		autogen->setShortcut(QKeySequence(Qt::Key_F));
		mCellToolbar->addSeparator();


		auto combine = mCellToolbar->addAction("Combine Cells", [&]() {
			mCellsWidget->combineCells();
			});
		combine->setShortcut(QKeySequence(Qt::Key_C));


		auto remove = mCellToolbar->addAction("Remove Cells", [&]() {
			mCellsWidget->deleteSelected();
			});
		remove->setShortcut(QKeySequence(Qt::Key_Delete));

		auto autoc = mCellToolbar->addAction("Auto Cell", [&]() {
			mCellsWidget->autoCreateCell();
			});
		autoc->setShortcut(QKeySequence(Qt::Key_R));
		mCellToolbar->addSeparator();

		//mCellsWidget = new CellsWidget(this);
		//mCellsWidget->setMinimumWidth(750);
		//mViewLayout->addWidget(mCellsWidget);
		*/

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


	void AtlasEditor::setupActions() {
		mViewActions = new QActionGroup(this);
		mViewActions->addAction(ui.actionCellMode);
		mViewActions->addAction(ui.actionOriginMode);
		mViewActions->addAction(ui.actionClipMode);
		mViewActions->setExclusionPolicy(QActionGroup::ExclusionPolicy::Exclusive);

		//connect(ui.actionSet_Texture_As_Link, &QAction::triggered, [&]() {
		//	cout << "TODO: set debug texture (link or alex)";
			/*string dir = gAssetDirectory + "spelunky";
			string key = "spelunky.png";
			gTexKey = key;
			gTextures.load(dir + ".png", gTexKey);
			mCellsWidget->updateTexture(gTexKey);
			mOriginView->updateTexture();
			mClipsView->updateTexture();

			string out;
			loadFile(dir + ".atls", out);
			mLastSavedPath.fromStdString(dir + ".atls");
			importAtlas(out);*/
		//});


		//connect(ui.actionSave_As, &QAction::triggered, [&]() {
		//	stringstream s;
		//	mAcceptClose = false;
		//	exportAtlas(s);
		//	saveBody(s);
		//	mAcceptClose = true;
		//	});

		//connect(ui.actionSave, &QAction::triggered, [&]() {
		//	stringstream s;
		//	mAcceptClose = false;
		//	exportAtlas(s);
		//	saveBody(s, mLastSavedPath.isEmpty());
		//	mAcceptClose = true;
		//	});

		//connect(ui.actionLoad, &QAction::triggered, [&]() {
		//	auto filePath = QFileDialog::getOpenFileName(this, tr("Import Atlas"), gProjectDir.path(), tr("Sprite Atlas (*.atls)"));
		//	mLastSavedPath = filePath;
		//	if (!filePath.isEmpty()) {
		//		string out;
		//		loadFile(filePath.toStdString(), out);
		//		importAtlas(out);
		//	}
		//	});


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
