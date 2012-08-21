#include <QWidget>

#include "fswidgetgroup.h"

void FSWidgetGroup::addWidgets(const QList<QWidget *> &widgets)
{
	foreach (QWidget *widget, widgets)
	{
		addWidget(widget);
	}
}

void FSWidgetGroup::setVisible(bool visible)
{
	foreach (QWidget *widget, _widgets)
	{
		widget->setVisible(visible);
	}
}

void FSWidgetGroup::setEnabled(bool enabled)
{
	foreach (QWidget *widget, _widgets)
	{
		widget->setEnabled(enabled);
	}
}

