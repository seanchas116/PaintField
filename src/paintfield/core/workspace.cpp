#include <QApplication>
#include <QClipboard>

#include "util.h"
#include "appcontroller.h"
#include "toolmanager.h"
#include "palettemanager.h"
#include "extension.h"
#include "extensionmanager.h"
#include "canvas.h"
#include "rasterlayer.h"
#include "documentcontroller.h"
#include "colorbuttongroup.h"
#include "workspaceview.h"
#include "serializationutil.h"

#include "workspace.h"

namespace PaintField
{

struct Workspace::Data
{
	QVariantMap state;
	
	QList<Canvas *> canvases;
	QPointer<Canvas> currentCanvas;
	
	ToolManager *toolManager = 0;
	PaletteManager *paletteManager = 0;
	ColorButtonGroup *colorButtonGroup = 0;
	
	QActionList actions;
	WorkspaceExtensionList extensions;
	
	QActionList nullCanvasActions;
	CanvasExtensionList nullCanvasModules;
	
	WorkspaceView *view = 0;
};

Workspace::Workspace(const QVariantMap &state, QObject *parent) :
    QObject(parent),
    d(new Data)
{
	d->state = state;
	
	d->toolManager = new ToolManager(this);
	d->paletteManager = new PaletteManager(this);
	d->colorButtonGroup = new ColorButtonGroup(this);
	
	d->actions << Util::createAction("paintfield.window.splitVertically", this, SIGNAL(splitVerticallyRequested()));
	d->actions << Util::createAction("paintfield.window.splitHorizontally", this, SIGNAL(splitHorizontallyRequested()));
	d->actions << Util::createAction("paintfield.window.closeCurrentSplit", this, SIGNAL(closeCurrentSplitRequested()));
	
	d->actions << Util::createAction("paintfield.file.new", this, SLOT(newCanvas()));
	d->actions << Util::createAction("paintfield.file.open", this, SLOT(openCanvas()));
	d->actions << Util::createAction("paintfield.file.newFromImageFile", this, SLOT(newCanvasFromImageFile()));
	d->actions << Util::createAction("paintfield.file.newFromClipboard", this, SLOT(newCanvasFromClipboard()));
	
	d->actions << Util::createAction("paintfield.window.closeWorkspace", this, SLOT(tryClose()));
	
	addExtensions(appController()->extensionManager()->createWorkspaceExtensions(this, this));
	addNullCanvasExtensions(appController()->extensionManager()->createCanvasExtensions(0, this));
	
	{
		auto paletteColorMaps = state["palette"].toList();
		
		int count = std::min(paletteColorMaps.size(), d->paletteManager->colorCount());
		
		for (int i = 0; i < count; ++i)
			d->paletteManager->setColor(i, SerializationUtil::brushFromMap(paletteColorMaps.at(i).toMap()).color());
	}
}


Workspace::~Workspace()
{
	delete d;
}

ToolManager *Workspace::toolManager()
{
	return d->toolManager;
}

PaletteManager *Workspace::paletteManager()
{
	return d->paletteManager;
}

ColorButtonGroup *Workspace::colorButtonGroup()
{
	return d->colorButtonGroup;
}

void Workspace::setView(WorkspaceView *view)
{
	d->view = view;
}

WorkspaceView *Workspace::view()
{
	return d->view;
}

void Workspace::addExtensions(const QList<WorkspaceExtension *> &extensions)
{
	for (auto extension : extensions) {
		addActions(extension->actions());
		auto state = d->state[extension->objectName()].toMap();
		PAINTFIELD_DEBUG << state;
		extension->loadState(state);
	}
	
	d->extensions += extensions;
}

WorkspaceExtensionList Workspace::extensions()
{
	return d->extensions;
}

void Workspace::addActions(const QActionList &actions)
{
	d->actions += actions;
}

QActionList Workspace::actions()
{
	return d->actions;
}

void Workspace::addNullCanvasExtensions(const CanvasExtensionList &extensions)
{
	for (auto extension : extensions)
		addNullCanvasActions(extension->actions());
	
	d->nullCanvasModules += extensions;
}

CanvasExtensionList Workspace::nullCanvasExtensions()
{
	return d->nullCanvasModules;
}

void Workspace::addNullCanvasActions(const QActionList &actions)
{
	d->nullCanvasActions += actions;
}

QActionList Workspace::nullCanvasActions()
{
	return d->nullCanvasActions;
}

void Workspace::addAndShowDocument(Document *document)
{
	if (document)
		addAndShowCanvas(new Canvas(document, this));
}

void Workspace::newCanvas()
{
	addAndShowDocument(DocumentController::createFromNewDialog(view()));
}

void Workspace::newCanvasFromImageFile()
{
	addAndShowDocument(DocumentController::createFromImportDialog());
}

void Workspace::newCanvasFromClipboard()
{
	addAndShowDocument(DocumentController::createFromClipboard());
}

void Workspace::openCanvas()
{
	openCanvasFromFilepath(DocumentController::getOpenSavedFilePath());
}

void Workspace::openCanvasFromFilepath(const QString &filepath)
{
	if (filepath.isEmpty())
		return;
	
	auto canvas = appController()->findCanvasWithFilepath(filepath);
	if (canvas)
	{
		if (d->canvases.contains(canvas))
		{
			setCurrentCanvas(canvas);
		}
		else
		{
			addAndShowCanvas(new Canvas(canvas, this));
		}
	}
	else
	{
		addAndShowDocument(DocumentController::createFromFile(filepath));
	}
}

bool Workspace::tryClose()
{
	auto canvases = d->canvases;
	
	for (Canvas *canvas : canvases)
	{
		if (!canvas->closeCanvas())
			return false;
	}
	emit shouldBeDeleted(this);
	return true;
}

void Workspace::setFocus()
{
	emit focused();
}

void Workspace::setCurrentCanvas(Canvas *canvas)
{
	if (d->currentCanvas != canvas)
	{
		d->currentCanvas = canvas;
		
		PAINTFIELD_DEBUG << "current canvas changed:" << canvas;
		emit currentCanvasChanged(canvas);
	}
}

void Workspace::addAndShowCanvas(Canvas *canvas)
{
	addCanvas(canvas);
	emit canvasShowRequested(canvas);
	setCurrentCanvas(canvas);
}

void Workspace::addCanvas(Canvas *canvas)
{
	if (!canvas)
		return;
	
	if (!d->canvases.contains(canvas))
	{
		d->canvases << canvas;
		connect(canvas, SIGNAL(shouldBeDeleted(Canvas*)),
		        this, SLOT(deleteCanvas(Canvas*)));
		connect(canvas, SIGNAL(documentPropertyChanged()), this, SLOT(onCanvasDocumentPropertyChanged()));
	}
}

QList<Canvas *> Workspace::canvases()
{
	return d->canvases;
}

Canvas *Workspace::currentCanvas()
{
	return d->currentCanvas;
}

void Workspace::deleteCanvas(Canvas *canvas)
{
	if (d->canvases.contains(canvas))
	{
		if (d->currentCanvas == canvas)
			setCurrentCanvas(0);
		
		d->canvases.removeAll(canvas);
		canvas->deleteLater();
	}
}

QActionList Workspace::currentCanvasActions()
{
	return d->currentCanvas ? d->currentCanvas->actions() : d->nullCanvasActions;
}

CanvasExtensionList Workspace::currentCanvasModules()
{
	return d->currentCanvas ? d->currentCanvas->extensions() : d->nullCanvasModules;
}

QVariantMap Workspace::state() const
{
	return d->state;
}

QVariantMap Workspace::saveState()
{
	QVariantMap state;
	
	if (d->view)
		state = d->view->saveState();
	
	QVariantList paletteColors;
	
	for (auto &color : d->paletteManager->colors()) {
		paletteColors << SerializationUtil::mapFromBrush(color);
	}
	
	state["palette"] = paletteColors;

	for (auto extension : d->extensions) {
		auto extensionState = extension->saveState();
		if (!extensionState.isEmpty()) {
			PAINTFIELD_DEBUG << extension->objectName() << extensionState;
			state[extension->objectName()] = extensionState;
		}
	}
	
	return state;
}

void Workspace::onCanvasDocumentPropertyChanged()
{
	auto canvas = qobject_cast<Canvas *>(sender());
	if (canvas && d->canvases.contains(canvas))
	{
		emit canvasDocumentPropertyChanged(canvas);
		
		if (canvas == d->currentCanvas)
			emit currentCanvasDocumentPropertyChanged();
	}
}

}
