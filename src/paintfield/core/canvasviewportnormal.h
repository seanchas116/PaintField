#pragma once

#include <QWidget>
#include <Malachite/Surface>

namespace PaintField {

class CanvasViewportNormal : public QWidget
{
	Q_OBJECT
public:
	explicit CanvasViewportNormal(Malachite::SurfaceU8 *surface, QWidget *parent = 0);
	
	void setRepaintImage(const Malachite::ImageU8 &image) { _repaintImage = image; }
	
public slots:
	
	void setTransform(const QTransform &transformToScene, const QTransform &transformToView) { _transformToScene = transformToScene; _transformToView = transformToView; }
	
protected:
	
	void paintEvent(QPaintEvent *event);
	
private:
	
	Malachite::SurfaceU8 *_surface;
	QTransform _transformToView, _transformToScene;
	Malachite::ImageU8 _repaintImage;
};

} // namespace PaintField
