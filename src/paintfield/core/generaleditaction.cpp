#include <QApplication>

#include "generaleditaction.h"

namespace PaintField {

GeneralEditAction::GeneralEditAction(QObject *parent) :
	ProxyAction(parent)
{
	connect(qApp, SIGNAL(focusChanged(QWidget*,QWidget*)), this, SLOT(onFocusChanged(QWidget*,QWidget*)));
}

GeneralEditAction::GeneralEditAction(const QString &id, const QString &text, QObject *parent) :
	GeneralEditAction(parent)
{
	setObjectName(id);
	setText(text);
}

void GeneralEditAction::onFocusChanged(QWidget *, QWidget *now)
{
	for (auto action : now->actions())
	{
		if (action->objectName() == this->objectName())
		{
			setBackendAction(action);
			return;
		}
	}
	
	setBackendAction(0);
}

} // namespace PaintField
