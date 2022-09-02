/*****************************************************************//**
 * @file   tree.h
 * @brief  Custom extension for QTreeWidget. 
 *		   The functionalities of some of the built-in QT widgets is either not enough or requires a workaround.
 *		   This extension not only helps extend those functionalities, it also provides a catch-all interface 
 *		   that does not require one inheriting from either the base QT class or this class. 
 * 
 * @author Sedomanai
 * @date   August 2022
 *********************************************************************/
#pragma once

namespace el {
	/**
	 * Custom extension for QTreeWidget.
	 * The interface is built around signals and slots.
	 *
	 * Custom signals and slots were (will be - trees aren't used for now so they're on hold) used over QSignals and QSlots.
	 * The reason for that is actually consistency - there were some cases where QSignals and QSlots cannot satisfy.
	 * 
	 */
struct QTreeExtension : public QTreeWidget
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