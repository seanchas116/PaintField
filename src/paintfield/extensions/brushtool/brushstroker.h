#pragma once

#include <Malachite/SurfacePainter>
#include "paintfield/core/tabletinputdata.h"

namespace PaintField {

class BrushEditor;

class BrushStroker
{
public:
	
	BrushStroker(Malachite::Surface *surface);
	virtual ~BrushStroker() {}
	
	void setRadiusBase(double radius) { _radiusBase = radius; }
	double radiusBase() const { return _radiusBase; }
	
	void setPixel(const Malachite::Pixel &pixel) { _pixel = pixel; }
	Malachite::Pixel pixel() const { return _pixel; }
	
	void setSmoothed(bool enabled) { _smoothed = enabled; }
	bool isSmoothed() const { return _smoothed; }
	
	virtual void loadSettings(const QVariantMap &settings) = 0;
	
	void moveTo(const TabletInputData &data);
	void lineTo(const TabletInputData &data);
	void end();
	
	QHash<QPoint, QRect> lastEditedKeysWithRects() const { return _lastEditedKeysWithRects; }
	QPointSet totalEditedKeys() const { return _totalEditedKeys; }
	
	void clearLastEditedKeys() { _lastEditedKeysWithRects.clear(); }
	
	Malachite::Surface *surface() { return _surface; }
	Malachite::Surface originalSurface() { return _originalSurface; }
	
	static QVector<double> calcLength(const Malachite::Polygon &polygon, double *totalLength);
	
protected:
	
	virtual void drawFirst(const TabletInputData &data) = 0;
	virtual void drawInterval(const Malachite::Polygon &polygon, const TabletInputData &dataStart, const TabletInputData &dataEnd) = 0;
	
	void addEditedKeys(const QHash<QPoint, QRect> &keysWithRects);
	void addEditedKey(const QPoint &key, const QRect &rect);
	void addEditedRect(const QRect &rect);
	
private:

	void filterData(TabletInputData &data);

	Malachite::Surface *_surface = 0;
	Malachite::Surface _originalSurface;
	
	QPointSet _totalEditedKeys;
	QHash<QPoint, QRect> _lastEditedKeysWithRects;
	
	int _count;
	TabletInputData  _dataPrev, _dataStart, _dataEnd, _currentData;
	//MLVec2D _v1, v2;
	
	Malachite::Pixel _pixel;
	double _radiusBase = 10;
	bool _smoothed = false;
	
	Malachite::Polygon _segment;
};

class BrushStrokerFactory : public QObject
{
	Q_OBJECT
public:
	explicit BrushStrokerFactory(QObject *parent = 0) : QObject(parent) {}
	
	virtual QString name() const = 0;
	
	virtual QVariantMap defaultSettings() const = 0;
	virtual BrushStroker *createStroker(Malachite::Surface *surface) = 0;
	virtual BrushEditor *createEditor(const QVariantMap &settings);
	
signals:
	
public slots:
	
protected:
	
private:
};

}
