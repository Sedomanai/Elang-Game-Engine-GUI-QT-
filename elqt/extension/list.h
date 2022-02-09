#pragma once

#include <QListWidget>
#include <QDropEvent>
#include <QKeyEvent>
#include <common/signal.h>

#include "../elang_qt_builder.h"

namespace el {
	class _ELANGQT_EXPORT QListExtension : public QListWidget
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