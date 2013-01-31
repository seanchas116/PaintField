#include "paintfield-core/appcontroller.h"
#include "paintfield-core/workspacecontroller.h"
#include "paintfield-core/settingsmanager.h"
#include "paintfield-core/widgets/simplebutton.h"
#include "paintfield-core/palettemanager.h"

#include "brushpreferencesmanager.h"
#include "brushsidebar.h"
#include "brushstrokerfactorymanager.h"
#include "brushpresetmanager.h"
#include "brushlibrarycontroller.h"
#include "brushtool.h"
#include "brushstrokerpen.h"
#include "brushstrokersimplebrush.h"

#include "brushtoolmodule.h"

namespace PaintField {

const QString _brushToolName("paintfield.tool.brush");
const QString _brushLibrarySidebarName("paintfield.sidebar.brushLibrary");
const QString _brushSideBarName("paintfield.sidebar.brush");

BrushToolModule::BrushToolModule(WorkspaceController *workspace, QObject *parent) :
    WorkspaceModule(workspace, parent),
    _presetManager(new BrushPresetManager(this)),
    _strokerFactoryManager(new BrushStrokerFactoryManager(this)),
    _preferencesManager(new BrushPreferencesManager(this))
{
	_strokerFactoryManager->addFactory(new BrushStrokerPenFactory);
	_strokerFactoryManager->addFactory(new BrushStrokerSimpleBrushFactory);
	
	connect(_presetManager, SIGNAL(strokerChanged(QString)), this, SLOT(onStrokerChanged(QString)));
	
	auto libraryController = new BrushLibraryController(_presetManager, this);
	
	addSideBar(_brushLibrarySidebarName, libraryController->view());
	
	{
		auto brushSideBar = new BrushSideBar;
		connect(brushSideBar, SIGNAL(brushSizeChanged(int)), _preferencesManager, SLOT(setBrushSize(int)));
		connect(_preferencesManager, SIGNAL(brushSizeChanged(int)), brushSideBar, SLOT(setBrushSize(int)));
		connect(_presetManager, SIGNAL(metadataChanged(BrushPresetMetadata)), brushSideBar, SLOT(setPresetMetadata(BrushPresetMetadata)));
		brushSideBar->setBrushSize(_preferencesManager->brushSize());
		brushSideBar->setPresetMetadata(_presetManager->metadata());
		
		addSideBar(_brushSideBarName, brushSideBar);
	}
}

Tool *BrushToolModule::createTool(const QString &name, CanvasView *parent)
{
	if (name == _brushToolName)
	{
		auto tool = new BrushTool(parent);
		
		connect(workspace()->paletteManager(), SIGNAL(currentColorChanged(Malachite::Color)), tool, SLOT(setColor(Malachite::Color)));
		tool->setColor(workspace()->paletteManager()->currentColor());
		
		connect(this, SIGNAL(strokerFactoryChanged(BrushStrokerFactory*)), tool, SLOT(setStrokerFactory(BrushStrokerFactory*)));
		connect(_presetManager, SIGNAL(settingsChanged(QVariantMap)), tool, SLOT(setBrushSettings(QVariantMap)));
		
		tool->setStrokerFactory(_strokerFactoryManager->factory(_presetManager->stroker()));
		tool->setBrushSettings(_presetManager->settings());
		
		connect(_preferencesManager, SIGNAL(brushSizeChanged(int)), tool, SLOT(setBrushSize(int)));
		tool->setBrushSize(_preferencesManager->brushSize());
		
		return tool;
	}
	return 0;
}

void BrushToolModule::onStrokerChanged(const QString &name)
{
	PAINTFIELD_DEBUG << "stroker changed";
	emit strokerFactoryChanged(_strokerFactoryManager->factory(name));
}

void BrushToolModuleFactory::initialize(AppController *app)
{
	{
		QString text = QObject::tr("Brush");
		QIcon icon = SimpleButton::createSimpleIconSet(":/icons/24x24/brush.svg", QSize(24,24));
		QStringList supportedTypes = { "raster" };
		app->settingsManager()->declareTool(_brushToolName, ToolDeclaration(text, icon, supportedTypes));
	}
	
	{
		app->settingsManager()->declareSideBar(_brushLibrarySidebarName, SidebarDeclaration(tr("Brush Library")));
		app->settingsManager()->declareSideBar(_brushSideBarName, SidebarDeclaration(tr("Brush")));
	}
}

}
