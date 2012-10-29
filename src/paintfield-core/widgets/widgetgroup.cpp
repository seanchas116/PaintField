#include <QWidget>

#include "widgetgroup.h"

namespace PaintField
{

void WidgetGroup::setVisible(bool visible)
{
	foreach (QWidget *widget, _widgets)
	{
		widget->setVisible(visible);
	}
}

void WidgetGroup::setEnabled(bool enabled)
{
	foreach (QWidget *widget, _widgets)
	{
		widget->setEnabled(enabled);
	}
}

}

