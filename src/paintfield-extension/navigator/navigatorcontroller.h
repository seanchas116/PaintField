#ifndef PAINTFIELD_NAVIGATORCONTROLLER_H
#define PAINTFIELD_NAVIGATORCONTROLLER_H

#include <QObject>
#include "navigatorview.h"

namespace PaintField {

class CanvasController;

class NavigatorController : public QObject
{
	Q_OBJECT
public:
	NavigatorController(CanvasController *canvas, QObject *parent = 0);
	
	NavigatorView *view() { return _view; }
	
signals:
	
public slots:
	
private:
	
	NavigatorView *_view;
};

} // namespace PaintField

#endif // PAINTFIELD_NAVIGATORCONTROLLER_H
