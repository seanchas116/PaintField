#ifndef FSBRUSHSTROKER_H
#define FSBRUSHSTROKER_H

#include <Malachite/SurfacePainter>
#include "paintfield-core/tabletinputdata.h"
#include <QMutexLocker>

namespace PaintField {

class BrushStroker
{
public:
	
	class SurfaceContext
	{
	public:
		SurfaceContext(BrushStroker *stroker) :
		    _mutexLocker(&stroker->_mutex),
		    _surfacePtr(&stroker->_surface)
		{}
		
		Malachite::Surface *pointer() { return _surfacePtr; }
		
	private:
		
		QMutexLocker _mutexLocker;
		Malachite::Surface *_surfacePtr;
	};
	
	BrushStroker();
	virtual ~BrushStroker() {}
	
	void setSurface(Malachite::Surface surface)
	{
		_surface = surface;
		_originalSurface = surface;
	}
	
	void setRadiusBase(double radius) { _radiusBase = radius; }
	double radiusBase() const { return _radiusBase; }
	
	void setArgb(const Malachite::Vec4F &argb) { _argb = argb; }
	Malachite::Vec4F argb() const { return _argb; }
	
	virtual void loadSettings(const QVariantMap &settings) = 0;
	
	void moveTo(const TabletInput &data);
	void lineTo(const TabletInput &data);
	void end();
	
	// thread-safe functions
	
	QHash<QPoint, QRect> lastEditedKeysWithRects() const { QMutexLocker locker(&_mutex); return _lastEditedKeysWithRects; }
	QPointSet totalEditedKeys() const { QMutexLocker locker(&_mutex); return _totalEditedKeys; }
	
	QHash<QPoint, QRect> getAndClearEditedKeysWithRects();
	void clearLastEditedKeys() { QMutexLocker locker(&_mutex); _lastEditedKeysWithRects.clear(); }
	
	Malachite::Surface surface() const { QMutexLocker locker(&_mutex); return _surface; }
	Malachite::Surface originalSurface() const { return _originalSurface; }
	
	// static functions
	
	static QVector<double> calcLength(const Malachite::Polygon &polygon, double *totalLength);
	
protected:
	
	virtual void drawFirst(const TabletInput &data) = 0;
	virtual void drawInterval(const Malachite::Polygon &polygon, const TabletInput &dataStart, const TabletInput &dataEnd) = 0;
	
	void addEditedKeys(const QHash<QPoint, QRect> &keysWithRects);
	
private:
	
	Malachite::Surface _surface;
	Malachite::Surface _originalSurface;
	
	QPointSet _totalEditedKeys;
	QHash<QPoint, QRect> _lastEditedKeysWithRects;
	
	int _count;
	TabletInput  _dataPrev, _dataStart, _dataEnd, _currentData;
	//MLVec2D _v1, v2;
	
	Malachite::Vec4F _argb;
	double _radiusBase = 10;
	
	mutable QMutex _mutex;
};

class BrushStrokerFactory : public QObject
{
	Q_OBJECT
public:
	explicit BrushStrokerFactory(QObject *parent = 0) : QObject(parent) {}
	
	virtual QString name() const = 0;
	
	virtual QVariantMap defaultSettings() const = 0;
	virtual BrushStroker *createStroker() = 0;
	
signals:
	
public slots:
	
protected:
	
private:
};

}

#endif // FSBRUSHSTROKER_H
