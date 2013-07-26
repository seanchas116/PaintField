#pragma once

#include "brushstroker.h"

namespace PaintField {

struct BrushScanline;

class BrushStrokerSimpleBrush : public BrushStroker
{
public:
	BrushStrokerSimpleBrush(Malachite::Surface *surface);
	
	void loadSettings(const QVariantMap &settings);
	
protected:
	
	void drawFirst(const TabletInputData &data);
	void drawInterval(const Malachite::Polygon &polygon, const TabletInputData &dataStart, const TabletInputData &dataEnd);
	
private:
	
	double drawSegment(const Malachite::Vec2D &p1, const Malachite::Vec2D &p2, double len, double &pressure, double pressureNormalized, double carryOver);
	QRect drawDab(const Malachite::Vec2D &pos, double pressure);
	
	void drawScanline(const BrushScanline &scanline, Malachite::Surface *surface);
	
	Malachite::Image *getTile(const QPoint &key, Malachite::Surface *surface);
	
	double _carryOver = 1.0;
	Malachite::Image *_lastTile = 0;
	QPoint _lastKey;
	QScopedArrayPointer<Malachite::PixelVec> _covers;
};

class BrushStrokerSimpleBrushFactory : public BrushStrokerFactory
{
	Q_OBJECT
	
public:
	
	explicit BrushStrokerSimpleBrushFactory(QObject *parent = 0);
	
	QString name() const override;
	QVariantMap defaultSettings() const override;
	BrushStroker *createStroker(Malachite::Surface *surface) override;
	
};

} // namespace PaintField

