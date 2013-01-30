#pragma once
#include <QGLWidget>
#include "abstractcanvasviewportcontroller.h"

namespace PaintField {

class CanvasViewportGL : public QGLWidget
{
	Q_OBJECT
	
public:
	
	CanvasViewportGL(const QGLFormat &format, QWidget *parent = 0);
	~CanvasViewportGL();
	
	void setDocumentSize(const QSize &size);
	void setTransform(const Malachite::Affine2D &transform);
	void updateTile(const QPoint &tileKey, const Malachite::Image &image, const QPoint &offset);
	void beforeUpdateTile();
	void afterUpdateTile();
	
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

class CanvasViewportControllerGL : public AbstractCanvasViewportController
{
	Q_OBJECT
public:
	explicit CanvasViewportControllerGL(QObject *parent = 0);
	~CanvasViewportControllerGL();
	
	QWidget *view() override;
	
	void setDocumentSize(const QSize &size) override;
	void setTransform(const Malachite::Affine2D &transform) override;
	void updateTile(const QPoint &tileKey, const Malachite::Image &image, const QPoint &offset) override;
	void beforeUpdateTile();
	void afterUpdateTile();
	void update();
	bool isReady() { return false; }
	
private:
	
	class Data;
	Data *d;
};

} // namespace PaintField

