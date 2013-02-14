#pragma once

#ifdef PAINTFIELD_CANVAS_GL

#include <QGLWidget>
#include "canvasviewportinterface.h"

namespace PaintField {

class CanvasViewportGL : public QGLWidget, public CanvasViewportInterface
{
	Q_OBJECT
	Q_INTERFACES(PaintField::CanvasViewportInterface)
	
public:
	
	CanvasViewportGL(QWidget *parent = 0);
	~CanvasViewportGL();
	
	void setScrollBarValue(Qt::Orientation orientation, int value);
	void setScrollBarRange(Qt::Orientation orientation, int max, int min);
	void setScrollBarPageStep(Qt::Orientation orientation, int value);
	
	void setDocumentSize(const QSize &size);
	void setTransform(const Malachite::Affine2D &transform);
	void updateTile(const QPoint &tileKey, const Malachite::Image &image, const QPoint &offset);
	void beforeUpdateTile();
	void afterUpdateTile();
	void update() { updateGL(); }
	bool isReady() { return false; }
	
signals:
	
	void ready();
	
protected:
	
	void initializeGL();
	void resizeGL(int w, int h);
	void paintGL();
	
private:
	
	class Data;
	Data *d;
};

} // namespace PaintField

#endif

