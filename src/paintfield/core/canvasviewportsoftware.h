#pragma once

#include <QWidget>
#include "canvasviewportinterface.h"

namespace PaintField {

class CanvasViewportSoftware : public QWidget, public CanvasViewportInterface
{
	Q_OBJECT
	Q_INTERFACES(PaintField::CanvasViewportInterface)
	
public:
	
	explicit CanvasViewportSoftware(QWidget *parent = 0);
	~CanvasViewportSoftware();
	
	void setScrollBarValue(Qt::Orientation orientation, int value) override;
	void setScrollBarRange(Qt::Orientation orientation, int max, int min) override;
	void setScrollBarPageStep(Qt::Orientation orientation, int value) override;
	
	void setDocumentSize(const QSize &size) override;
	void setTransform(const Malachite::Affine2D &transform, bool hasTranslation, bool hasScaling, bool hasRotation) override;
	void updateTile(const QPoint &tileKey, const Malachite::Image &image, const QPoint &offset) override;
	void updateAccurately() override;
	void beforeUpdateTile() override {}
	void afterUpdateTile() override;
	bool isReady() override { return true; }
	
signals:
	
	void scrollBarXChanged(int value);
	void scrollBarYChanged(int value);
	
protected:
	
	void paintEvent(QPaintEvent *event);
	void resizeEvent(QResizeEvent *event);
	
private:
	
	void repaintRects(const QVector<QRect> &rects, bool considerBorder);
	
	class Data;
	Data *d;
};


}
