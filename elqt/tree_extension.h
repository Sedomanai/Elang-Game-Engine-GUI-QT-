#pragma once

#include <QTreeWidget>
#include <QDropEvent>

#include "../elang_qt_builder.h"

namespace el {
struct _ELANGQT_EXPORT QTreeExtension : public QTreeWidget
{
	using QTreeWidget::QTreeWidget;
	Q_OBJECT

signals:
	void sig_FocusIn();
	void sig_FocusOut();
	void sig_Clicked(QTreeWidgetItem*);
	void sig_Move(QTreeWidgetItem* clicked);
	void sig_MoveErase(QTreeWidgetItem* clicked);
	void sig_MoveInsert(QTreeWidgetItem* clicked, QTreeWidgetItem* parent, uint insertAt);
	void sig_KeyPressEvent(QKeyEvent*);
	void sig_KeyReleaseEvent(QKeyEvent*);
	void sig_SelectionChanged();
public:
	QTreeExtension(QWidget* parent);
	~QTreeExtension();

	void dropEvent(QDropEvent*) override;
	void focusInEvent(QFocusEvent*) override;
	void focusOutEvent(QFocusEvent*) override;
	void mousePressEvent(QMouseEvent*) override;
	void mouseReleaseEvent(QMouseEvent*) override;
	void keyPressEvent(QKeyEvent*) override;
	void keyReleaseEvent(QKeyEvent*) override;
	void getInsertPosition(QTreeWidgetItem* clicked, QTreeWidgetItem*& parent, uint& insertAt);

	bool suppressItemSelectedSignal;
};
}