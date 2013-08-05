#pragma once

#include <QObject>
#include "layer.h"
#include <typeinfo>

namespace PaintField {

class LayerFactoryManager : public QObject
{
	Q_OBJECT
public:
	explicit LayerFactoryManager(QObject *parent = 0);
	
	LayerRef createLayer(const QString &name) const;
	
	template <class T> QString nameForType() const { return nameForTypeInfo(typeid(T)); }
	QString nameForTypeInfo(const std::type_info &info) const;
	
	static LayerFactoryManager instance;
	
signals:
	
public slots:
	
private:
	
	QList<LayerFactory *> _factories;
};

inline LayerFactoryManager *layerFactoryManager() { return &LayerFactoryManager::instance; }

} // namespace PaintField
