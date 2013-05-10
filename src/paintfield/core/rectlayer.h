#pragma once

#include "abstractrectlayer.h"

namespace PaintField {

class RectLayer : public AbstractRectLayer
{
public:
	
	typedef AbstractRectLayer super;
	
	enum ShapeType
	{
		ShapeTypeRect,
		ShapeTypeEllipse
	};
	
	RectLayer() : super() {}
	
	LayerPtr createAnother() const override { return std::make_shared<RectLayer>(); }
	
protected:
	
	void updateFillPath() override;
};

class RectLayerFactory : public LayerFactory
{
public:
	
	RectLayerFactory() : LayerFactory() {}
	
	QString name() const override;
	LayerPtr create() const override { return std::make_shared<RectLayer>(); }
	const std::type_info &typeInfo() const override { return typeid(RectLayer); }
	
};

} // namespace PaintField
