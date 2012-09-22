#include "application.h"

#include "canvasmanager.h"

namespace PaintField
{

CanvasManager::CanvasManager(QObject *parent) :
    QObject(parent)
{
	app()->actionManager()->addAction(new QAction(this), "newDocument", this, SLOT(newCanvas()));
	app()->actionManager()->addAction(new QAction(this), "openDocument", this, SLOT(openCanvas()));
}

void CanvasManager::newCanvasFromDocument(Document *document)
{
	addCanvas(new Canvas(document));
}

void CanvasManager::openCanvas()
{
	addCanvas(Canvas::openCanvas());
}

void CanvasManager::newCanvas()
{
	addCanvas(Canvas::newCanvas());
}

void CanvasManager::addCanvas(Canvas *canvas)
{
	canvas->show();
	_canvasList << canvas;
}

}

