#ifndef PAINTFIELD_NAVIGATORCONTROLLER_H
#define PAINTFIELD_NAVIGATORCONTROLLER_H

#include <QObject>
#include "navigatorview.h"

namespace PaintField {

class CanvasView;

class NavigatorController : public QObject
{
	Q_OBJECT
public:
	NavigatorController(CanvasView *canvasView, QObject *parent = 0);
	
	QWidget *createView();
	
signals:
	
public slots:
	
private:
	CanvasView *_canvasView;
};

} // namespace PaintField

#endif // PAINTFIELD_NAVIGATORCONTROLLER_H
