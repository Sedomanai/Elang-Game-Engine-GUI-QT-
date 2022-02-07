#include "tree.h"

namespace el
{
	QTreeExtension::QTreeExtension(QWidget* parent) : suppressItemSelectedSignal(false)
	{
		setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
		connect(this, &QTreeExtension::itemSelectionChanged, [&]() {
			if (!suppressItemSelectedSignal) {
				emit sig_SelectionChanged();
			}
		});
	}

	QTreeExtension::~QTreeExtension() {}


	void QTreeExtension::dropEvent(QDropEvent* e) {
		if (e->source() == this) {
			emit sig_Move(itemAt(e->pos()));
		}
	}

	void QTreeExtension::focusInEvent(QFocusEvent* e) {
		QTreeWidget::focusInEvent(e);
		emit sig_FocusIn();
	}

	void QTreeExtension::focusOutEvent(QFocusEvent* e) {
		QTreeWidget::focusOutEvent(e);
		emit sig_FocusOut();
	}

	void QTreeExtension::mousePressEvent(QMouseEvent* e) {
		QTreeWidget::mousePressEvent(e);
	}

	void QTreeExtension::mouseReleaseEvent(QMouseEvent* e) {
		QTreeWidget::mouseReleaseEvent(e);
	}

	void QTreeExtension::keyPressEvent(QKeyEvent* e) {
		emit sig_KeyPressEvent(e);

		if (e->key() == Qt::Key_F2) {
			editItem(currentItem(), 0);
		}
	}

	void QTreeExtension::keyReleaseEvent(QKeyEvent* e) {
		emit sig_KeyReleaseEvent(e);
	}

	void QTreeExtension::getInsertPosition(QTreeWidgetItem* clicked, QTreeWidgetItem*& parent, uint& insertAt) {
		QAbstractItemView::DropIndicatorPosition dropIndicator = dropIndicatorPosition();
		insertAt = 0;
		parent = 0;
		switch (dropIndicator) {
		case QAbstractItemView::DropIndicatorPosition::BelowItem:
			insertAt++;
		case QAbstractItemView::DropIndicatorPosition::AboveItem:
			if (clicked->parent()) {
				parent = clicked->parent();
				insertAt += parent->indexOfChild(clicked);
			} else {
				parent = 0;
				insertAt += indexOfTopLevelItem(clicked);
			}
			break;
		case QAbstractItemView::DropIndicatorPosition::OnItem:
			parent = clicked;
			insertAt = parent->childCount();
			break;
		case QAbstractItemView::DropIndicatorPosition::OnViewport:
			parent = 0;
			insertAt = topLevelItemCount();
			break;
		}
	}
}