#pragma once

#include <qmessagebox.h>
#include <qfiledialog>
#include <qdialog>
#include <uic/ui_new_project.h>

#include <apparatus/loader.h>
#include "../../elang_qt_builder.h"


namespace el {

	class _ELANGQT_EXPORT NewProjectDialog : public QDialog
	{
		Q_OBJECT

	public:
		NewProjectDialog(QWidget* parent = Q_NULLPTR);
		QString saveNewProject(Project& project);
		bool accepted;
	private:
		Ui::NewProjectUI ui;
	};
} 
