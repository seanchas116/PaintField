#pragma once
#include "layer.h"

namespace PaintField {

class GroupLayer : public Layer
{
public:
	
	GroupLayer(const QString &name = QString()) : Layer(name) { setBlendMode(Malachite::BlendMode::PassThrough); }
	GroupLayer(const GroupLayer &other) : Layer(other) {}
	
	Layer *clone() const override { return new GroupLayer(*this); }
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

