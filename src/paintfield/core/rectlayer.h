#pragma once

#include "shapelayer.h"

namespace PaintField {

class RectLayer : public ShapeLayer
{
public:
	
	typedef ShapeLayer super;
	
	enum ShapeType
	{
		ShapeTypeRect,
		ShapeTypeEllipse
	};
	
	RectLayer() : super() {}
	
	Layer *createAnother() const override { return new RectLayer(); }
	
	bool setProperty(const QVariant &data, int role) override;
	QVariant property(int role) const override;
	
	void encode(QDataStream &stream) const override;
	void decode(QDataStream &stream) override;
	
	QVariantMap saveProperies() const override;
	void loadProperties(const QVariantMap &map) override;
	
	void setRect(const QRectF &rect);
	QRectF rect() const { return _rect; }
	
	void setShapeType(ShapeType type) { _shapeType = type; }
	ShapeType shapeType() const { return _shapeType; }
	
	void setShapeTypeString(const QString &str);
	QString shapeTypeString() const;
	
private:
	
	QRectF _rect;
	ShapeType _shapeType = ShapeTypeRect;
};

} // namespace PaintField
