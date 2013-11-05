#include <QtPlugin>

#include "paintfield/core/appcontroller.h"
#include "paintfield/core/workspace.h"
#include "paintfield/core/settingsmanager.h"
#include "paintfield/core/widgets/simplebutton.h"
#include "paintfield/core/palettemanager.h"
#include "paintfield/core/observablevariantmap.h"

#include "brushpresetitem.h"
#include "brushpresetdatabase.h"
#include "brushsidebar.h"
#include "brushstrokerfactorymanager.h"
#include "brushpresetmanager.h"
#include "brushlibraryview.h"
#include "brushtool.h"
#include "brushstrokerpen.h"
#include "brushstrokersimplebrush.h"
#include "brusheditorview.h"

#include "brushtoolextension.h"

namespace PaintField {

const QString brushToolName("paintfield.tool.brush");
const QString brushLibrarySidebarName("paintfield.sidebar.brushLibrary");
const QString brushSideBarName("paintfield.sidebar.brush");
const QString brushEditorSideBarName("paintfield.sidebar.brushEditor");

BrushToolExtension::BrushToolExtension(BrushPresetDatabase *presetDatabase, Workspace *workspace, QObject *parent) :
    WorkspaceExtension(workspace, parent),
	mPresetManager(new BrushPresetManager(this)),
	mStrokerFactoryManager(new BrushStrokerFactoryManager(this))
{
	mStrokerFactoryManager->addFactory(new BrushStrokerPenFactory);
	mStrokerFactoryManager->addFactory(new BrushStrokerSimpleBrushFactory);

	{
		connect(presetDatabase, &BrushPresetDatabase::currentPresetChanged, mPresetManager, &BrushPresetManager::setPreset);
		mPresetManager->setPreset(presetDatabase->currentPreset());

		addSideBar(brushLibrarySidebarName, new BrushLibraryView(presetDatabase));
	}
	{
		// brush sidebar
		auto brushSideBar = new BrushSideBar(mPresetManager);
		addSideBar(brushSideBarName, brushSideBar);
	}

	{
		// brush editor
		auto editorView = new BrushEditorView(mStrokerFactoryManager, mPresetManager);
		addSideBar(brushEditorSideBarName, editorView);
	}
}

Tool *BrushToolExtension::createTool(const QString &name, Canvas *parent)
{
	if (name == brushToolName)
	{
		auto tool = new BrushTool(parent);
		
		connect(workspace()->paletteManager(), SIGNAL(currentColorChanged(Malachite::Color)), tool, SLOT(setColor(Malachite::Color)));
		tool->setColor(workspace()->paletteManager()->currentColor());

		connect(mPresetManager, &BrushPresetManager::strokerChanged, tool, [this, tool](const QString &name) {
			tool->setStrokerFactory(mStrokerFactoryManager->factory(name));
		});

		connect(mPresetManager->parameters(), SIGNAL(mapChanged(QVariantMap)), tool, SLOT(setBrushSettings(QVariantMap)));
		
		tool->setStrokerFactory(mStrokerFactoryManager->factory(mPresetManager->stroker()));
		tool->setBrushSettings(mPresetManager->parameters()->map());

		using namespace std::placeholders;
		mPresetManager->parameters()->onValueChanged<int>("size", tool, std::bind(&BrushTool::setBrushSize, tool, _1));
		mPresetManager->commonParameters()->onValueChanged<bool>("smooth", tool, std::bind(&BrushTool::setSmoothEnabled, tool, _1));
		tool->setBrushSize(mPresetManager->parameters()->value("size").toInt());
		tool->setSmoothEnabled(mPresetManager->commonParameters()->value("smooth").toBool());
		
		return tool;
	}
	return 0;
}

void BrushToolExtensionFactory::initialize(AppController *app)
{
	{
		QString text = QObject::tr("Brush");
		QIcon icon = SimpleButton::createIcon(":/icons/24x24/brush.svg");
		//QStringList supportedTypes = { "raster" };
		app->settingsManager()->declareTool(brushToolName, ToolInfo(text, icon, QStringList()));
	}
	
	{
		app->settingsManager()->declareSideBar(brushLibrarySidebarName, SideBarInfo(tr("Brush Library")));
		app->settingsManager()->declareSideBar(brushSideBarName, SideBarInfo(tr("Brush")));
		app->settingsManager()->declareSideBar(brushEditorSideBarName, SideBarInfo(tr("Brush Editor")));
	}

	{
		auto presetDatabase = new BrushPresetDatabase(this);

		QDir userSettingsDir = appController()->settingsManager()->userDataDir();
		userSettingsDir.cd("Settings");
		QFileInfo fileInfo(userSettingsDir.filePath("brush-presets.json"));
		mPresetFilePath = fileInfo.filePath();
		if (fileInfo.exists()) {
			presetDatabase->load(fileInfo.filePath());
		} else {
			QDir builtinSettingsDir = appController()->settingsManager()->builtinDataDir();

			if (builtinSettingsDir.cd("Settings")) {
				presetDatabase->load(builtinSettingsDir.filePath("brush-presets.json"));
			}
		}
		mPresetDatabase = presetDatabase;
	}
}

BrushToolExtensionFactory::~BrushToolExtensionFactory()
{
	mPresetDatabase->save(mPresetFilePath);
}

WorkspaceExtensionList BrushToolExtensionFactory::createWorkspaceExtensions(Workspace *workspace, QObject *parent)
{
	return { new BrushToolExtension(mPresetDatabase, workspace, parent) };
}

}
