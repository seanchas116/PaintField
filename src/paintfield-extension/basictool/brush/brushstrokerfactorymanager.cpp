#include "paintfield-core/workspace.h"
#include "paintfield-core/palettemanager.h"

#include "brushstroker.h"

#include "brushstrokerfactorymanager.h"

namespace PaintField {

BrushStrokerFactoryManager::BrushStrokerFactoryManager(QObject *parent) :
    QObject(parent)
{
}

void BrushStrokerFactoryManager::addFactory(BrushStrokerFactory *factory)
{
	factory->setParent(this);
	_factories << factory;
}

bool BrushStrokerFactoryManager::contains(const QString &name) const
{
	return _factories.find([&](BrushStrokerFactory *f){return f->name() == name;}, 0);
}

BrushStrokerFactory *BrushStrokerFactoryManager::factory(const QString &name)
{
	return _factories.find([&](BrushStrokerFactory *f){return f->name() == name;}, 0);
}

} // namespace PaintField
