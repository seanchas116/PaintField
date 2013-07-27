#include "surfacepainter.h"
#include "surfaceselection.h"

namespace Malachite
{

void SurfaceSelection::setPath(const QPainterPath &path)
{
	_type = TypePath;
	_path = path;
	_surface = Surface();
}

void SurfaceSelection::setSurface(const Surface &surface)
{
	_type = TypeSurface;
	_path = QPainterPath();
	_surface = surface;
}

Surface SurfaceSelection::clip(const Surface &surface) const
{
	if (_type == TypeWhole)
		return surface;
	
	Surface result = surface;
	SurfacePainter painter(&result);
	painter.setBlendMode(BlendMode::DestinationIn);
	
	switch (_type)
	{
	case TypePath:
		painter.drawPath(_path);
		break;
	case TypeSurface:
		painter.drawSurface(0, 0, _surface);
		break;
	default:
		Q_ASSERT(0);
		break;
	}
	
	painter.flush();
	return result;
}

}
