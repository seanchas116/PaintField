#pragma once
#include <Malachite/Color>
#include "layer.h"

namespace Malachite
{
class Painter;
}

namespace PaintField {

class ShapeLayer : public Layer
{
public:
	
	typedef Layer super;
	
	ShapeLayer() : Layer() {}
	ShapeLayer(const ShapeLayer &other);
	
	bool setProperty(const QVariant &data, int role) override;
	QVariant property(int role) const override;
	
	void updateThumbnail(const QSize &size);
	
	virtual QPainterPath shape() const = 0;
	
	StrokePosition strokePosition() const { return _strokePos; }
	void setStrokePos(StrokePosition pos) { _strokePos = pos; }
	
	double strokeWidth() const { return _strokeWidth; }
	void setStrokeWidth(double w) { _strokeWidth = w; }
	
	Qt::PenJoinStyle joinStyle() const { return _joinStyle; }
	void setJoinStyle(Qt::PenJoinStyle style) { _joinStyle = style; }
	
	Qt::PenCapStyle capStyle() const { return _capStyle; }
	void setCapStyle(Qt::PenCapStyle style) { _capStyle = style; }
	
	bool isFillEnabled() const { return _fillEnabled; }
	void setFillEnabled(bool enabled) { _fillEnabled = enabled; }
	
	Malachite::Color fillColor() const { return _fillColor; }
	void setFillColor(const Malachite::Color &color) { _fillColor = color; }
	
	bool isStrokeEnabled() const { return _strokeEnabled; }
	void setStrokeEnabled(bool enabled) { _strokeEnabled = enabled; }
	
	Malachite::Color storkeColor() const { return _strokeColor; }
	void setStrokeColor(const Malachite::Color &color) { _strokeColor = color; }
	
	void render(Malachite::Painter *painter) const override;
	
private:
	
	StrokePosition _strokePos = StrokePositionCenter;
	double _strokeWidth = 1.0;
	Qt::PenJoinStyle _joinStyle = Qt::RoundJoin;
	Qt::PenCapStyle _capStyle = Qt::RoundCap;
	
	bool _fillEnabled = true, _strokeEnabled = true;
	Malachite::Color _fillColor, _strokeColor;
};

} // namespace PaintField
