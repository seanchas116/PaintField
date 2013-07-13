#pragma once
#include "layer.h"

namespace PaintField {

class GroupLayer : public Layer
{
public:
	
	GroupLayer(const QString &name = QString()) : Layer(name) { setBlendMode(Malachite::BlendMode::PassThrough); }
	
	LayerRef createAnother() const override { return std::make_shared<GroupLayer>(); }
	bool canHaveChildren() const override { return true; }
	
	void updateThumbnail(const QSize &size) override;
};

class GroupLayerFactory : public LayerFactory
{
public:
	
	GroupLayerFactory() : LayerFactory() {}
	
	QString name() const override;
	LayerRef create() const override { return std::make_shared<GroupLayer>(); }
	const std::type_info &typeInfo() const override { return typeid(GroupLayer); }
};

} // namespace PaintField

