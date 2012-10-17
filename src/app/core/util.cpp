#include <QtGui>
#include <QJson/Parser>
#include "util.h"

namespace PaintField
{

QVariant loadJsonFromFile(const QString &path)
{
	QFile file(path);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		qWarning() << Q_FUNC_INFO << ": failed to open file";
		return QVariant();
	}
	
	QJson::Parser parser;
	return parser.parse(&file);
}

void applyMacSmallSize(QWidget *widget)
{
	widget->setAttribute(Qt::WA_MacSmallSize);
	foreach (QObject *object, widget->children())
	{
		QWidget *widget = qobject_cast<QWidget *>(object);
		if (widget)
			applyMacSmallSize(widget);
	}
}

}
