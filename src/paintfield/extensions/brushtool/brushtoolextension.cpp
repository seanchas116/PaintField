#include <QtPlugin>

#include "paintfield/core/appcontroller.h"
#include "paintfield/core/workspace.h"
#include "paintfield/core/settingsmanager.h"
#include "paintfield/core/widgets/simplebutton.h"
#include "paintfield/core/palettemanager.h"

#include "brushpreferencesmanager.h"
#include "brushsidebar.h"
#include "brushstrokerfactorymanager.h"
#include "brushpresetmanager.h"
#include "brushlibrarycontroller.h"
#include "brushtool.h"
#include "brushstrokerpen.h"
#include "brushstrokersimplebrush.h"

#include "brushtoolextension.h"

namespace PaintField {

const QString _brushToolName("paintfield.tool.brush");
const QString _brushLibrarySidebarName("paintfield.sidebar.brushLibrary");
const QString _brushSideBarName("paintfield.sidebar.brush");

BrushToolExtension::BrushToolExtension(Workspace *workspace, QObject *parent) :
    WorkspaceExtension(workspace, parent),
    _presetManager(new BrushPresetManager(this)),
    _strokerFactoryManager(new BrushStrokerFactoryManager(this)),
    _preferencesManager(new BrushPreferencesManager(this))
{
	_strokerFactoryManager->addFactory(new BrushStrokerPenFactory);
	_strokerFactoryManager->addFactory(new BrushStrokerSimpleBrushFactory);
	
	connect(_presetManager, SIGNAL(strokerChanged(QString)), this, SLOT(onStrokerChanged(QString)));
	
	auto libraryController = new BrushLibraryController(_presetManager, this);
	
	connect(libraryController, SIGNAL(currentItemChanged(QStandardItem*,QStandardItem*)), _preferencesManager, SLOT(onCurrentPresetItemChanged(QStandardItem*,QStandardItem*)));
	
	addSideBar(_brushLibrarySidebarName, libraryController->view());
	
	{
		auto brushSideBar = new BrushSideBar;
		
		connect(brushSideBar, SIGNAL(brushSizeChanged(int)), _preferencesManager, SLOT(setBrushSize(int)));
		connect(_preferencesManager, SIGNAL(brushSizeChanged(int)), brushSideBar, SLOT(setBrushSize(int)));
		brushSideBar->setBrushSize(_preferencesManager->brushSize());
		
		connect(brushSideBar, SIGNAL(smoothEnabledChanged(bool)), _preferencesManager, SLOT(setSmoothEnabled(bool)));
		connect(_preferencesManager, SIGNAL(smoothEnabledChanged(bool)), brushSideBar, SLOT(setSmoothEnabled(bool)));
		brushSideBar->setSmoothEnabled(_preferencesManager->isSmoothEnabled());
		
		connect(_presetManager, SIGNAL(metadataChanged(BrushPresetMetadata)), brushSideBar, SLOT(setPresetMetadata(BrushPresetMetadata)));
		brushSideBar->setPresetMetadata(_presetManager->metadata());
		
		addSideBar(_brushSideBarName, brushSideBar);
	}
}

Tool *BrushToolExtension::createTool(const QString &name, Canvas *parent)
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
		connect(_preferencesManager, SIGNAL(smoothEnabledChanged(bool)), tool, SLOT(setSmoothEnabled(bool)));
		tool->setBrushSize(_preferencesManager->brushSize());
		tool->setSmoothEnabled(_preferencesManager->isSmoothEnabled());
		
		return tool;
	}
	return 0;
}

void BrushToolExtension::onStrokerChanged(const QString &name)
{
	PAINTFIELD_DEBUG << "stroker changed";
	emit strokerFactoryChanged(_strokerFactoryManager->factory(name));
}

void BrushToolExtensionFactory::initialize(AppController *app)
{
	{
		QString text = QObject::tr("Brush");
		QIcon icon = SimpleButton::createIcon(":/icons/24x24/brush.svg");
		QStringList supportedTypes = { "raster" };
		app->settingsManager()->declareTool(_brushToolName, ToolInfo(text, icon, QStringList()));
	}
	
	{
		app->settingsManager()->declareSideBar(_brushLibrarySidebarName, SideBarInfo(tr("Brush Library")));
		app->settingsManager()->declareSideBar(_brushSideBarName, SideBarInfo(tr("Brush")));
	}
}

}
