#pragma once
#include <QtWidgets/QMainWindow>

#include "../elqt/extension/view.h"
#include "../elang_qt_builder.h"
#include "../elang_qt_globals.h"


namespace el
{
	class _ELANGQT_EXPORT ElangOpenGlInitializer : public QMainWindow
	{
		Q_OBJECT
			
	public:
		single_signal<> sig_Initialized;
		ElangOpenGlInitializer() : QMainWindow() {
			mSignature = new QElangViewSignaled(this);
			mSignature->sig_Start.connect([&]() {
				if (glewInit() != GLEW_OK)
					throw "GLEW could not initialize.";
				cout << "Global Qt OpenGL GLEW Initialized .." << endl;
				sig_Initialized.invoke();
				mSignature->hide();
				hide();
			});
		}

	private:
		QElangViewSignaled* mSignature;
	};
}
