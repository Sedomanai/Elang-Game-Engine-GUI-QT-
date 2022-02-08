#pragma once

#include <qdialog.h>
#include <qstring.h>
#include <apparatus/loader.h>

#include "elang_qt_builder.h"
#include "elqt/dialog/new_project.h"


namespace el
{
	struct _ELANGQT_EXPORT ElangGUI
	{
		ElangGUI() : mProjectOpen(false) {}

		Project rc, project;

		void makeNewProject(QWidget* parent) {
			NewProjectDialog dialog(parent);
			dialog.exec();
			mProjectPath = dialog.saveNewProject(project).toStdString();
			if (!mProjectPath.empty() && !project.assetDir.empty()) {
				mProjectOpen = true;
				reloadAll(true);
				saveElangProject(mProjectPath.c_str());
				initElang();
			}
		}

		void loadCurrentProject(QWidget* parent) {
			mProjectPath = QFileDialog::getOpenFileName(
				parent, "Open Project", "D:/Programming/_Elang", "Elang Project (*.elang)"
			).toStdString();
			if (!mProjectPath.empty()) {
				bind(project);
				mProjectOpen = true;
				loadElangProject(mProjectPath.c_str(), true);
			}
		}

		void loadDebugProject() {
			mProjectPath = "D:/Programming/_Elang/test/project.elang";
			bind(project);
			mProjectOpen = true;
			loadElangProject(mProjectPath.c_str(), true);
		}

		void saveCurrentProject() {
			if (!mProjectPath.empty()) {
				bind(project);
				saveElangProject(mProjectPath.c_str());
			}
		}

		void saveCurrentProjectAs(QWidget* parent) {
			mProjectPath = QFileDialog::getSaveFileName(
				parent, "Open Project", "D:/Programming/_Elang", "Elang Project (*.elang)"
			).toStdString();
			saveCurrentProject();
		}

		string enginePath() { return "D:/Programming/_Elang/___gui/"; }
		string projectPath() { return mProjectPath; }
		bool open() { return mProjectOpen; }
		
	private:
		string mProjectPath;
		bool mProjectOpen;
	};

	extern _ELANGQT_EXPORT ElangGUI gGUI;
}

