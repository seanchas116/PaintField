#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QItemSelectionModel>

#include <Malachite/ImageIO>

#include "documentcontroller.h"
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
	
	QPoint maxAbsTranslation;
	
	double memorizedScale = 1, memorizedRotation = 0;
	QPoint memorizedTranslation;
	
	std::shared_ptr<CanvasTransforms> transforms;
	
	QScopedPointer<Tool> tool;
};

Canvas::Canvas(Document *document, Workspace *parent) :
    QObject(parent),
    d(new Data)
{
	d->workspace = parent;
	d->document = document;
	d->documentRefCount = new int;
	*d->documentRefCount = 0;
	
	d->transforms = std::make_shared<CanvasTransforms>();
	
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
	
	d->transforms = std::make_shared<CanvasTransforms>();
	d->transforms->translation = other->d->transforms->translation;
	d->transforms->scale = other->d->transforms->scale;
	d->transforms->rotation = other->d->transforms->rotation;
	
	commonInit();
}

void Canvas::commonInit()
{
	d->transforms->sceneSize = d->document->size();
	
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
	
	updateTransforms();
}

Canvas::~Canvas()
{
	appController()->documentReferenceManager()->removeCanvas(this);
	delete d;
}

void Canvas::memorizeNavigation()
{
	d->memorizedScale = d->transforms->scale;
	d->memorizedRotation = d->transforms->rotation;
	d->memorizedTranslation = d->transforms->translation;
}

void Canvas::restoreNavigation()
{
	setScale(d->memorizedScale);
	setRotation(d->memorizedRotation);
	setTranslation(d->memorizedTranslation);
}

double Canvas::scale() const
{
	return d->transforms->scale;
}

double Canvas::rotation() const
{
	return d->transforms->rotation;
}

QPoint Canvas::translation() const
{
	return d->transforms->translation;
}

bool Canvas::isMirrored() const
{
	return d->transforms->mirrored;
}

bool Canvas::isRetinaMode() const
{
	return d->transforms->retinaMode;
}

std::shared_ptr<const CanvasTransforms> Canvas::transforms() const
{
	return d->transforms;
}

QSize Canvas::viewSize() const
{
	return d->transforms->viewSize;
}

QPoint Canvas::maxAbsoluteTranslation() const
{
	return d->maxAbsTranslation;
}

void Canvas::setScale(double scale)
{
	if (d->transforms->scale != scale)
	{
		d->transforms->scale = scale;
		updateTransforms();
		emit scaleChanged(scale);
	}
}

void Canvas::setRotation(double rotation)
{
	if (d->transforms->rotation != rotation)
	{
		d->transforms->rotation = rotation;
		updateTransforms();
		emit rotationChanged(rotation);
	}
}

void Canvas::setTranslation(const QPoint &translation)
{
	if (d->transforms->translation != translation)
	{
		d->transforms->translation = translation;
		updateTransforms();
		emit translationChanged(translation);
	}
}

void Canvas::setMirrored(bool mirrored)
{
	if (d->transforms->mirrored != mirrored)
	{
		d->transforms->mirrored = mirrored;
		updateTransforms();
		emit mirroredChanged(mirrored);
	}
}

void Canvas::setRetinaMode(bool mode)
{
	if (d->transforms->retinaMode != mode)
	{
		d->transforms->retinaMode = mode;
		updateTransforms();
		emit retinaModeChanged(mode);
	}
}

void Canvas::setViewSize(const QSize &size)
{
	PAINTFIELD_DEBUG << size;
	
	if (d->transforms->viewSize != size)
	{
		d->transforms->viewSize = size;
		updateTransforms();
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

static void updateCanvasTransforms(const std::shared_ptr<CanvasTransforms> &transforms)
{
	auto sceneSize = transforms->sceneSize;
	auto viewSize = transforms->viewSize;
	
	auto mipmapScale = transforms->scale;
	
	int mipmapLevel = 0;
	// get mipmap level
	while (mipmapScale <= 0.5)
	{
		++mipmapLevel;
		mipmapScale *= 2;
	}
	
	auto mipmapRatio = std::pow(0.5, mipmapLevel);
	auto mipmapSceneSize = QSize(std::round(sceneSize.width() * mipmapRatio), std::round(sceneSize.height() * mipmapRatio));
	
	// set transforms
	{
		auto mipmapSceneOffset = QPoint(mipmapSceneSize.width(), mipmapSceneSize.height()) / 2;
		auto viewOffset = QPoint(viewSize.width(), viewSize.height()) / 2 + transforms->translation;
		
		auto mipmapToView = Affine2D::fromTranslation(Vec2D(viewOffset)) *
		                    Affine2D::fromRotationDegrees(transforms->rotation) *
		                    Affine2D::fromScale(mipmapScale) *
		                    Affine2D::fromTranslation(Vec2D(-mipmapSceneOffset));
		
		if (transforms->mirrored)
			mipmapToView = mipmapToView * Affine2D(-1, 0, 0, 1, mipmapSceneSize.width(), 0);
		
		auto sceneToView = (mipmapLevel != 0) ? mipmapToView * Affine2D::fromScale(mipmapRatio) : mipmapToView;
		
		auto sceneToWindow = (transforms->retinaMode) ? (Affine2D::fromScale(0.5) * sceneToView) : sceneToView;
		
		transforms->viewToScene = sceneToView.inverted().toQTransform();
		transforms->sceneToView = sceneToView.toQTransform();
		
		transforms->windowToScene = sceneToWindow.inverted().toQTransform();
		transforms->sceneToWindow = sceneToWindow.toQTransform();
		
		transforms->viewToMipmap = mipmapToView.inverted().toQTransform();
		transforms->mipmapToView = mipmapToView.toQTransform();
		
		transforms->mipmapScale = mipmapScale;
		transforms->mipmapLevel = mipmapLevel;
		transforms->mipmapSceneSize = mipmapSceneSize;
	}
}

void Canvas::updateTransforms()
{
	updateCanvasTransforms(d->transforms);
	
	// set max absolute translation
	{
		auto sceneSize = d->transforms->sceneSize;
		auto viewSize = d->transforms->viewSize;
		
		int radius = ceil(hypot(sceneSize.width(), sceneSize.height()) * d->transforms->scale * 0.5);
		d->maxAbsTranslation = QPoint(radius + viewSize.width(), radius + viewSize.height());
	}
	
	emit transformsChanged(d->transforms);
}

/*
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
		
		auto transformToView = Affine2D::fromTranslation(Vec2D(viewOffset)) *
		                       Affine2D::fromRotationDegrees(d->rotation) *
		                       Affine2D::fromScale(d->scale) *
		                       Affine2D::fromTranslation(Vec2D(-sceneOffset));
		
		if (d->mirrored)
			transformToView = transformToView * Affine2D(-1, 0, 0, 1, sceneSize.width(), 0);
		
		auto transformToScene = transformToView.inverted();
		
		d->transformToView = transformToView;
		d->transformToScene = transformToScene;
		transformChanged(transformToScene, transformToView);
	}
}*/

}
