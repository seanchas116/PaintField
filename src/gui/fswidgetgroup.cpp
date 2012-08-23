#include <QWidget>

#include "fswidgetgroup.h"

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

