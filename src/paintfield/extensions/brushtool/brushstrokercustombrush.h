#pragma once

#include "brushstroker.h"

namespace PaintField {

class BrushStrokerCustomBrush : public BrushStroker
{
public:
	
	struct Settings
	{
		static Settings fromVariantMap(const QVariantMap &map);
		QVariantMap toVariantMap() const;

		double flattening = 0; // 扁平率
		double rotation = 0;
		double tableWidth = 0, tableHeight = 1;
		double erasing = 0.0;
		double smudge = 0.0;
		double opacity = 1.0;
	};
	
	BrushStrokerCustomBrush(Malachite::Surface *surface) : BrushStroker(surface) {}
	
	void loadSettings(const QVariantMap &settings) override;
	
protected:
	
	void drawFirst(const TabletInputData &data);
	void drawInterval(const Malachite::Polygon &polygon, const TabletInputData &dataStart, const TabletInputData &dataEnd);
	
private:
	
	void drawSegment(const Malachite::Vec2D &p1, const Malachite::Vec2D &p2, double length, TabletInputData &data, double pressureNormalized, double rotationNormalized, double tangentialPressureNormalized, const Malachite::Vec2D &tiltNormalized);
	
	void drawDab(const TabletInputData &data);
	Malachite::Image drawDabImage(const TabletInputData &data, QRect *resultRect);
	
	Settings _setting;
	double _carryOver;
	double _lastMinorRadius;
};

class BrushStrokerCustomBrushFactory : public BrushStrokerFactory
{
	Q_OBJECT
	
public:
	
	explicit BrushStrokerCustomBrushFactory(QObject *parent = 0) : BrushStrokerFactory(parent) {}
	
	QString name() const override { return "paintfield.brush.custom"; }
	
	QVariantMap defaultSettings() const override;
	BrushStroker *createStroker(Malachite::Surface *surface) override;

	BrushEditor *createEditor(const QVariantMap &settings) override;
};

}
