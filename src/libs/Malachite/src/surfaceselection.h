#pragma once

//ExportName: SurfaceSelection

#include "surface.h"
#include <QPainterPath>

namespace Malachite
{

class MALACHITESHARED_EXPORT SurfaceSelection
{
public:
	
	enum Type
	{
		TypePath,
		TypeSurface,
		TypeWhole
	};
	
	SurfaceSelection() : _type(TypeWhole) {}
	SurfaceSelection(const QPainterPath &path) : _type(TypePath), _path(path) {}
	SurfaceSelection(const Surface &surface) : _type(TypeSurface), _surface(surface) {}
	
	Type type() const { return _type; }
	
	void setPath(const QPainterPath &path);
	QPainterPath path() const { return _path; }
	
	void setSurface(const Surface &surface);
	Surface surface() const { return _surface; }
	
	Surface clip(const Surface &surface) const;
	
private:
	
	Type _type;
	QPainterPath _path;
	Surface _surface;
};

}

