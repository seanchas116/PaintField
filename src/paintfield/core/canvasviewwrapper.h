#ifndef PAINTFIELD_CANVASVIEWWRAPPER_H
#define PAINTFIELD_CANVASVIEWWRAPPER_H

#include <QWidget>

namespace PaintField {

class Canvas;

class CanvasViewWrapper : public QWidget
{
	Q_OBJECT
public:
	explicit CanvasViewWrapper(Canvas *canvas, QWidget *parent = 0);
	~CanvasViewWrapper();
	
signals:
	
public slots:
	
protected:
	
	void resizeEvent(QResizeEvent *);
	void showEvent(QShowEvent *);
	void hideEvent(QHideEvent *);
	void changeEvent(QEvent *event);
	
private:
	
	void moveView();
	
	struct Data;
	Data *d;
};

} // namespace PaintField

#endif // PAINTFIELD_CANVASVIEWWRAPPER_H
