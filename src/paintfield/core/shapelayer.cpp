#include <QPainterPathStroker>
#include <Malachite/Painter>
#include <Malachite/SurfacePainter>
#include "thumbnail.h"

#include "shapelayer.h"

using namespace Malachite;

namespace PaintField {

bool ShapeLayer::includes(const QPoint &pos, int margin) const
{
	QPainterPath marginPath;
	marginPath.addRect(pos.x() - margin, pos.y() - margin, margin * 2, margin * 2);
	return !(_unitedPath & marginPath).isEmpty();
	//return _unitedPath.contains(pos);
}

QPointSet ShapeLayer::tileKeys() const
{
	return Surface::rectToKeys(boundingRect().toAlignedRect());
}

bool ShapeLayer::setProperty(const QVariant &data, int role)
{
	switch (role)
	{
		case RoleStrokePosition:
			setStrokePosition(StrokePosition(data.toInt()));
			return true;
		case RoleStrokeWidth:
			setStrokeWidth(data.toDouble());
			return true;
		case RoleJoinStyle:
			setJoinStyle(Qt::PenJoinStyle(data.toInt()));
			return true;
		case RoleCapStyle:
			setCapStyle(Qt::PenCapStyle(data.toInt()));
			return true;
		case RoleFillBrush:
			setFillBrush(data.value<Brush>());
			return true;
		case RoleStrokeBrush:
			setStrokeBrush(data.value<Brush>());
			return true;
		case RoleFillEnabled:
			setFillEnabled(data.toBool());
			return true;
		case RoleStrokeEnabled:
			setStrokeEnabled(data.toBool());
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
	
	updatePaths();
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
		strokeMap["width"] = strokeWidth();
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
		setStrokeWidth(sm["width"].toDouble());
	}
	
	{
		auto fm = map["fill"].toMap();
		setFillEnabled(fm["enabled"].toBool());
		setFillBrush(brushFromMap(fm["brush"].toMap()));
	}
	
	updatePaths();
}

void ShapeLayer::updateThumbnail(const QSize &size)
{
	QPixmap pixmap(size);
	pixmap.fill(Qt::transparent);
	
	{
		QPainter painter(&pixmap);
		
		if (_fillEnabled)
		{
			painter.setBrush(_fillBrush.color().toQColor());
			painter.drawPath(fillPath());
		}
		
		if (_strokeEnabled)
		{
			painter.setBrush(_fillBrush.color().toQColor());
			painter.drawPath(strokePath());
		}
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
		setStrokePosition(StrokePositionInside);
	else if (string == "outside")
		setStrokePosition(StrokePositionOutside);
	else
		setStrokePosition(StrokePositionCenter);
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
		setJoinStyle(Qt::BevelJoin);
	else if (string == "round")
		setJoinStyle(Qt::RoundJoin);
	else
		setJoinStyle(Qt::MiterJoin);
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
		setCapStyle(Qt::SquareCap);
	else if (string == "round")
		setCapStyle(Qt::RoundCap);
	else
		setCapStyle(Qt::FlatCap);
}

void ShapeLayer::render(Malachite::Painter *painter) const
{
	auto surfacePainter = dynamic_cast<SurfacePainter *>(painter);
	if (surfacePainter)
	{
		if (!surfacePainter->keyClip().isEmpty())
		{
			if ((tileKeys() & surfacePainter->keyClip()).isEmpty())
				return;
		}
	}
	
	if (_fillEnabled)
	{
		painter->setBrush(_fillBrush);
		painter->drawPath(fillPath());
	}
	
	if (_strokeEnabled)
	{
		painter->setBrush(_strokeBrush);
		painter->drawPath(strokePath());
	}
}

void ShapeLayer::updatePaths()
{
	QPainterPathStroker stroker;
	
	if (_strokePos != StrokePositionCenter)
		stroker.setWidth(_strokeWidth * 2);
	else
		stroker.setWidth(_strokeWidth);
	
	stroker.setCapStyle(_capStyle);
	stroker.setJoinStyle(_joinStyle);
	
	auto fill = _fillPath;
	auto stroke = stroker.createStroke(fill).simplified();
	
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
	
	if (_strokePos == StrokePositionInside)
		_unitedPath = _fillPath;
	else
		_unitedPath = _fillPath | _strokePath;
	
	setThumbnailDirty(true);
}

void ShapeLayer::setFillPath(const QPainterPath &path)
{
	_fillPath = path;
	updatePaths();
}

QRectF ShapeLayer::boundingRect() const
{
	return _unitedPath.boundingRect();
}


} // namespace PaintField
