#pragma once

#include <QWidget>

namespace PaintField {

class Canvas;
class CanvasViewport;

class CanvasView : public QWidget
{
	Q_OBJECT
public:
	explicit CanvasView(Canvas *canvas, QWidget *parent = 0);
	~CanvasView();

	Canvas *canvas();
	CanvasViewport *viewport();

signals:

public slots:

protected:

	void resizeEvent(QResizeEvent *);
	void changeEvent(QEvent *ev);
	void showEvent(QShowEvent *);
	void hideEvent(QHideEvent *);

private:

	struct Data;
	QScopedPointer<Data> d;
};

} // namespace PaintField
