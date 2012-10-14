#include <QtGui>

#include "core/application.h"
#include "core/workspacecontroller.h"

#include "extension/layeractioncontroller.h"

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
		connect(workspace, SIGNAL(canvasAdded(CanvasController*)), this, SLOT(onCanvasAdded(CanvasController*)));
	}
	
	void onCanvasAdded(CanvasController *canvas)
	{
		new LayerActionController(canvas);
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
