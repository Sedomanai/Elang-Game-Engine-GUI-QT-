#pragma once

#include <GL/glew.h>
#include <qdialog.h>
#include <qstring.h>
#include <qopenglcontext.h>
#include <apparatus/loader.h>

#include "elang_qt_assets.h"
#include "elang_qt_builder.h"
#include "elqt/dialog/new_project.h"


namespace el
{
	struct _ELANGQT_EXPORT ElangGUI
	{
		ElangGUI() : mProjectOpen(false), mOpenGLContext(0) {}

		Project rc, project;

		void makeNewProject(QWidget* parent);
		void loadCurrentProject(QWidget* parent);
		void loadDebugProject();
		void saveCurrentProject();
		void saveCurrentProjectAs(QWidget* parent);
		string enginePath() { return "D:/Programming/_Elang/___gui/"; }
		string projectPath() { return mProjectPath; }
		bool open() { return mProjectOpen; }
		void declareGlobalShareContext();
		
	private:
		QOpenGLContext* mOpenGLContext;
		string mProjectPath;
		bool mProjectOpen;
	};

	extern _ELANGQT_EXPORT ElangGUI gGUI;
}

