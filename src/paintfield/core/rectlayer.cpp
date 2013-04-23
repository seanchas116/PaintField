#include "rectlayer.h"

namespace PaintField {

void RectLayer::updateFillPath()
{
	QPainterPath path;
	
	switch (shapeType())
	{
		default:
		case ShapeTypeRect:
			path.addRect(rect());
			break;
		case ShapeTypeEllipse:
			path.addEllipse(rect());
			break;
	}
	
	this->setFillPath(path);
}

QString RectLayerFactory::name() const { return "rect"; }

} // namespace PaintField
