#include <QtGui>

#include "fspanel.h"

void fsApplyMacSmallSize(QWidget *widget)
{
	widget->setAttribute(Qt::WA_MacSmallSize);
	foreach (QObject *object, widget->children())
	{
		QWidget *widget = qobject_cast<QWidget *>(object);
		if (widget)
			fsApplyMacSmallSize(widget);
	}
}

FSPanel::FSPanel(QWidget *parent) :
	QWidget(parent)
{
	setWindowFlags(Qt::Tool);
}

void FSPanel::closeEvent(QCloseEvent *event)
{
	event->ignore();
	hide();
}
