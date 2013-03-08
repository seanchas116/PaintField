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
	
}

void ShapeLayer::render(Malachite::Painter *painter) const
{
	
}

} // namespace PaintField
