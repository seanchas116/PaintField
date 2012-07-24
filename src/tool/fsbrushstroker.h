#ifndef FSBRUSHSTROKER_H
#define FSBRUSHSTROKER_H

#include "mlsurfacepainter.h"
#include "fstabletinputdata.h"

class FSBrushStroker
{
public:
	FSBrushStroker(MLSurface *surface);
	
	QPointSet lastEditedKeys() const { return _lastEditedKeys; }
	QPointSet totalEditedKeys() const { return _totalEditedKeys; }
	
	void moveTo(const FSTabletInputData &data);
	void lineTo(const FSTabletInputData &data);
	void end();
	
private:
	// returns next carry-over
	double drawInterval(const QPolygonF &polygon, const FSTabletInputData &dataStart, const FSTabletInputData &dataEnd, double carryOver);
	void drawDab(const FSTabletInputData &data);
	void drawSimpleDab(const QPointF &pos);
	
	MLSurface *_surface;
	QPointSet _lastEditedKeys, _totalEditedKeys;
	
	FSTabletInputData _dataStart, _dataEnd;
	QPointF _posPrevious;
	int _stackCount;
	double _carryOver;
	
	double _radius;
};

#endif // FSBRUSHSTROKER_H
