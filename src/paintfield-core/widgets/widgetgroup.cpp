#include <QWidget>

#include "widgetgroup.h"

namespace PaintField
{

void WidgetGroup::setVisible(bool visible)
{
	for (QWidget *widget : _widgets)
		widget->setVisible(visible);
}

void WidgetGroup::setEnabled(bool enabled)
{
	for (QWidget *widget : _widgets)
		widget->setEnabled(enabled);
}

}

