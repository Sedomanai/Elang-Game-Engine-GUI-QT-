#include <elqtpch.h>
#include "atlas_setup.h"

#include <tools/controls.h>
#include <tools/atlas.h>
#include <atlas/cells_widget.h>
#include <atlas/pivot_widget.h>
#include <atlas/clips_widget.h>
#include <apparatus/asset_loader.h>

namespace el
{
	AtlasSetup::AtlasSetup(QWidget* parent) : QMainWindow(parent) {
		cout << "Setting up Atlas Editor..." << endl;
		ui.setupUi(this);
		setupActions();
		setupLayout();
		setupList();
		setupCellMode();
		setupPivotMode();
		setupClipMode();
		setupInitView();

		QTimer* timer = new QTimer(this);
		connect(timer, &QTimer::timeout, this, &AtlasSetup::loop);
		timer->start(1000.0f / 60.0f);
	}

	void AtlasSetup::setupActions() {
		mViewActions = new QActionGroup(this);
		mViewActions->addAction(ui.actionCellsView);
		mViewActions->addAction(ui.actionPivotView);
		mViewActions->addAction(ui.actionClipsView);
		mViewActions->setExclusionPolicy(QActionGroup::ExclusionPolicy::Exclusive);

		connect(mViewActions, &QActionGroup::triggered, [&](QAction* action) {
			switch (mViewMode) {
				case AtlasViewMode::Cells:
					mCellToolbar->hide();
					mCellsWidget->hideEditor();
					gMouse.reset();
					break;
				case AtlasViewMode::Pivot:
					mPivotToolbar1->hide();
					mPivotToolbar2->hide();
					mPivotView->hideEditor();
					gMouse.reset();
					break;
				case AtlasViewMode::Clips:
					mClipsToolbar->hide();
					mClipsWidget->hideEditor();
					gMouse.reset();
					break;
			}

			if (action == ui.actionCellsView) {
				mViewMode = AtlasViewMode::Cells;
				mCellToolbar->show();
				mCellsWidget->showEditor();
			} else if (action == ui.actionPivotView) {
				mViewMode = AtlasViewMode::Pivot;
				mPivotToolbar1->show();
				mPivotToolbar2->show();
				mPivotView->showEditor();
			} else if (action == ui.actionClipsView) {
				mViewMode = AtlasViewMode::Clips;
				mClipsToolbar->show();
				mClipsWidget->showEditor();
			}
			});

		mViewActions->setEnabled(true);
	}

	void AtlasSetup::loop() {
		switch (mViewMode) {
			case AtlasViewMode::Cells: mCellsWidget->loop(); break;
			case AtlasViewMode::Pivot: mPivotView->loop(); break;
			case AtlasViewMode::Clips: mClipsWidget->loop(); break;
		}
	}

	void AtlasSetup::setupLayout() {
		QHBoxLayout* layout = new QHBoxLayout();
		layout->setContentsMargins(QMargins(0, 0, 0, 0));
		mListLayout = new QVBoxLayout();
		mListLayout->setSizeConstraint(QLayout::SizeConstraint::SetFixedSize);
		layout->addLayout(mListLayout);
		mViewLayout = new QVBoxLayout();
		layout->addLayout(mViewLayout);
		ui.frame->setLayout(layout);
	}

	void AtlasSetup::setupList() {
		gAtlsUtil.cellList = new QListExtension(this);
		auto& cells = *gAtlsUtil.cellList;
		cells.setSelectionMode(QAbstractItemView::SelectionMode::ExtendedSelection);
		cells.setDragDropMode(QAbstractItemView::DragDropMode::DragDrop);
		cells.setDefaultDropAction(Qt::DropAction::MoveAction);
		cells.setEditTriggers(QAbstractItemView::EditTrigger::EditKeyPressed);
		cells.setMinimumWidth(150);
		cells.setMaximumWidth(220);
		mListLayout->addWidget(&cells);

		gAtlsUtil.clipList = new QListExtension(this);
		auto& clips = *gAtlsUtil.clipList;
		clips.setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
		clips.setDragDropMode(QAbstractItemView::DragDropMode::DragDrop);
		clips.setDefaultDropAction(Qt::DropAction::MoveAction);
		clips.setEditTriggers(QAbstractItemView::EditTrigger::EditKeyPressed);
		clips.setMinimumWidth(150);
		clips.setMaximumWidth(220);
		mListLayout->addWidget(&clips);
	}

	void AtlasSetup::setupInitView() {
		//mAutoGen = new AtlasAutoGenerator(this);
		mViewMode = AtlasViewMode::Cells;
		mCellsWidget->showEditor();

		gAtlsUtil.clipList->hide();
		mPivotView->hide();
		mPivotToolbar1->hide();
		mPivotToolbar2->hide();
		mClipsWidget->hide();
		mClipsToolbar->hide();
	}

	void AtlasSetup::setupCellMode() {
		mCellToolbar = new QToolBar(this);

		//auto autogen = mCellToolbar->addAction("Auto Atlas", [&]() {
		//	//mAutoGen->exec();
		//	//if (mAutoGen->gen) {
		//	//	mCellsWidget->autoGenCells(mAutoGen->sortorder, mAutoGen->margin);
		//	//	//TODO: add warning, will remove all clips and cells
		//	//	mClipsWidget->clearAllViews();

		//	//	mAutoGen->gen = false;
		//	//}
		//}); 
		//autogen->setShortcut(QKeySequence(Qt::Key_F));

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
			//mCellsWidget->view()->setStage(&mStage);
			mCellsWidget->sig_Modified.connect([&]() { setModified(); });
			mViewLayout->addWidget(mCellsWidget);


			addToolBarBreak();
			addToolBar(Qt::ToolBarArea::TopToolBarArea, mCellToolbar);
	}

	void AtlasSetup::setupPivotMode() {
		mPivotToolbar1 = new QToolBar(this);
		mPivotToolbar2 = new QToolBar(this);

		auto left = mPivotToolbar1->addAction("Cell Left", [&]() {
			mPivotView->incrementPivot(-1, 0);
			});
		auto right = mPivotToolbar1->addAction("Cell Right", [&]() {
			mPivotView->incrementPivot(1, 0);
			});
		auto down = mPivotToolbar1->addAction("Cell Down", [&]() {
			mPivotView->incrementPivot(0, -1);
			});
		auto up = mPivotToolbar1->addAction("Cell Up", [&]() {
			mPivotView->incrementPivot(0, 1);
			});
		left->setShortcut(QKeySequence(Qt::Key_A));
		right->setShortcut(QKeySequence(Qt::Key_D));
		up->setShortcut(QKeySequence(Qt::Key_W));
		down->setShortcut(QKeySequence(Qt::Key_S));

		mPivotToolbar1->addSeparator();

		auto prevCell = mPivotToolbar1->addAction("Prev Cell", [&]() {
			mPivotView->shiftCell(-1);
			});
		prevCell->setShortcut(QKeySequence(Qt::Key_Q));
		auto nextCell = mPivotToolbar1->addAction("Next Cell", [&]() {
			mPivotView->shiftCell(1);
			});
		nextCell->setShortcut(QKeySequence(Qt::Key_E));

		mPivotToolbar1->addSeparator();

		auto recenterCam = mPivotToolbar1->addAction("Recenter Camera", [&]() {
			mPivotView->recenterCamera();
			});
		recenterCam->setShortcut(QKeySequence(Qt::Key_G));

		mPivotToolbar1->addSeparator();

		auto act = mPivotToolbar2->addAction("Set Ghost", [&]() {
			mPivotView->execGhostDialog();
			});

		mPivotToolbar2->addSeparator();

		auto prevGhost = mPivotToolbar2->addAction("Previous Ghost", [&]() {
			mPivotView->setPreviousGhost();
			});
		prevGhost->setShortcut(QKeySequence(Qt::Key_V));
		auto captureGhost = mPivotToolbar2->addAction("Capture Ghost", [&]() {
			mPivotView->captureGhost();
			});
		captureGhost->setShortcut(QKeySequence(Qt::Key_C));
		auto paletteGhost = mPivotToolbar2->addAction("Palette Ghost", [&]() {
			mPivotView->ghostPalette();
			});
		paletteGhost->setShortcut(QKeySequence(Qt::Key_Space));

		mPivotToolbar2->addSeparator();

		auto backGhost = mPivotToolbar2->addAction("Back Ghost");
		backGhost->setShortcut(QKeySequence(Qt::Key_B));
		backGhost->setCheckable(true);
		auto frontGhost = mPivotToolbar2->addAction("Front Ghost");
		frontGhost->setShortcut(QKeySequence(Qt::Key_F));
		frontGhost->setCheckable(true);

		auto ghostPosAction = new QActionGroup(this);
		ghostPosAction->addAction(backGhost);
		ghostPosAction->addAction(frontGhost);
		ghostPosAction->setExclusionPolicy(QActionGroup::ExclusionPolicy::Exclusive);
		connect(ghostPosAction, &QActionGroup::triggered, [&](QAction* action) {
			if (action->text() == "Back Ghost") {
				mPivotView->setGhostPosition(false);
			} else if (action->text() == "Front Ghost") {
				mPivotView->setGhostPosition(true);
			}
		});
		ghostPosAction->setEnabled(true);

		mPivotToolbar2->addSeparator();

		mPivotView = new PivotView(this);
		mPivotView->setMinimumWidth(750);
		mPivotView->sig_Modified.connect([&]() { setModified(); });
		mViewLayout->addWidget(mPivotView);

		addToolBarBreak();
		addToolBar(Qt::ToolBarArea::TopToolBarArea, mPivotToolbar1);
		addToolBarBreak();
		addToolBar(Qt::ToolBarArea::TopToolBarArea, mPivotToolbar2);
	}


	void AtlasSetup::setupClipMode() {
		mClipsToolbar = new QToolBar(this);
		QLabel* label = new QLabel("  FPS  ", mClipsToolbar);
		QSpinBox* box = new QSpinBox(mClipsToolbar);
		box->setMaximum(999);

		mClipsToolbar->addAction("Create Clip", [&]() {
			mClipsWidget->createClip();
			});
		auto removeClip = mClipsToolbar->addAction("Delete Clip", [&]() {
			mClipsWidget->deleteClip();
			}); removeClip->setShortcut(QKeySequence(Qt::Key_Delete));

			mClipsToolbar->addAction("Add Frame", [&]() {
				mClipsWidget->addFrame();
				});

			mClipsToolbar->addSeparator();

			auto recenterCam = mClipsToolbar->addAction("Recenter Camera", [&]() {
				mClipsWidget->recenterCamera();
				});
			recenterCam->setShortcut(QKeySequence(Qt::Key_G));
			mClipsToolbar->addSeparator();

			mClipsToolbar->addWidget(label);
			mClipsToolbar->addWidget(box);
			mClipsToolbar->addSeparator();

			mClipsWidget = new ClipsWidget(this);
			mClipsWidget->setMinimumWidth(750);
			mClipsWidget->sig_Modified.connect([&]() { setModified(); });
			mViewLayout->addWidget(mClipsWidget);

			mClipsTimer = new QTimer(this);
			connect(mClipsTimer, &QTimer::timeout, mClipsWidget, &ClipsWidget::animLoop);
			mClipsTimer->start(1000.0f / 30.0f);
			box->setValue(30.0f);

			connect(box, &QSpinBox::valueChanged, [&](int value) {
				mClipsTimer->stop();
				mClipsTimer->start(1000.0f / value);
			});

			addToolBar(Qt::ToolBarArea::TopToolBarArea, mClipsToolbar);
	}

	void AtlasSetup::updateEditorTitle(asset<Atlas> atlas) {
		string title = "Elang Atlas Editor";
		if (atlas) {
			title += " - ";
			title += atlas.get<GUIAsset>().filePath.generic_u8string();
			if (atlas.has<AssetModified>()) {
				title += "*";
			}
		}
		setWindowTitle(QString::fromUtf8(title));
	}

	void AtlasSetup::setModified() {
		auto atlas = gAtlsUtil.currentAtlas;
		if (!atlas.has<AssetModified>()) {
			atlas.add<AssetModified>();
			updateEditorTitle(atlas);
		}
	}
}
