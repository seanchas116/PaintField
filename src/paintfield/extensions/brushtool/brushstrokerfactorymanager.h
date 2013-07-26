#pragma once

#include <QObject>
#include <QVariant>
#include <Malachite/List>

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
	
signals:
	
public slots:
	
private:
	
	Malachite::List<BrushStrokerFactory *> _factories;
};

} // namespace PaintField

