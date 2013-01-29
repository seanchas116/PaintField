#pragma once

#include <QWidget>
#include "abstractcanvasviewportcontroller.h"

namespace PaintField {

class CanvasViewportSoftware : public QWidget
{
	Q_OBJECT
	
public:
	
	explicit CanvasViewportSoftware(QWidget *parent = 0);
	~CanvasViewportSoftware();
	
	void setDocumentSize(const QSize &size);
	void setTransform(const Malachite::Affine2D &transform);
	
	QTransform transform() const;
	QPixmap *pixmap();
	
protected:
	
	void paintEvent(QPaintEvent *event);
	
private:
	
	class Data;
	Data *d;
};

class CanvasViewportControllerSoftware : public AbstractCanvasViewportController
{
	Q_OBJECT
public:
	explicit CanvasViewportControllerSoftware(QObject *parent = 0);
	~CanvasViewportControllerSoftware();
	
	QWidget *view() override;
	
	void setDocumentSize(const QSize &size) override;
	void setTransform(const Malachite::Affine2D &transform) override;
	void updateTile(const QPoint &tileKey, const Malachite::Image &image, const QPoint &offset) override;
	void update();
	bool isReady() { return true; }
	
signals:
	
public slots:
	
private:
	
	class Data;
	Data *d;
};

}
