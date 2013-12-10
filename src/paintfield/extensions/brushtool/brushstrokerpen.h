#pragma once

#include "brushstroker.h"
#include <Malachite/BlendMode>

namespace PaintField {

class BrushStrokerPen : public BrushStroker
{
public:
	struct Settings
	{
		bool eraser = false;
	};
	
	BrushStrokerPen(Malachite::Surface *surface);
	
	void loadSettings(const QVariantMap &settings) override;
	
protected:
	void drawFirst(const TabletInputData &data) override;
	void drawInterval(
		const Malachite::Polygon &polygon, const QVector<double> &polygonLengths, double totalLength,
		const TabletInputData &dataStart, const TabletInputData &dataEnd) override;
	
private:
	void drawShape(const Malachite::FixedMultiPolygon &shape);
	
	QHash<QPoint, Malachite::FixedMultiPolygon> _drawnShapes;
	Settings _settings;
};

class BrushStrokerPenFactory : public BrushStrokerFactory
{
	Q_OBJECT
	
public:
	explicit BrushStrokerPenFactory(QObject *parent = 0) : BrushStrokerFactory(parent) {}
	
	QString name() const override { return "paintfield.brush.pen"; }
	QString title() const override;
	QVariantMap defaultSettings() const override;
	BrushStroker *createStroker(Malachite::Surface *surface) override;
};

}

