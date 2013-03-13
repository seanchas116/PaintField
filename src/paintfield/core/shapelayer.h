#pragma once
#include <Malachite/Color>
#include <Malachite/Brush>
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
	
	void updateThumbnail(const QSize &size) override;
	
	void encode(QDataStream &stream) const override;
	void decode(QDataStream &stream) override;
	
	QVariantMap saveProperies() const override;
	void loadProperties(const QVariantMap &map) override;
	
	virtual QPainterPath shape() const = 0;
		
	StrokePosition strokePosition() const { return _strokePos; }
	void setStrokePos(StrokePosition pos) { _strokePos = pos; }
	
	QString strokePositionString() const;
	void setStrokePositionString(const QString &string);
	
	double strokeWidth() const { return _strokeWidth; }
	void setStrokeWidth(double w) { _strokeWidth = w; }
	
	Qt::PenJoinStyle joinStyle() const { return _joinStyle; }
	void setJoinStyle(Qt::PenJoinStyle style) { _joinStyle = style; }
	
	QString joinStyleString() const;
	void setJoinStyleString(const QString &string);
	
	Qt::PenCapStyle capStyle() const { return _capStyle; }
	void setCapStyle(Qt::PenCapStyle style) { _capStyle = style; }
	
	QString capStyleString() const;
	void setCapStyleString(const QString &string);
	
	bool isFillEnabled() const { return _fillEnabled; }
	void setFillEnabled(bool enabled) { _fillEnabled = enabled; }
	
	Malachite::Brush fillBrush() const { return _fillBrush; }
	void setFillBrush(const Malachite::Brush &brush) { _fillBrush = brush; }
	
	bool isStrokeEnabled() const { return _strokeEnabled; }
	void setStrokeEnabled(bool enabled) { _strokeEnabled = enabled; }
	
	Malachite::Brush strokeBrush() const { return _strokeBrush; }
	void setStrokeBrush(const Malachite::Brush &brush) { _strokeBrush = brush; }
	
	void render(Malachite::Painter *painter) const override;
	
	QPainterPath fillPath() const { return _fillPath; }
	QPainterPath strokePath() const { return _strokePath; }
	
	void updateStrokePath();
	
private:
	
	void setFillPath(const QPainterPath &path);
	
	StrokePosition _strokePos = StrokePositionCenter;
	double _strokeWidth = 1.0;
	Qt::PenJoinStyle _joinStyle = Qt::RoundJoin;
	Qt::PenCapStyle _capStyle = Qt::RoundCap;
	
	bool _fillEnabled = true, _strokeEnabled = true;
	Malachite::Brush _fillBrush, _strokeBrush;
	
	QPainterPath _fillPath, _strokePath;
};

} // namespace PaintField
