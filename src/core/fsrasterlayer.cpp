#include "fsrasterlayer.h"

bool FSRasterLayer::setProperty(const QVariant &data, int role)
{
	switch (role)
	{
	case FSGlobal::RoleSurface:
		_surface = data.value<MLSurface>();
		return true;
	default:
		return FSLayer::setProperty(data, role);
	}
}
