#include <QPainterPathStroker>
#include <Malachite/Painter>
#include "thumbnail.h"

#include "shapelayer.h"

using namespace Malachite;

namespace PaintField {

bool ShapeLayer::setProperty(const QVariant &data, int role)
{
	switch (role)
	{
		case RoleStrokePosition:
			_strokePos = StrokePosition(data.toInt());
			return true;
		case RoleStrokeWidth:
			_strokeWidth = data.toDouble();
			return true;
		case RoleJoinStyle:
			_joinStyle = Qt::PenJoinStyle(data.toInt());
			return true;
		case RoleCapStyle:
			_capStyle = Qt::PenCapStyle(data.toInt());
			return true;
		case RoleFillBrush:
			_fillBrush = data.value<Brush>();
			return true;
		case RoleStrokeBrush:
			_strokeBrush = data.value<Brush>();
			return true;
		case RoleFillEnabled:
			_fillEnabled = data.toBool();
			return true;
		case RoleStrokeEnabled:
			_strokeEnabled = data.toBool();
			return true;
		default:
			return super::setProperty(data, role);
	}
}

QVariant ShapeLayer::property(int role) const
{
	switch (role)
	{
		case RoleStrokePosition:
			return _strokePos;
		case RoleStrokeWidth:
			return _strokeWidth;
		case RoleJoinStyle:
			return _joinStyle;
		case RoleCapStyle:
			return _capStyle;
		case RoleFillBrush:
			return QVariant::fromValue(_fillBrush);
		case RoleStrokeBrush:
			return QVariant::fromValue(_strokeBrush);
		case RoleFillEnabled:
			return _fillEnabled;
		case RoleStrokeEnabled:
			return _strokeEnabled;
		default:
			return super::property(role);
	}
}

void ShapeLayer::encode(QDataStream &stream) const
{
	super::encode(stream);
	
	stream << _strokePos << _strokeWidth << _joinStyle << _capStyle << _fillBrush.color() << _strokeBrush.color() << _fillEnabled << _strokeEnabled;
}

void ShapeLayer::decode(QDataStream &stream)
{
	super::decode(stream);
	
	int strokePos, joinStyle, capStyle;
	Malachite::Color fillColor, strokeColor;
	stream >> strokePos >> _strokeWidth >> joinStyle >> capStyle >> fillColor >> strokeColor >> _fillEnabled >> _strokeEnabled;
	_strokePos = (StrokePosition)strokePos;
	_joinStyle = (Qt::PenJoinStyle)joinStyle;
	_capStyle = (Qt::PenCapStyle)capStyle;
	_fillBrush = fillColor;
	_strokeBrush = strokeColor;
}


static QVariantMap mapFromColor(const Color &c)
{
	QVariantMap map;
	map["a"] = c.alpha();
	map["r"] = c.red();
	map["g"] = c.green();
	map["b"] = c.blue();
	return map;
}

static Color colorFromMap(const QVariantMap &map)
{
	return Color::fromRgbValue(map["r"].toDouble(), map["g"].toDouble(), map["b"].toDouble(), map["a"].toDouble());
}

static QVariantMap mapFromBrush(const Brush &brush)
{
	QVariantMap map;
	
	switch (brush.type())
	{
		// TODO: other types
		default:
		case BrushTypeColor:
		{
			map["type"] = "color";
			map["color"] = mapFromColor(brush.color());
			break;
		}
	}
	
	return map;
}

static Brush brushFromMap(const QVariantMap &map)
{
	auto type = map["type"].toString();
	
	if (type == "color")
		return colorFromMap(map["color"].toMap());
	else
		return Brush();
}


QVariantMap ShapeLayer::saveProperies() const
{
	QVariantMap map = super::saveProperies();
	
	{
		QVariantMap strokeMap;
		strokeMap["position"] = strokePositionString();
		strokeMap["joinStyle"] = joinStyleString();
		strokeMap["capStyle"] = capStyleString();
		strokeMap["enabled"] = _strokeEnabled;
		strokeMap["brush"] = mapFromBrush(_strokeBrush);
		map["stroke"] = strokeMap;
	}
	
	{
		QVariantMap fillMap;
		fillMap["enabled"] = _fillEnabled;
		fillMap["brush"] = mapFromBrush(_fillBrush);
		map["fill"] = fillMap;
	}
	
	return map;
}

void ShapeLayer::loadProperties(const QVariantMap &map)
{
	super::loadProperties(map);
	
	{
		auto sm = map["stroke"].toMap();
		setStrokePositionString(sm["position"].toString());
		setJoinStyleString(sm["joinStyle"].toString());
		setCapStyleString(sm["capStyle"].toString());
		setStrokeEnabled(sm["enabled"].toBool());
		setStrokeBrush(brushFromMap(sm["brush"].toMap()));
	}
	
	{
		auto fm = map["fill"].toMap();
		setFillEnabled(fm["enabled"].toBool());
		setFillBrush(brushFromMap(fm["brush"].toMap()));
	}
}

void ShapeLayer::updateThumbnail(const QSize &size)
{
	QPixmap pixmap(size);
	pixmap.fill(Qt::transparent);
	
	{
		QPainter painter(&pixmap);
		
		painter.setBrush(_fillBrush.color().toQColor());
		painter.drawPath(shape());
		
		painter.setBrush(_fillBrush.color().toQColor());
		painter.drawPath(strokePath());
	}
	
	setThumbnail(Thumbnail::createThumbnail(pixmap));
}

QString ShapeLayer::strokePositionString() const
{
	switch (_strokePos)
	{
		case StrokePositionInside:
			return "inside";
		default:
		case StrokePositionCenter:
			return "center";
		case StrokePositionOutside:
			return "outside";
	}
}

void ShapeLayer::setStrokePositionString(const QString &string)
{
	if (string == "inside")
		_strokePos = StrokePositionInside;
	else if (string == "outside")
		_strokePos = StrokePositionOutside;
	else
		_strokePos = StrokePositionCenter;
}

QString ShapeLayer::joinStyleString() const
{
	switch (_joinStyle)
	{
		default:
		case Qt::MiterJoin:
			return "miter";
		case Qt::BevelJoin:
			return "bevel";
		case Qt::RoundJoin:
			return "round";
	}
}

void ShapeLayer::setJoinStyleString(const QString &string)
{
	if (string == "bevel")
		_joinStyle = Qt::BevelJoin;
	else if (string == "round")
		_joinStyle = Qt::RoundJoin;
	else
		_joinStyle = Qt::MiterJoin;
}

QString ShapeLayer::capStyleString() const
{
	switch (_capStyle)
	{
		default:
		case Qt::SquareCap:
			return "square";
		case Qt::FlatCap:
			return "flat";
		case Qt::RoundCap:
			return "round";
	}
}

void ShapeLayer::setCapStyleString(const QString &string)
{
	if (string == "square")
		_capStyle = Qt::SquareCap;
	else if (string == "round")
		_capStyle = Qt::RoundCap;
	else
		_capStyle = Qt::FlatCap;
}

void ShapeLayer::render(Malachite::Painter *painter) const
{
	painter->setBrush(_fillBrush);
	painter->drawPath(shape());
	
	painter->setBrush(_strokeBrush);
	painter->drawPath(strokePath());
}

void ShapeLayer::updateStrokePath()
{
	QPainterPathStroker stroker;
	
	if (_strokePos != StrokePositionCenter)
		stroker.setWidth(_strokeWidth * 2);
	else
		stroker.setWidth(_strokeWidth);
	
	stroker.setCapStyle(_capStyle);
	stroker.setJoinStyle(_joinStyle);
	
	auto fill = _fillPath;
	auto stroke = stroker.createStroke(fill);
	
	switch (_strokePos)
	{
		default:
		case StrokePositionCenter:
			_strokePath = stroke;
			break;
		case StrokePositionInside:
			_strokePath = stroke & fill;
			break;
		case StrokePositionOutside:
			_strokePath = stroke - fill;
			break;
	}
}

void ShapeLayer::setFillPath(const QPainterPath &path)
{
	_fillPath = path;
	updateStrokePath();
}


} // namespace PaintField
