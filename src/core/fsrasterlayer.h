#ifndef FSRASTERLAYER_H
#define FSRASTERLAYER_H

#include "mlsurface.h"
#include "fslayer.h"

class FSRasterLayer : public FSLayer
{
public:
	FSRasterLayer(const QString &name = QString()) : FSLayer(name) {}
	FSRasterLayer(const FSRasterLayer &other) : FSLayer(other), _surface(other._surface) {}
	
	bool setProperty(const QVariant &data, int role);
	
	FSLayer *clone() const { return new FSRasterLayer(*this); }
	Type type() const { return FSLayer::TypeRaster; }
	
	MLSurface surface() const { return _surface; }
	void setSurface(const MLSurface &surface) { _surface = surface; }
	
	MLSurface *psurface() { return &_surface; }
	
private:
	MLSurface _surface;
};

#endif // FSRASTERLAYER_H
