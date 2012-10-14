#ifndef LAYERTREESIDEBARFACTORY_H
#define LAYERTREESIDEBARFACTORY_H

#include "core/sidebarfactory.h"

namespace PaintField
{

class LayerTreeSidebarFactory : public SidebarFactory
{
	Q_OBJECT
public:
	explicit LayerTreeSidebarFactory(QObject *parent = 0);
	
	QWidget *createSidebarForCanvas(CanvasController *canvas, QWidget *parent);
	
signals:
	
public slots:
	
};
	
}

#endif // LAYERTREESIDEBARFACTORY_H
