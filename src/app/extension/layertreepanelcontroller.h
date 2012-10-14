#ifndef LAYERTREEPANELCONTROLLER_H
#define LAYERTREEPANELCONTROLLER_H

#include <QObject>
#include "core/panelcontroller.h"
#include "core/canvascontroller.h"
#include "layeractioncontroller.h"

namespace PaintField
{

class LayerTreePanelController : public PanelController
{
	Q_OBJECT
public:
	explicit LayerTreePanelController(LayerActionController *actionController, QObject *parent = 0) :
		PanelController(parent),
		_actionController(actionController)
	{
		setObjectName("paintfield.panel.layerTree");
	}
	
	QWidget *createView(QWidget *parent);
	
signals:
	
public slots:
	
private:
	
	LayerActionController *_actionController;
};

}

#endif // LAYERTREEPANELCONTROLLER_H
