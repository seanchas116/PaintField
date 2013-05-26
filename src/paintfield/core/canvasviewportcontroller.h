#pragma once

#include <QObject>
#include "global.h"

class QPoint;
class QSize;

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
	
	void placeViewport(QWidget *window);
	void moveViewport(const QRect &rect, bool visible);
	
signals:
	
	void viewSizeChanged(const QSize &size);
	
public slots:
	
	void setTransform(const Malachite::Affine2D &toScene, const Malachite::Affine2D &fromScene);
	void setRetinaMode(bool mode);
	void setDocumentSize(const QSize &size);
	void update();
	
private:
	
	struct Data;
	Data *d;
};

} // namespace PaintField
