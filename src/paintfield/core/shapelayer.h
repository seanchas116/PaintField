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
	
	QPointSet tileKeys() const override;
	bool includes(const QPoint &pos, int margin) const override;
	bool isGraphicallySelectable() const override;
	
	bool setProperty(const QVariant &data, int role) override;
	QVariant property(int role) const override;
	
	void updateThumbnail(const QSize &size) override;
	
	void encode(QDataStream &stream) const override;
	void decode(QDataStream &stream) override;
	
	void encodeShapeProperties(QDataStream &stream) const;
	void decodeShapeProperties(QDataStream &stream);
	
	QVariantMap saveProperties() const override;
	void loadProperties(const QVariantMap &map) override;
	
	QVariantMap saveShapeProperties() const;
	void loadShapeProperties(const QVariantMap &map);
	
	StrokePosition strokePosition() const { return _strokePos; }
	void setStrokePosition(StrokePosition pos) { _strokePos = pos; updatePaths(); }
	
	QString strokePositionString() const;
	void setStrokePositionString(const QString &string);
	
	double strokeWidth() const { return _strokeWidth; }
	void setStrokeWidth(double w) { _strokeWidth = w; updatePaths(); }
	
	Qt::PenJoinStyle joinStyle() const { return _joinStyle; }
	void setJoinStyle(Qt::PenJoinStyle style) { _joinStyle = style; updatePaths(); }
	
	QString joinStyleString() const;
	void setJoinStyleString(const QString &string);
	
	Qt::PenCapStyle capStyle() const { return _capStyle; }
	void setCapStyle(Qt::PenCapStyle style) { _capStyle = style; updatePaths(); }
	
	QString capStyleString() const;
	void setCapStyleString(const QString &string);
	
	bool isFillEnabled() const { return _fillEnabled; }
	void setFillEnabled(bool enabled) { _fillEnabled = enabled; setThumbnailDirty(true); }
	
	Malachite::Brush fillBrush() const { return _fillBrush; }
	void setFillBrush(const Malachite::Brush &brush) { _fillBrush = brush; setThumbnailDirty(true); }
	
	bool isStrokeEnabled() const { return _strokeEnabled; }
	void setStrokeEnabled(bool enabled) { _strokeEnabled = enabled; setThumbnailDirty(true); }
	
	Malachite::Brush strokeBrush() const { return _strokeBrush; }
	void setStrokeBrush(const Malachite::Brush &brush) { _strokeBrush = brush; setThumbnailDirty(true); }
	
	void render(Malachite::Painter *painter) const override;
	
	QPainterPath fillPath() const { return _fillPath; }
	QPainterPath strokePath() const { return _strokePath; }
	QPainterPath unitedPath() const { return _unitedPath; }
	
	QRectF boundingRect() const;
	
protected:
	
	void setFillPath(const QPainterPath &path);
	
private:
	
	void updatePaths();
	
	StrokePosition _strokePos = StrokePositionCenter;
	double _strokeWidth = 1.0;
	Qt::PenJoinStyle _joinStyle = Qt::MiterJoin;
	Qt::PenCapStyle _capStyle = Qt::SquareCap;
	
	bool _fillEnabled = true, _strokeEnabled = true;
	Malachite::Brush _fillBrush, _strokeBrush;
	
	QPainterPath _fillPath, _strokePath, _unitedPath;
};

} // namespace PaintField
