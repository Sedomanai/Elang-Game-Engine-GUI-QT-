/*****************************************************************//**
 * @file   tree.h
 * @brief  Custom extension for QListWidget.
 *		   The functionalities of some of the built-in QT widgets is either not enough or requires a workaround.
 *		   This extension not only helps extend those functionalities, it also provides a catch-all interface
 *		   that does not require one inheriting from either the base QT class or this class.
 *
 * @author Sedomanai
 * @date   August 2022
 *********************************************************************/

#pragma once
#include <common/signal.h>

namespace el {
	/**
	 * Custom extension for QListWidget.
	 * The interface is built around signals and slots. 
	 * 
	 * Custom signals and slots were used over QSignals and QSlots. 
	 * The reason for that is actually consistency - there were some cases where QSignals and QSlots cannot satisfy. 
	 */
	class QListExtension : public QListWidget
	{
		Q_OBJECT

	public:
		signal<> sig_FocusIn;
		signal<> sig_FocusOut;
		signal<> sig_Clicked;
		signal<QKeyEvent*> sig_KeyPress;
		signal<QKeyEvent*> sig_KeyRelease;

		QListExtension(QWidget *parent);
		~QListExtension();

		QString getNoneConflictingName(const QString& string, bool self_compare = true);

		void dropEvent(QDropEvent* event) override;
		void focusInEvent(QFocusEvent*) override;
		void focusOutEvent(QFocusEvent*) override;
		void mousePressEvent(QMouseEvent*) override;
		void keyPressEvent(QKeyEvent*) override;
		void keyReleaseEvent(QKeyEvent*) override;
	};
}