#ifndef FSBRUSHSTROKER_H
#define FSBRUSHSTROKER_H

#include <Malachite/SurfacePainter>
#include "paintfield-core/tabletinputdata.h"
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
	
	QPointHashToQRect lastEditedKeysWithRects() const { return _lastEditedKeysWithRects; }
	QPointSet totalEditedKeys() const { return _totalEditedKeys; }
	
	void clearLastEditedKeys() { _lastEditedKeysWithRects.clear(); }
	
	const BrushSetting *setting() const { return _setting; }
	Malachite::Vec4F argb() const { return _argb; }
	Malachite::Surface *surface() { return _surface; }
	Malachite::Surface originalSurface() { return _originalSurface; }
	
	static QVector<double> calcLength(const Malachite::Polygon &polygon, double *totalLength);
	
protected:
	
	virtual void drawFirst(const TabletInputData &data) = 0;
	virtual void drawInterval(const Malachite::Polygon &polygon, const TabletInputData &dataStart, const TabletInputData &dataEnd) = 0;
	
	void addEditedKeys(const QPointHashToQRect &keysWithRects);
	
private:
	
	Malachite::Surface *_surface = 0;
	Malachite::Surface _originalSurface;
	
	QPointSet _totalEditedKeys;
	QPointHashToQRect _lastEditedKeysWithRects;
	
	int _count;
	TabletInputData  _dataPrev, _dataStart, _dataEnd, _currentData;
	//MLVec2D _v1, v2;
	
	const BrushSetting *_setting = 0;
	Malachite::Vec4F _argb;
};

}

#endif // FSBRUSHSTROKER_H
