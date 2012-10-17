#ifndef FSBRUSHSTROKER_H
#define FSBRUSHSTROKER_H

#include "Malachite/mlsurfacepainter.h"
#include "core/tabletinputdata.h"
#include "brushsetting.h"

namespace PaintField
{

class Stroker
{
public:
	
	Stroker(Malachite::Surface *surface, const BrushSetting *setting, const Malachite::Vec4F &argb);
	virtual ~Stroker() {}
	
	void moveTo(const TabletInputData &data);
	void lineTo(const TabletInputData &data);
	void end();
	
	QPointSet lastEditedKeys() const { return _lastEditedKeys; }
	QPointSet totalEditedKeys() const { return _totalEditedKeys; }
	
	void clearLastEditedKeys() { _lastEditedKeys.clear(); }
	
	const BrushSetting *setting() const { return _setting; }
	Malachite::Vec4F argb() const { return _argb; }
	Malachite::Surface *surface() { return _surface; }
	
protected:
	
	virtual void drawFirst(const TabletInputData &data) = 0;
	virtual void drawInterval(const Malachite::Polygon &polygon, const TabletInputData &dataStart, const TabletInputData &dataEnd) = 0;
	
	void addEditedKeys(const QPointSet &keys);
	
private:
	
	Malachite::Surface *_surface = 0;
	QPointSet _lastEditedKeys, _totalEditedKeys;
	
	int _count;
	TabletInputData  _dataPrev, _dataStart, _dataEnd, _currentData;
	//MLVec2D _v1, v2;
	
	const BrushSetting *_setting = 0;
	Malachite::Vec4F _argb;
};

class FSPenStroker : public Stroker
{
public:
	
	//using FSStroker::FSStroker;
	FSPenStroker(Malachite::Surface *surface, const BrushSetting *setting, const Malachite::Vec4F &argb) : Stroker(surface, setting, argb) {}
	
protected:
	
	void drawFirst(const TabletInputData &data);
	void drawInterval(const Malachite::Polygon &polygon, const TabletInputData &dataStart, const TabletInputData &dataEnd);
	
private:
	
	void drawOne(const Malachite::Vec2D &pos, double pressure, bool drawQuad);
	
	static Malachite::Polygon calcTangentQuadrangle(double radius1, const Malachite::Vec2D &center1, double radius2, const Malachite::Vec2D &center2);
	
	Malachite::FixedMultiPolygon _drawnShape;
	double _radiusPrev;
	Malachite::Vec2D _posPrev;
};

class FSBrushStroker : public Stroker
{
public:
	
	//using FSStroker::FSStroker;
	FSBrushStroker(Malachite::Surface *surface, const BrushSetting *setting, const Malachite::Vec4F &argb) : Stroker(surface, setting, argb) {}
	
protected:
	
	void drawFirst(const TabletInputData &data);
	void drawInterval(const Malachite::Polygon &polygon, const TabletInputData &dataStart, const TabletInputData &dataEnd);
	
private:
	
	void drawSegment(const Malachite::Vec2D &p1, const Malachite::Vec2D &p2, double length, TabletInputData &data, double pressureNormalized, double rotationNormalized, double tangentialPressureNormalized, const Malachite::Vec2D &tiltNormalized);
	
	void drawDab(const TabletInputData &data);
	Malachite::Image drawDabImage(const TabletInputData &data, QRect *resultRect);
	
	double _carryOver;
	double _lastMinorRadius;
};

}

#endif // FSBRUSHSTROKER_H
