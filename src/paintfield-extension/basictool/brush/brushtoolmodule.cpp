#include "paintfield-core/appcontroller.h"
#include "paintfield-core/widgets/simplebutton.h"
#include "paintfield-core/debug.h"

#include "brushstrokerfactorymanager.h"
#include "brushpresetmanager.h"
#include "brushlibrarycontroller.h"
#include "brushtool.h"
#include "brushstrokerpen.h"

#include "brushtoolmodule.h"

namespace PaintField {

const QString _brushToolName("paintfield.tool.brush");
const QString _brushLibrarySidebarName("paintfield.sidebar.brushLibrary");

BrushToolModule::BrushToolModule(WorkspaceController *workspace, QObject *parent) :
    WorkspaceModule(workspace, parent),
    _presetManager(new BrushPresetManager(this)),
    _strokerFactoryManager(new BrushStrokerFactoryManager(this))
{
	_strokerFactoryManager->addFactory(new BrushSourcePenFactory);
	connect(_presetManager, SIGNAL(strokerChanged(QString)), this, SLOT(onStrokerChanged(QString)));
	
	auto libraryController = new BrushLibraryController(_presetManager, this);
	
	addSideBar(_brushLibrarySidebarName, libraryController->view());
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
		app->declareTool(_brushToolName, ToolDeclaration(text, icon, supportedTypes));
	}
	
	{
		app->declareSideBar(_brushLibrarySidebarName, SidebarDeclaration(tr("Brush Library")));
	}
}

}
