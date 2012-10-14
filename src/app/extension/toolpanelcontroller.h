#ifndef TOOLPANELCONTROLLER_H
#define TOOLPANELCONTROLLER_H

#include <QObject>
#include "core/panelcontroller.h"

namespace PaintField
{

class ToolPanelController : public PanelController
{
	Q_OBJECT
public:
	explicit ToolPanelController(WorkspaceController *workspace, QObject *parent = 0) :
		PanelController(panel),
		_workspace(workspace)
	{
		setObjectName("paintfield.panel.tool");
	}
	
	QWidget *createView(QWidget *parent);
	
signals:
	
public slots:
	
private:
	
	WorkspaceController *_workspace;
};

}

#endif // TOOLPANELCONTROLLER_H
