#include "list_extension.h"

namespace el {
	QListExtension::QListExtension(QWidget *parent)
		: QListWidget(parent)
	{
		setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
	}

	QListExtension::~QListExtension()
	{

	}

	void QListExtension::dropEvent(QDropEvent* e) {
		if (e->source() == this) {
			QListWidget::dropEvent(e);
		}
	}

	void QListExtension::focusInEvent(QFocusEvent* e) {
		QListWidget::focusInEvent(e);
		emit sig_FocusIn();
	}

	void QListExtension::focusOutEvent(QFocusEvent* e) {
		QListWidget::focusOutEvent(e);
		emit sig_FocusOut();
	}

	void QListExtension::mousePressEvent(QMouseEvent* e) {
		QListWidget::mousePressEvent(e);
		emit sig_Clicked();
	}

	void QListExtension::keyPressEvent(QKeyEvent* e) {
		QListWidget::keyPressEvent(e);
		emit sig_KeyPressEvent(e);
	}
}