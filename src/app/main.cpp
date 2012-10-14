#include <QtGui>

#include "core/application.h"
#include "core/workspacecontroller.h"

#include "extension/colorpanelcontroller.h"
#include "extension/layertreepanelcontroller.h"
#include "extension/layeruicontroller.h"
#include "extension/toolpanelcontroller.h"

using namespace PaintField;

class Extension : public QObject
{
	Q_OBJECT
	
public:
	
	Extension(QObject *parent = 0) : QObject(parent)
	{
		connect(app()->workspaceManager(), SIGNAL(workspaceAdded(WorkspaceController*)), this, SLOT(onWorkspaceAdded(WorkspaceController*)));
	}
	
private slots:
	
	void onWorkspaceAdded(WorkspaceController *workspace)
	{
		workspace->addPanel(new ColorPanelController(), "paintfield.panel.color");
		workspace->addPanel(new ToolPanelController(), "paintfield.panel.tool");
		connect(workspace, SIGNAL(canvasAdded(CanvasController*)), this, SLOT(onCanvasAdded(CanvasController*)));
	}
	
	void onCanvasAdded(CanvasController *canvas)
	{
		auto actionController = new LayerActionController(canvas);
		canvas->addPanel(new LayerTreePanelController(actionController), "paintfield.panel.layerTree");
	}
	
private:
	
};

int main(int argc, char *argv[])
{
	Application application(argc, argv);
	
	application.loadMenuBarOrderFromJson(":/menubar.json");
	application.loadPanelOrderFromJson(":/panels.json");
	application.loadKeyMapFromJson(":/keymap.json");
	
	application.workspaceManager()->newWorkspace();
	
	return application.exec();
}
