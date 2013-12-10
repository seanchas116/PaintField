#pragma once

#include "stroker.h"
#include <Malachite/SurfacePainter>

namespace PaintField {

class ObservableVariantMap;

class BrushStroker : public Stroker
{
public:
	
	BrushStroker(Malachite::Surface *surface);
	
	void setPixel(const Malachite::Pixel &pixel) { mPixel = pixel; }
	Malachite::Pixel pixel() const { return mPixel; }
	
	Malachite::Surface *surface() { return mSurface; }
	Malachite::Surface originalSurface() { return mOriginalSurface; }
	
	virtual void loadSettings(const QVariantMap &settings) = 0;

private:

	Malachite::Surface *mSurface = 0;
	Malachite::Surface mOriginalSurface;
	
	Malachite::Pixel mPixel;
};

class BrushStrokerFactory : public QObject
{
	Q_OBJECT
public:
	explicit BrushStrokerFactory(QObject *parent = 0) : QObject(parent) {}
	
	/**
	 * @return Brusn name for identification
	 */
	virtual QString name() const = 0;

	/**
	 * @return Human-readable brush name
	 */
	virtual QString title() const = 0;
	
	virtual QVariantMap defaultSettings() const = 0;
	virtual BrushStroker *createStroker(Malachite::Surface *surface) = 0;
	virtual QWidget *createEditor(ObservableVariantMap *parameters);
};

}
