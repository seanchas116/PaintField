#pragma once

#include <QObject>
#include "canvas.h"
#include "canvasviewportsurface.h"

/*
#if defined(Q_OS_MAC) && !defined(PF_FORCE_RASTER_ENGINE)
#define PF_CANVAS_VIEWPORT_COCOA
#endif
*/

class QPoint;
class QSize;
class QRect;

namespace Malachite
{
class Affine2D;
class Image;
}

namespace PaintField {

class CanvasViewportController : public QObject
{
	Q_OBJECT
public:
	explicit CanvasViewportController(QObject *parent = 0);
	~CanvasViewportController();
	
	void beginUpdateTile(int tileCount);
	void updateTile(const QPoint &tileKey, const Malachite::Image &image, const QPoint &offset);
	void endUpdateTile();
	
	QWidget *viewport();
	
	void placeViewport(QWidget *window);
	void moveViewport(const QRect &rect, bool visible);

	CanvasViewportSurface mergedSurface() const;
	
signals:
	
	void viewSizeChanged(const QSize &size);
	
public slots:
	
	void setTransforms(const Ref<const CanvasTransforms> &transforms);
	//void setTransform(const Malachite::Affine2D &toScene, const Malachite::Affine2D &fromScene);
	void setRetinaMode(bool mode);
	void setDocumentSize(const QSize &size);
	void update();
	
	void deleteViewportLater();
	
private:
	
	struct Data;
	Data *d;
};

} // namespace PaintField
