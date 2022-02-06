#pragma once

#include <QListWidget>
#include <QDropEvent>
#include <QKeyEvent>

#include "../elang_qt_builder.h"

namespace el {
	class _ELANGQT_EXPORT QListExtension : public QListWidget
	{
		Q_OBJECT

	signals:
		void sig_FocusIn();
		void sig_FocusOut();
		void sig_Clicked();
		void sig_KeyPressEvent(QKeyEvent*);
	public:
		QListExtension(QWidget *parent);
		~QListExtension();

		void dropEvent(QDropEvent* event) override;
		void focusInEvent(QFocusEvent*) override;
		void focusOutEvent(QFocusEvent*) override;
		void mousePressEvent(QMouseEvent*) override;
		void keyPressEvent(QKeyEvent*) override;
	};
}