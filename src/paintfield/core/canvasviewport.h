#pragma once

#include "global.h"
#include "canvas.h"
#include "canvasviewportsurface.h"
#include <QWidget>

namespace PaintField {

class Canvas;

class CanvasViewport : public QWidget
{
	Q_OBJECT
public:
	explicit CanvasViewport(Canvas *canvas, QWidget *parent = 0);
	~CanvasViewport();

	CanvasViewportSurface mergedSurface() const;

	QSize viewSize() const;

	void setUpdateEnabled(bool x);
	bool isUpdateEnabled() const;

signals:

	void viewSizeChanged(const QSize &size);

protected:

	void resizeEvent(QResizeEvent *) override;
	void enterEvent(QEvent *) override;
	void leaveEvent(QEvent *) override;
	void paintEvent(QPaintEvent *event) override;
	bool event(QEvent *event) override;

private:

	struct Data;
	QScopedPointer<Data> d;

};

} // namespace PaintField
