#include "rectlayer.h"

namespace PaintField {

bool RectLayer::setProperty(const QVariant &data, int role)
{
	switch (role)
	{
		case RoleRect:
			setRect(data.toRect());
			return true;
		case RoleRectShapeType:
			setShapeType(ShapeType(data.toInt()));
			return true;
		default:
			return super::setProperty(data, role);
	}
}

QVariant RectLayer::property(int role) const
{
	switch (role)
	{
		case RoleRect:
			return _rect;
		case RoleRectShapeType:
			return _shapeType;
		default:
			return super::property(role);
	}
}

void RectLayer::encode(QDataStream &stream) const
{
	super::encode(stream);
	stream << _rect << _shapeType;
}

void RectLayer::decode(QDataStream &stream)
{
	super::decode(stream);
	int shapeTypeInt;
	stream >> _rect >> shapeTypeInt;
	_shapeType = ShapeType(shapeTypeInt);
	
	setShapeFromRect();
}

QVariantMap mapFromRect(const QRectF &rect)
{
	QVariantMap map;
	map["x"] = rect.x();
	map["y"] = rect.y();
	map["width"] = rect.width();
	map["height"] = rect.height();
	return map;
}

QRectF rectFromMap(const QVariantMap &map)
{
	auto x = map["x"].toDouble();
	auto y = map["y"].toDouble();
	auto width = map["width"].toDouble();
	auto height = map["height"].toDouble();
	return QRectF(x, y, width, height);
}

QVariantMap RectLayer::saveProperies() const
{
	auto map = super::saveProperies();
	map["rect"] = mapFromRect(_rect);
	map["shapeType"] = shapeTypeString();
	return map;
}

void RectLayer::loadProperties(const QVariantMap &map)
{
	super::loadProperties(map);
	setRect(rectFromMap(map["rect"].toMap()));
	setShapeTypeString(map["shapeType"].toString());
}

void RectLayer::setRect(const QRectF &rect)
{
	_rect = rect;
	setShapeFromRect();
}

void RectLayer::setShapeType(ShapeType type)
{
	_shapeType = type;
	setShapeFromRect();
}

void RectLayer::setShapeTypeString(const QString &str)
{
	if (str == "ellipse")
		setShapeType(ShapeTypeEllipse);
	else
		setShapeType(ShapeTypeRect);
}

QString RectLayer::shapeTypeString() const
{
	switch (_shapeType)
	{
		default:
		case ShapeTypeRect:
			return "rect";
		case ShapeTypeEllipse:
			return "ellipse";
	}
}

QString RectLayerFactory::name() const
{
	return "rect";
}

void RectLayer::setShapeFromRect()
{
	QPainterPath path;
	path.addRect(_rect);
	this->setFillPath(path);
}

} // namespace PaintField
