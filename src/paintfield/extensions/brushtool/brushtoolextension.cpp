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
#include "brushlibrarymodel.h"
#include "brushlibraryview.h"
#include "brushtool.h"
#include "brushstrokerpen.h"
#include "brushstrokersimplebrush.h"
#include "brushstrokercustombrush.h"
#include "brusheditorview.h"

#include "brushtoolextension.h"

namespace PaintField {

const QString _brushToolName("paintfield.tool.brush");
const QString _brushLibrarySidebarName("paintfield.sidebar.brushLibrary");
const QString _brushSideBarName("paintfield.sidebar.brush");
const QString _brushEditorSideBarName("paintfield.sidebar.brushEditor");

BrushToolExtension::BrushToolExtension(Workspace *workspace, QObject *parent) :
    WorkspaceExtension(workspace, parent),
    _presetManager(new BrushPresetManager(this)),
    _strokerFactoryManager(new BrushStrokerFactoryManager(this)),
    _preferencesManager(new BrushPreferencesManager(this))
{
	_strokerFactoryManager->addFactory(new BrushStrokerPenFactory);
	_strokerFactoryManager->addFactory(new BrushStrokerSimpleBrushFactory);
	_strokerFactoryManager->addFactory(new BrushStrokerCustomBrushFactory);
	
	connect(_presetManager, SIGNAL(strokerChanged(QString)), this, SLOT(onStrokerChanged(QString)));
	connect(_presetManager, SIGNAL(presetChanged(QVariantMap,QString)), _preferencesManager, SLOT(onPresetChanged(QVariantMap,QString)));

	{
		// library

		auto libraryModel = new BrushLibraryModel(this);

		connect(libraryModel, SIGNAL(currentPathChanged(QString,QString)), _presetManager, SLOT(setPreset(QString)));
		_presetManager->setPreset(libraryModel->currentPath());

		addSideBar(_brushLibrarySidebarName, new BrushLibraryView(libraryModel));
	}
	
	{
		// brush sidebar
		auto brushSideBar = new BrushSideBar(_presetManager, _preferencesManager);
		addSideBar(_brushSideBarName, brushSideBar);
	}

	{
		// brush editor
		auto editorView = new BrushEditorView(_strokerFactoryManager, _presetManager);
		addSideBar(_brushEditorSideBarName, editorView);
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
		app->settingsManager()->declareSideBar(_brushEditorSideBarName, SideBarInfo(tr("Brush Editor")));
	}
}

}
