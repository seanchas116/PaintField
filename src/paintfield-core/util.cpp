#include <QtGui>
#include <qjson/parser.h>
#include <qjson/serializer.h>

#include "debug.h"
#include "util.h"

namespace PaintField
{

QAction *createAction(const QString &id, QObject *parent)
{
	auto action = new QAction(parent);
	action->setObjectName(id);
	return action;
}

QAction *createAction(const QString &id, QObject *receiver, const char *onTriggeredSlot, QObject *parent)
{
	auto action = createAction(id, parent);
	QObject::connect(action, SIGNAL(triggered()), receiver, onTriggeredSlot);
	return action;
}

QVariant loadJsonFromFile(const QString &path)
{
	QFile file(path);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		PAINTFIELD_DEBUG << "failed to open file";
		return QVariant();
	}
	
	QJson::Parser parser;
	return parser.parse(&file);
}

bool saveJsonToFile(const QString &path, const QVariant &data)
{
	QJson::Serializer serializer;
	bool ok;
	
	QFile file(path);
	
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		PAINTFIELD_DEBUG << "failed to save file";
		return false;
	}
	serializer.serialize(data, &file, &ok);
	
	return ok;
}

void applyMacSmallSize(QWidget *widget)
{
	widget->setAttribute(Qt::WA_MacSmallSize);
	
	for (QObject *object : widget->children())
	{
		QWidget *widget = qobject_cast<QWidget *>(object);
		if (widget)
			applyMacSmallSize(widget);
	}
}

QString unduplicatedName(const QStringList &existingNames, const QString &name)
{
	if (!existingNames.contains(name))
		return name;
	
	int i = 0;
	forever
	{
		QString newName = name + " #" + QString::number(++i);
		if (!existingNames.contains(newName))
			return newName;
	}
}

QString fileDialogFilterFromExtensions(const QStringList &extensions)
{
	QString ret;
	
	ret += "(";
	
	for (auto ext : extensions)
		ret += "* " + ext;
	
	ret += ")";
	
	return ret;
}

QTransform makeTransform(double scale, double rotation, const QPointF &translation)
{
	QTransform transform;
	
	if (translation != QPointF(0, 0))
		transform.translate(translation.x(), translation.y());
	
	if (rotation)
		transform.rotate(rotation);
	
	if (scale != 1.0)
		transform.scale(scale, scale);
	
	return transform;
}

void maximizeWindowSize(QWidget *widget)
{
	auto availableGeom = qApp->desktop()->availableGeometry();
	
	QPoint offset = widget->geometry().topLeft() - widget->pos();
	
	QSize size(availableGeom.width() - offset.x(), availableGeom.height() - offset.y());
	widget->move(availableGeom.topLeft());
	widget->resize(size);
}

}
