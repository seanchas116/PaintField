#ifndef BRUSHSTROKER_H
#define BRUSHSTROKER_H

#include "brushstroker.h"

namespace PaintField {

class BrushStrokerCustomBrush : public BrushStroker
{
public:
	
	struct Setting
	{
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
	
	Setting _setting;
	double _carryOver;
	double _lastMinorRadius;
};

class BrushSourceBrushFactory : public BrushStrokerFactory
{
	Q_OBJECT
	
public:
	
	explicit BrushSourceBrushFactory(QObject *parent = 0);
	
	QString name() const override { return "paintfield.brush.custom"; }
	
	QVariantMap defaultSettings() const override;
	BrushStroker *createStroker(Malachite::Surface *surface) override;
	
};

}
#endif // BRUSHSTROKER_H
