/*****************************************************************//**
 * @file   q_elang_logo.h
 * @brief  It's not unusual to show a splash logo while loading the entire assets necessary for an application.
 *         This class will show an image until the entire application loads and shows itself.
 * 
 * @author Sedomanai
 * @date   September 2022
 *********************************************************************/

#pragma once
#include <uic/ui_q_elang_logo.h>

namespace el
{
	class QElangLogo : public QWidget
	{
		Q_OBJECT

		QImage mLogo;
		QWidget* mWidget;
		QLabel* mLabel;

		int mCounter;
	public:
		QElangLogo(QWidget* parent = nullptr);
		~QElangLogo();

		signal<> sig_Loaded;

	private:
		void closeEvent(QCloseEvent* e);

		Ui::QElangLogoClass ui;
	};
}