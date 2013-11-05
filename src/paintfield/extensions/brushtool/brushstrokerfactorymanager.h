#pragma once

#include <QObject>
#include <QVariant>
#include "paintfield/core/global.h"

namespace Malachite
{
class Surface;
}

namespace PaintField
{

class BrushStrokerFactory;
class BrushStroker;

class BrushStrokerFactoryManager : public QObject
{
	Q_OBJECT
public:
	explicit BrushStrokerFactoryManager(QObject *parent = 0);
	
	void addFactory(BrushStrokerFactory *factory);
	
	bool contains(const QString &name) const;
	QVariantMap defaultSettings(const QString &name) const;
	BrushStrokerFactory *factory(const QString &name);
	QList<BrushStrokerFactory *> factories() { return mFactories; }
	
private:
	QList<BrushStrokerFactory *> mFactories;
};

} // namespace PaintField

