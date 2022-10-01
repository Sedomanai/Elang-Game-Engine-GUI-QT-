#include <elqtpch.h>
#include "q_elang_logo.h"
#include <apparatus/asset_loader.h>

namespace el
{
	QElangLogo::QElangLogo(QWidget* parent)
		: QWidget(parent) {
		setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
		setAttribute(Qt::WA_NoSystemBackground);
		setAttribute(Qt::WA_TranslucentBackground);
		setAttribute(Qt::WA_TransparentForMouseEvents);
		setAttribute(Qt::WA_DeleteOnClose);

		ui.setupUi(this);

		cout << "Initiating..." << endl;
		if (mLogo.load("../___gui/logo.png")) {
			mWidget = new QWidget();
			mWidget->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
			mWidget->setAttribute(Qt::WA_NoSystemBackground);
			mWidget->setAttribute(Qt::WA_TranslucentBackground);
			mWidget->setAttribute(Qt::WA_TransparentForMouseEvents);

			auto label = new QLabel(mWidget);
			auto pixmap = QPixmap::fromImage(mLogo);
			label->setPixmap(pixmap);
			label->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

			QPalette paleta;
			paleta.setBrush(mWidget->backgroundRole(), QBrush(pixmap));
			label->setPalette(paleta);
 
			setMaximumSize(label->pixmap().size());
			mWidget->setMaximumSize(size());
			label->setMinimumSize(size());

			mWidget->show();
		}
		
		// don't ask why
		mCounter = 2;
		ui.openGLWidget->sig_Paint.connect([&]() {
			if (mCounter >= 0) {
				mCounter--;
				if (mCounter == 0 && mWidget) {
					cout << "Initializing GLEW..." << endl;
					glewInit();
					AssetLoader loader;
					cout << "Importing Native GUI..." << endl;
					loader.initNativeGUI();
					loader.importAllNativeGUIAssets();
					sig_Loaded.invoke();
				}
			}
		});
	}

	void QElangLogo::closeEvent(QCloseEvent* e) {
		mLogo = QImage();
		delete mWidget;
		mWidget = 0;
	}

	QElangLogo::~QElangLogo() {
	

	}
}