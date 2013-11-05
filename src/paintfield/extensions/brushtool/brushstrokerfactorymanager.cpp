#include "paintfield/core/workspace.h"
#include "paintfield/core/palettemanager.h"

#include "brushstroker.h"

#include "brushstrokerfactorymanager.h"

namespace PaintField {

using namespace Malachite;

BrushStrokerFactoryManager::BrushStrokerFactoryManager(QObject *parent) :
    QObject(parent)
{
}

void BrushStrokerFactoryManager::addFactory(BrushStrokerFactory *factory)
{
	factory->setParent(this);
	mFactories << factory;
}

bool BrushStrokerFactoryManager::contains(const QString &name) const
{
	return mFactories++.find([&](BrushStrokerFactory *f){return f->name() == name;}).hasValue();
}

BrushStrokerFactory *BrushStrokerFactoryManager::factory(const QString &name)
{
	return mFactories++.find([&](BrushStrokerFactory *f){return f->name() == name;}).getOr(nullptr);
}

} // namespace PaintField
