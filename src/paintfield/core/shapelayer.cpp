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
		case RoleFillColor:
			_fillColor = data.value<Color>();
			return true;
		case RoleStrokeColor:
			_strokeColor = data.value<Color>();
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
		case RoleFillColor:
			return QVariant::fromValue(_fillColor);
		case RoleStrokeColor:
			return QVariant::fromValue(_strokeColor);
		case RoleFillEnabled:
			return _fillEnabled;
		case RoleStrokeEnabled:
			return _strokeEnabled;
		default:
			return super::property(role);
	}
}

void ShapeLayer::updateThumbnail(const QSize &size)
{
	QPixmap pixmap(size);
	pixmap.fill(Qt::transparent);
	
	{
		QPainter painter(&pixmap);
		
		painter.setBrush(_fillColor.toQColor());
		painter.drawPath(shape());
		
		painter.setBrush(_strokeColor.toQColor());
		painter.drawPath(strokePath());
	}
	
	setThumbnail(Thumbnail::createThumbnail(pixmap));
}

void ShapeLayer::render(Malachite::Painter *painter) const
{
	painter->setColor(_fillColor);
	painter->drawPath(shape());
	
	painter->setColor(_strokeColor);
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
