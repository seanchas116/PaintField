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
	
	Layer *createAnother() const override { return new RectLayer(); }
	
protected:
	
	void updateFillPath() override;
};

class RectLayerFactory : public LayerFactory
{
public:
	
	RectLayerFactory() : LayerFactory() {}
	
	QString name() const override;
	Layer *create() const override { return new RectLayer(); }
	const ::std::type_info &typeInfo() const override { return typeid(RectLayer); }
	
};

} // namespace PaintField
