#pragma once
#include "elang_qt_globals.h"


namespace el
{
	ElangGUI gGUI;

	void ElangGUI::makeNewProject(QWidget* parent) {
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
	void ElangGUI::loadCurrentProject(QWidget* parent) {
		mProjectPath = QFileDialog::getOpenFileName(
			parent, "Open Project", "D:/Programming/_Elang", "Elang Project (*.elang)"
		).toStdString();
		if (!mProjectPath.empty()) {
			bind(project);
			mProjectOpen = true;
			loadElangProject(mProjectPath.c_str(), true);
		}
	}

	void ElangGUI::loadDebugProject() {
		mProjectPath = "D:/Programming/_Elang/test/project.elang";
		bind(project);
		mProjectOpen = true;
		loadElangProject(mProjectPath.c_str(), true);
	}

	void ElangGUI::saveCurrentProject() {
		if (!mProjectPath.empty()) {
			bind(project);
			saveElangProject(mProjectPath.c_str());
		}
	}

	void ElangGUI::saveCurrentProjectAs(QWidget* parent) {
		mProjectPath = QFileDialog::getSaveFileName(
			parent, "Open Project", "D:/Programming/_Elang", "Elang Project (*.elang)"
		).toStdString();
		saveCurrentProject();
	}

	void ElangGUI::declareGlobalShareContext() {
		QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
	}
}

