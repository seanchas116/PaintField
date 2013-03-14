#pragma once
#include "layer.h"

namespace PaintField {

class GroupLayer : public Layer
{
public:
	
	GroupLayer(const QString &name = QString()) : Layer(name) { setBlendMode(Malachite::BlendMode::PassThrough); }
	
	Layer *createAnother() const override { return new GroupLayer(); }
	bool canHaveChildren() const override { return true; }
	
	void updateThumbnail(const QSize &size) override;
};

class GroupLayerFactory : public LayerFactory
{
public:
	
	GroupLayerFactory() : LayerFactory() {}
	
	QString name() const override;
	Layer *create() const override { return new GroupLayer(); }
	const ::std::type_info &typeInfo() const override { return typeid(GroupLayer); }
};

} // namespace PaintField

