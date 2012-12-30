#ifndef FSBRUSHSTROKER_H
#define FSBRUSHSTROKER_H

#include <Malachite/SurfacePainter>
#include "paintfield-core/tabletinputdata.h"

namespace PaintField {

class BrushStroker
{
public:
	
	BrushStroker(Malachite::Surface *surface);
	virtual ~BrushStroker() {}
	
	void setRadiusBase(double radius) { _radiusBase = radius; }
	double radiusBase() const { return _radiusBase; }
	
	void setArgb(const Malachite::Vec4F &argb) { _argb = argb; }
	Malachite::Vec4F argb() const { return _argb; }
	
	virtual void loadSettings(const QVariantMap &settings) = 0;
	
	void moveTo(const TabletInputData &data);
	void lineTo(const TabletInputData &data);
	void end();
	
	QPointHashToQRect lastEditedKeysWithRects() const { return _lastEditedKeysWithRects; }
	QPointSet totalEditedKeys() const { return _totalEditedKeys; }
	
	void clearLastEditedKeys() { _lastEditedKeysWithRects.clear(); }
	
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
	
	Malachite::Vec4F _argb;
	double _radiusBase = 10;
};

class BrushStrokerFactory : public QObject
{
	Q_OBJECT
public:
	explicit BrushStrokerFactory(QObject *parent = 0) : QObject(parent) {}
	
	virtual QString name() const = 0;
	
	virtual QVariantMap defaultSettings() const = 0;
	virtual BrushStroker *createStroker(Malachite::Surface *surface) = 0;
	
signals:
	
public slots:
	
protected:
	
private:
};

}

#endif // FSBRUSHSTROKER_H
