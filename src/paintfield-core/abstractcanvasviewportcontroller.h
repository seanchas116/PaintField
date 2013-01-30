#pragma once
#include <QObject>
#include <Malachite/Affine2D>

class QPoint;
class QSize;
class QTransform;

namespace Malachite
{
class Image;
}

namespace PaintField
{

class AbstractCanvasViewportController : public QObject
{
	Q_OBJECT
	
public:
	
	explicit AbstractCanvasViewportController(QObject *parent = 0);
	~AbstractCanvasViewportController();
	
	virtual QWidget *view() = 0;
	
	virtual void setDocumentSize(const QSize &size) = 0;
	virtual void setTransform(const Malachite::Affine2D &transform) = 0;
	virtual void updateTile(const QPoint &tileKey, const Malachite::Image &image, const QPoint &offset) = 0;
	virtual void beforeUpdateTile() {}
	virtual void afterUpdateTile() {}
	virtual void update() = 0;
	virtual bool isReady() = 0;
	
signals:
	
	void ready();
	
private:
	
	class Data;
	Data *d;
};

}

