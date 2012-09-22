#include "core/application.h"
#include "modules/mainwindow/mainwindowmodule.h"

#include "layeruicontroller.h"
#include "layertreewidget.h"

#include "layeruimodule.h"

namespace PaintField
{

LayerUIDocumentModule::LayerUIDocumentModule(QObject *parent) :
    DocumentModuleBase(parent)
{
	
}

void LayerUIDocumentModule::firstInitialize(Document *document)
{
	if (document)
		new LayerUIController(document->layerModel(), this);
	
	_treePanelFrame->setPanel(new LayerTreeWidget(document->layerModel()));
}

LayerUIModule::LayerUIModule(QObject *parent) :
    DocumentApplicationModuleBase(parent)
{
	setTitle("PaintField.LayerUI");
	addDependency(MetaInfo("PaintField.Document"));
}

void LayerUIModule::initialize()
{
	_treePanelFrame = new PanelFrame();
	_treePanelFrame->setObjectName("layerTree");
	MainWindowModule::mainWindow()->addPanel(_treePanelFrame);
}

DocumentModuleBase *LayerUIModule::createDocumentModule()
{
	LayerUIDocumentModule *documentModule = new LayerUIDocumentModule();
	documentModule->_treePanelFrame = _treePanelFrame;
	return documentModule;
}

}
