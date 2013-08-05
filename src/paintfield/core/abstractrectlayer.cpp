#include "abstractrectlayer.h"

namespace PaintField {

bool AbstractRectLayer::setProperty(const QVariant &data, int role)
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

QVariant AbstractRectLayer::property(int role) const
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

void AbstractRectLayer::encode(QDataStream &stream) const
{
	super::encode(stream);
	stream << _rect << _shapeType;
}

void AbstractRectLayer::decode(QDataStream &stream)
{
	super::decode(stream);
	int shapeTypeInt;
	stream >> _rect >> shapeTypeInt;
	_shapeType = ShapeType(shapeTypeInt);
	
	updateFillPath();
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

QVariantMap AbstractRectLayer::saveProperties() const
{
	auto map = super::saveProperties();
	map["rect"] = mapFromRect(_rect);
	map["shapeType"] = shapeTypeString();
	return map;
}

void AbstractRectLayer::loadProperties(const QVariantMap &map)
{
	super::loadProperties(map);
	setRect(rectFromMap(map["rect"].toMap()));
	setShapeTypeString(map["shapeType"].toString());
}

void AbstractRectLayer::setRect(const QRectF &rect)
{
	_rect = rect;
	updateFillPath();
}

void AbstractRectLayer::setShapeType(ShapeType type)
{
	_shapeType = type;
	updateFillPath();
}

void AbstractRectLayer::setShapeTypeString(const QString &str)
{
	if (str == "ellipse")
		setShapeType(ShapeTypeEllipse);
	else
		setShapeType(ShapeTypeRect);
}

QString AbstractRectLayer::shapeTypeString() const
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


} // namespace PaintField
