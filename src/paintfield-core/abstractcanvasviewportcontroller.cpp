#include <QtGui>

#include "abstractcanvasviewportcontroller.h"

namespace PaintField
{

struct AbstractCanvasViewportController::Data
{
};

AbstractCanvasViewportController::AbstractCanvasViewportController(QObject *parent) :
    QObject(parent),
    d(new Data)
{
}

AbstractCanvasViewportController::~AbstractCanvasViewportController()
{
	delete d;
}


}
