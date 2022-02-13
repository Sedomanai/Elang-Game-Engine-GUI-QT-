#pragma once

#include <GL/glew.h>
#include <qdialog.h>
#include <qstring.h>
#include <qopenglcontext.h>

#include <elang_gui_assets.h>
#include <apparatus/loader.h>


#include "elang_qt_builder.h"
#include "elqt/dialog/new_project.h"


namespace el
{
	struct QElangView;
	struct _ELANGQT_EXPORT ElangGUI
	{
		ElangGUI() : mProjectOpen(false), mOpenGLContext(0), mSignature(0) {}

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

		void initializeGlobalOpenGL(QWidget* widget);
		
	private:
		void loadAllAssets();
		QElangView* mSignature;
		QOpenGLContext* mOpenGLContext;
		string mProjectPath;
		bool mProjectOpen;
	};

	extern _ELANGQT_EXPORT ElangGUI gGUI;
}

