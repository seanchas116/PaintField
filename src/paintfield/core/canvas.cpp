#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QItemSelectionModel>

#include <Malachite/ImageIO>

#include "documentcontroller.h"
#include "smartpointer.h"
#include "appcontroller.h"
#include "toolmanager.h"
#include "documentio.h"
#include "workspace.h"
#include "extension.h"
#include "layerrenderer.h"
#include "extensionmanager.h"
#include "tool.h"
#include "rasterlayer.h"
#include "appcontroller.h"
#include "documentreferencemanager.h"
#include "layeritemmodel.h"

#include "dialogs/messagebox.h"

#include "canvasview.h"

#include "canvas.h"

using namespace Malachite;

namespace PaintField
{

struct Canvas::Data
{
	Workspace *workspace = 0;
	Document *document = 0;
	int *documentRefCount = 0;
	
	CanvasViewController *viewController = 0;
	QActionList actions;
	CanvasExtensionList extensions;
	
	double scale = 1, rotation = 0;
	QPoint translation;
	bool mirrored = false, retinaMode = false;
	
	Affine2D transformToScene, transformToView;
	QSize viewSize;
	QPoint maxAbsTranslation;
	
	double memorizedScale = 1, memorizedRotation = 0;
	QPoint memorizedTranslation;
	
	ScopedQObjectPointer<Tool> tool;
};

Canvas::Canvas(Document *document, Workspace *parent) :
    QObject(parent),
    d(new Data)
{
	d->workspace = parent;
	d->document = document;
	d->documentRefCount = new int;
	*d->documentRefCount = 0;
	
	document->setParent(0);
	
	commonInit();
}

Canvas::Canvas(Canvas *other, Workspace *parent) :
    QObject(parent),
    d(new Data)
{
	d->workspace = parent;
	d->document = other->document();
	d->documentRefCount = other->d->documentRefCount;
	
	d->translation = other->d->translation;
	d->scale = other->d->scale;
	d->rotation = other->d->rotation;
	
	commonInit();
}

void Canvas::commonInit()
{
	appController()->documentReferenceManager()->addCanvas(this);
	
	connect(d->document, SIGNAL(filePathChanged(QString)), this, SIGNAL(documentPropertyChanged()));
	connect(d->document, SIGNAL(modifiedChanged(bool)), this, SIGNAL(documentPropertyChanged()));
	
	// create actions
	
	{
		auto documentController = new DocumentController(d->document, this);
		
		d->actions << Util::createAction("paintfield.file.save", documentController, SLOT(save()));
		d->actions << Util::createAction("paintfield.file.saveAs", documentController, SLOT(saveAs()));
		d->actions << Util::createAction("paintfield.file.export", documentController, SLOT(exportToImage()));
	}
	
	d->actions << Util::createAction("paintfield.file.close", this, SLOT(closeCanvas()));
	d->actions << Util::createAction("paintfield.file.newCanvasIntoDocument", this, SLOT(newCanvasIntoDocument()));
	
	auto undoAction  = d->document->undoStack()->createUndoAction(this);
	undoAction->setObjectName("paintfield.edit.undo");
	d->actions << undoAction;
	
	auto redoAction = d->document->undoStack()->createRedoAction(this);
	redoAction->setObjectName("paintfield.edit.redo");
	d->actions << redoAction;
	
	addExtensions(appController()->extensionManager()->createCanvasExtensions(this, this));
	
	// connect to workspace
	connect(d->workspace->toolManager(), SIGNAL(currentToolChanged(QString)), this, SLOT(onToolChanged(QString)));
	onToolChanged(d->workspace->toolManager()->currentTool());
	
	updateTransform();
}

Canvas::~Canvas()
{
	appController()->documentReferenceManager()->removeCanvas(this);
	delete d;
}

void Canvas::memorizeNavigation()
{
	d->memorizedScale = d->scale;
	d->memorizedRotation = d->rotation;
	d->memorizedTranslation = d->translation;
}

void Canvas::restoreNavigation()
{
	setScale(d->memorizedScale);
	setRotation(d->memorizedRotation);
	setTranslation(d->memorizedTranslation);
}

double Canvas::scale() const
{
	return d->scale;
}

double Canvas::rotation() const
{
	return d->rotation;
}

QPoint Canvas::translation() const
{
	return d->translation;
}

bool Canvas::isMirrored() const
{
	return d->mirrored;
}

bool Canvas::isRetinaMode() const
{
	return d->retinaMode;
}

Affine2D Canvas::transformToScene() const
{
	return d->transformToScene;
}

Affine2D Canvas::transformToView() const
{
	return d->transformToView;
}

QSize Canvas::viewSize() const
{
	return d->viewSize;
}

QPoint Canvas::maxAbsoluteTranslation() const
{
	return d->maxAbsTranslation;
}

void Canvas::setScale(double scale)
{
	if (d->scale != scale)
	{
		d->scale = scale;
		updateTransform();
		emit scaleChanged(scale);
	}
}

void Canvas::setRotation(double rotation)
{
	if (d->rotation != rotation)
	{
		d->rotation = rotation;
		updateTransform();
		emit rotationChanged(rotation);
	}
}

void Canvas::setTranslation(const QPoint &translation)
{
	if (d->translation != translation)
	{
		d->translation = translation;
		updateTransform();
		emit translationChanged(translation);
	}
}

void Canvas::setMirrored(bool mirrored)
{
	if (d->mirrored != mirrored)
	{
		d->mirrored = mirrored;
		updateTransform();
		emit mirroredChanged(mirrored);
	}
}

void Canvas::setRetinaMode(bool mode)
{
	if (d->retinaMode != mode)
	{
		d->retinaMode = mode;
		updateTransform();
		emit retinaModeChanged(mode);
	}
}

void Canvas::setViewSize(const QSize &size)
{
	if (d->viewSize != size)
	{
		d->viewSize = size;
		updateTransform();
	}
}

Workspace *Canvas::workspace()
{
	return d->workspace;
}

Document *Canvas::document()
{
	return d->document;
}

void Canvas::addActions(const QActionList &actions)
{
	d->actions += actions;
}

QActionList Canvas::actions()
{
	return d->actions;
}

CanvasExtensionList Canvas::extensions()
{
	return d->extensions;
}

void Canvas::setViewController(CanvasViewController *controller)
{
	d->viewController = controller;
}

CanvasViewController *Canvas::viewController()
{
	return d->viewController;
}

CanvasView *Canvas::view()
{
	return d->viewController->view();
}

void Canvas::addExtensions(const CanvasExtensionList &extensions)
{
	for (CanvasExtension *extension : extensions)
		addActions(extension->actions());
	d->extensions += extensions;
}

void Canvas::onSetCurrent()
{
	view()->setFocus();
}

Tool *Canvas::tool()
{
	return d->tool.data();
}

void Canvas::onToolChanged(const QString &name)
{
	auto tool = ExtensionUtil::createTool(appController()->extensions(), workspace()->extensions(), extensions(), name, this);
	d->tool.reset(tool);
	emit toolChanged(tool);
}

bool Canvas::closeCanvas()
{
	if (appController()->documentReferenceManager()->tryRemoveCanvas(this))
	{
		emit shouldBeDeleted(this);
		return true;
	}
	return false;
}

void Canvas::newCanvasIntoDocument()
{
	workspace()->addAndShowCanvas(new Canvas(this, d->workspace));
}

void Canvas::updateTransform()
{
	auto sceneSize = d->document->size();
	auto viewSize = d->viewSize;
	
	// set max absolute translation
	{
		int radius = ceil(hypot(sceneSize.width(), sceneSize.height()) * d->scale * 0.5);
		d->maxAbsTranslation = QPoint(radius + viewSize.width(), radius + viewSize.height());
	}
	
	// set transforms
	{
		QPoint sceneOffset = QPoint(sceneSize.width(), sceneSize.height()) / 2;
		QPoint viewOffset = QPoint(viewSize.width(), viewSize.height()) / 2 + d->translation;
		
		double scale = d->retinaMode ? d->scale * 0.5 : d->scale;
		
		auto transformToView = Affine2D::fromTranslation(Vec2D(viewOffset)) *
		                       Affine2D::fromRotationDegrees(d->rotation) *
		                       Affine2D::fromScale(scale) *
		                       Affine2D::fromTranslation(Vec2D(-sceneOffset));
		
		if (d->mirrored)
			transformToView = transformToView * Affine2D(-1, 0, 0, 1, sceneSize.width(), 0);
		
		auto transformToScene = transformToView.inverted();
		
		d->transformToView = transformToView;
		d->transformToScene = transformToScene;
		transformChanged(transformToScene, transformToView);
	}
}

}
