#ifndef CANVASMANAGER_H
#define CANVASMANAGER_H

#include <QObject>

#include "canvas.h"

namespace PaintField
{

class CanvasManager : public QObject
{
	Q_OBJECT
public:
	explicit CanvasManager(QObject *parent = 0);
	
	void newCanvasFromDocument(Document *document);
	
signals:
	
public slots:
	
	void openCanvas();
	void newCanvas();
	
private:
	
	void addCanvas(Canvas *canvas);
	
	QList<Canvas *> _canvasList;
};

}

#endif // CANVASMANAGER_H
