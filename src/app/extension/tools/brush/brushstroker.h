#ifndef FSBRUSHSTROKER_H
#define FSBRUSHSTROKER_H

#include "mlsurfacepainter.h"
#include "tabletinputdata.h"
#include "brushsetting.h"

namespace PaintField
{

class Stroker
{
public:
	
	Stroker(MLSurface *surface, const BrushSetting *setting);
	virtual ~Stroker() {}
	
	void moveTo(const TabletInputData &data);
	void lineTo(const TabletInputData &data);
	void end();
	
	QPointSet lastEditedKeys() const { return _lastEditedKeys; }
	QPointSet totalEditedKeys() const { return _totalEditedKeys; }
	
	void clearLastEditedKeys() { _lastEditedKeys.clear(); }
	
	const BrushSetting *setting() const { return _setting; }
	MLVec4F argb() const { return _argb; }
	MLSurface *surface() { return _surface; }
	
protected:
	
	virtual void drawFirst(const TabletInputData &data) = 0;
	virtual void drawInterval(const MLPolygon &polygon, const TabletInputData &dataStart, const TabletInputData &dataEnd) = 0;
	
	void addEditedKeys(const QPointSet &keys);
	
private:
	
	MLSurface *_surface;
	QPointSet _lastEditedKeys, _totalEditedKeys;
	
	int _count;
	TabletInputData  _dataPrev, _dataStart, _dataEnd, _currentData;
	//MLVec2D _v1, v2;
	
	MLVec4F _argb;
	const BrushSetting *_setting;
};

class FSPenStroker : public Stroker
{
public:
	
	//using FSStroker::FSStroker;
	FSPenStroker(MLSurface *surface, const BrushSetting *setting) : Stroker(surface, setting) {}
	
protected:
	
	void drawFirst(const TabletInputData &data);
	void drawInterval(const MLPolygon &polygon, const TabletInputData &dataStart, const TabletInputData &dataEnd);
	
private:
	
	void drawOne(const MLVec2D &pos, double pressure, bool drawQuad);
	
	static MLPolygon calcTangentQuadrangle(double radius1, const MLVec2D &center1, double radius2, const MLVec2D &center2);
	
	MLFixedMultiPolygon _drawnShape;
	double _radiusPrev;
	MLVec2D _posPrev;
};

class FSBrushStroker : public Stroker
{
public:
	
	//using FSStroker::FSStroker;
	FSBrushStroker(MLSurface *surface, const BrushSetting *setting) : Stroker(surface, setting) {}
	
protected:
	
	void drawFirst(const TabletInputData &data);
	void drawInterval(const MLPolygon &polygon, const TabletInputData &dataStart, const TabletInputData &dataEnd);
	
private:
	
	void drawSegment(const MLVec2D &p1, const MLVec2D &p2, double length, TabletInputData &data, double pressureNormalized, double rotationNormalized, double tangentialPressureNormalized, const MLVec2D &tiltNormalized);
	
	void drawDab(const TabletInputData &data);
	MLImage drawDabImage(const TabletInputData &data, QRect *resultRect);
	
	double _carryOver;
	double _lastMinorRadius;
};

}

#endif // FSBRUSHSTROKER_H
