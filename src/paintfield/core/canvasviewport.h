#ifndef PAINTFIELD_CANVASVIEWPORT_H
#define PAINTFIELD_CANVASVIEWPORT_H

#include "global.h"
#include <QWidget>

class QAbstractSlider;

namespace Malachite
{
class Affine2D;
class Image;
}

namespace PaintField {

class CanvasViewport : public QWidget
{
	Q_OBJECT
public:
	explicit CanvasViewport(QWidget *parent = 0);
	~CanvasViewport();
	
	enum UpdateMode
	{
		NoUpdate,
		PartialAccurateUpdate,
		WholeAccurateUpdate,
		FastUpdate
	};
	
	QAbstractSlider *scrollBar(Qt::Orientation orientation);
	
	void setDocumentSize(const QSize &size);
	void setTransform(const Malachite::Affine2D &transformToView, const QPoint &translation, double scale, double rotation, bool retinaMode);
	
	void beforeUpdateTile(UpdateMode mode, int updateTileCount);
	void updateTile(const QPoint &tileKey, const Malachite::Image &image, const QPoint &offset);
	void afterUpdateTile();
	
	void updateWholeAccurately();
	
signals:
	
public slots:
	
protected:
	
	void paintEvent(QPaintEvent *event);
	void resizeEvent(QResizeEvent *);
	
private:
	
	void repaintOutputRect(const QRect &rect);
	void paintRects(const QVector<QRect> &rects, bool considerBorder);
	
	struct Data;
	Data *d;
};

} // namespace PaintField

#endif // PAINTFIELD_CANVASVIEWPORT_H
