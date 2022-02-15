#include "list.h"
#include <common/string.h>
#include <common/container.h>

namespace el {
	QListExtension::QListExtension(QWidget *parent)
		: QListWidget(parent)
	{
		setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
	}

	QListExtension::~QListExtension()
	{

	}

	QString QListExtension::getNoneConflictingName(const QString& name_, bool self_compare) {
		auto name = name_.toUtf8();
		auto curr = currentItem();

		string num; num.reserve(32);
		vector<int> numbers;
		while (name.size() > 0 && name.back() == '_')
			name.erase(name.end() - 1, name.end());

		for (uint i = 0; i < count(); i++) {
			auto it = item(i);
			if (!self_compare || it != curr) {
				auto comp = it->text().toStdString();

				auto it = comp.find(name);
				if (it == 0) {
					auto cs = comp.size();
					auto ns = name.size();
					if (cs == ns) {
						numbers.push_back(-1);
					}
					else {
						assert(cs > ns);
						auto rem = strview(&comp[ns], cs - ns);

						if (rem[0] == '_') {
							for (sizet i = 1; i < rem.size(); i++) {
								if (isdigit(rem[i]))
									num.push_back(rem[i]);
								else {
									num.clear();
									break;
								}
							}
							if (num.size() > 0) {
								numbers.push_back(toInt(num));
								num.clear();
							}
						}
					}
				}
			}
		}

		int intmax = -2;
		for (sizet i = 0; i < numbers.size(); i++) {
			if (intmax < numbers[i]) {
				intmax = numbers[i];
			}
		}

		if (intmax != -2) {
			name.push_back('_');
			name.append(std::to_string(intmax + 1));
		}

		return QString::fromUtf8(name);
	}

	void QListExtension::dropEvent(QDropEvent* e) {
		if (e->source() == this) {
			QListWidget::dropEvent(e);
		}
	}

	void QListExtension::focusInEvent(QFocusEvent* e) {
		QListWidget::focusInEvent(e);
		sig_FocusIn.invoke();
	}

	void QListExtension::focusOutEvent(QFocusEvent* e) {
		QListWidget::focusOutEvent(e);
		sig_FocusOut.invoke();
	}

	void QListExtension::mousePressEvent(QMouseEvent* e) {
		QListWidget::mousePressEvent(e);
		sig_Clicked.invoke();
	}

	void QListExtension::keyPressEvent(QKeyEvent* e) {
		QListWidget::keyPressEvent(e);
		sig_KeyPress.invoke(e);
	}
	void QListExtension::keyReleaseEvent(QKeyEvent* e) {
		QListWidget::keyReleaseEvent(e);
		sig_KeyRelease.invoke(e);
	}
}