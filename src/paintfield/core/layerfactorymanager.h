#pragma once

#include <QObject>
#include <typeinfo>

namespace PaintField {

class LayerFactory;
class Layer;

class LayerFactoryManager : public QObject
{
	Q_OBJECT
public:
	explicit LayerFactoryManager(QObject *parent = 0);
	
	Layer *createLayer(const QString &name) const;
	
	template <class T> QString nameForType() const { return nameForTypeInfo(typeid(T)); }
	QString nameForTypeInfo(const ::std::type_info &info) const;
	
	static LayerFactoryManager *instance() { return &_instance; }
	
signals:
	
public slots:
	
private:
	
	QList<LayerFactory *> _factories;
	
	static LayerFactoryManager _instance;
};

inline LayerFactoryManager *layerFactoryManager() { return LayerFactoryManager::instance(); }

} // namespace PaintField
