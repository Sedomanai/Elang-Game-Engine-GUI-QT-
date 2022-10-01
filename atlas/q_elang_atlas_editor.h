#pragma once
#include <atlas/atlas_setup.h>

namespace el
{
	class QElangAtlasEditor : public AtlasSetup
	{
		Q_OBJECT

	public:
		QElangAtlasEditor(QWidget* parent = nullptr);

		signal<> sig_Shown;
	private:
		bool mSuppressClose;
		QTimer* mLogoTimer;

		void showEvent(QShowEvent* e) override;
		void connectActions();
		
		void newAtlas();
		void openTexture();
		void openAtlas();
		void saveAtlas();
		void backupAtlas();
		void refresh();
		void recordLastDirectoryHistory(const fio::path&);

		void debugTexture();
		void debugAtlas();
		void debugEditorView(bool anim = false);

		void beginWaitProcess();
		void endWaitProcess();
		QMessageBox::StandardButton askSaveMessage();

		void clearLists();
		void keyPressEvent(QKeyEvent*) override;
		void keyReleaseEvent(QKeyEvent*) override;
		void closeEvent(QCloseEvent*) override;
		bool eventFilter(QObject* watched, QEvent* event) override;
	};
}

