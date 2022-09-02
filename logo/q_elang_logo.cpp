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
					glewInit();
					AssetLoader loader;
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


	//asset<Material> QElangLogo::importSplashLogo(fio::path absPath) {
	//	auto tex = gProject.make<Texture>();
	//	tex->importFile(absPath, *tex.add<TextureMeta>());
	//	tex.add<Material>()->setTexture(tex, 0);
	//	tex.add<Painter>()->init();


	//	return mat;
	//}

	//void QElangLogo::unloadSplashLogo(asset<Material> mat) {
	//	if (mat->hasTexture()) {
	//		auto tex = mat->textures[0];
	//		tex->unload(tex.get<TextureMeta>());
	//		tex.destroy();
	//	} mat.destroy();
	//}
}