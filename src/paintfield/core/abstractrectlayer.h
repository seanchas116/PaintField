#pragma once

#include "shapelayer.h"

namespace PaintField {

class AbstractRectLayer : public ShapeLayer
{
public:
	
	typedef ShapeLayer super;
	
	enum ShapeType
	{
		ShapeTypeRect,
		ShapeTypeEllipse
	};
	
	AbstractRectLayer() : super() {}
	
	bool setProperty(const QVariant &data, int role) override;
	QVariant property(int role) const override;
	
	void encode(QDataStream &stream) const override;
	void decode(QDataStream &stream) override;
	
	QVariantMap saveProperies() const override;
	void loadProperties(const QVariantMap &map) override;
	
	void setRect(const QRectF &rect);
	QRectF rect() const { return _rect; }
	
	void setShapeType(ShapeType type);
	ShapeType shapeType() const { return _shapeType; }
	
	void setShapeTypeString(const QString &str);
	QString shapeTypeString() const;
	
protected:
	
	virtual void updateFillPath() = 0;
	
private:
	
	QRectF _rect;
	ShapeType _shapeType = ShapeTypeRect;
};

} // namespace PaintField
