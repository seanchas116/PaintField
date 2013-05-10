#include "rasterlayer.h"
#include "grouplayer.h"
#include "rectlayer.h"
#include "textlayer.h"

#include "layerfactorymanager.h"

namespace PaintField {

LayerFactoryManager::LayerFactoryManager(QObject *parent) :
	QObject(parent)
{
	_factories
			<< new RasterLayerFactory()
			<< new GroupLayerFactory()
			<< new RectLayerFactory()
			<< new TextLayerFactory();
}

LayerPtr LayerFactoryManager::createLayer(const QString &name) const
{
	for (auto factory : _factories)
	{
		if (factory->name() == name)
			return factory->create();
	}
	return 0;
}

QString LayerFactoryManager::nameForTypeInfo(const std::type_info &info) const
{
	for (auto factory : _factories)
	{
		if (factory->typeInfo() == info)
			return factory->name();
	}
	return QString();
}

LayerFactoryManager LayerFactoryManager::_instance;

} // namespace PaintField
