#ifndef CANVASVIEW_H
#define CANVASVIEW_H

#include <QScrollArea>
#include "tabletevent.h"
#include "document.h"

namespace PaintField
{

class Tool;
class CanvasController;

class CanvasViewViewport : public QWidget
{
	Q_OBJECT
	
public:
	
	CanvasViewViewport(LayerModel *layerModel, QWidget *parent = 0);
	
	QTransform transformToScene() const { return _transformToScene; }
	QTransform transformFromScene() const { return _transformFromScene; }
	
	void setNavigatorTransform(const QTransform &transform);
	QTransform navigatorTransform() const { return _navigatorTransform; }
	
	void setTool(Tool *tool);
	
signals:
	
	void resized(const QSize &size);
	
protected:
	
	void customTabletEvent(WidgetTabletEvent *event);
	void resizeEvent(QResizeEvent *event);
	void paintEvent(QPaintEvent *event);
	
private slots:
	
	void updateTiles(const QPointSet &keys);
	
private:
	
	void updateTransforms();
	
	LayerModel *_layerModel = 0;
	Tool *_tool = 0;
	QPixmap _pixmap;
	QTransform _transformToScene, _transformFromScene, _navigatorTransform;
};

class CanvasView : public QScrollArea
{
	Q_OBJECT
public:
	explicit CanvasView(Document *document, CanvasController *controller, QWidget *parent = 0);
	
	Document *document() { return _document; }
	CanvasController *controller() { return _controller; }
	
signals:
	
public slots:
	
	void setTool(const QString &name);
	
protected:
	
	CanvasViewViewport *_viewport = 0;
	Document *_document = 0;
	CanvasController *_controller = 0;
};

}

#endif // CANVASVIEW_H
