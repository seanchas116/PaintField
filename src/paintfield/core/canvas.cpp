#include <QAction>
#include <Malachite/ImageIO>

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

#include "dialogs/filedialog.h"
#include "dialogs/messagebox.h"
#include "dialogs/exportdialog.h"
#include "dialogs/newdocumentdialog.h"
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
	
	QItemSelectionModel *selectionModel = 0;
	
	CanvasView *view = 0;
	QActionList actions;
	CanvasExtensionList extensions;
	
	double scale = 1, rotation = 0;
	QPoint translation;
	bool mirrored = false, retinaMode = false;
	
	double memorizedScale = 1, memorizedRotation = 0;
	QPoint memorizedTranslation;
	
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
	*d->documentRefCount += 1;
	
	connect(d->document, SIGNAL(filePathChanged(QString)), this, SIGNAL(documentPropertyChanged()));
	connect(d->document, SIGNAL(modifiedChanged(bool)), this, SIGNAL(documentPropertyChanged()));
	
	// create selection model
	
	d->selectionModel = new QItemSelectionModel(d->document->layerModel(), this);
	d->selectionModel->setCurrentIndex(d->document->layerModel()->index(0, QModelIndex()), QItemSelectionModel::Current);
	
	// create actions
	
	d->actions << Util::createAction("paintfield.file.save", this, SLOT(saveCanvas()));
	d->actions << Util::createAction("paintfield.file.saveAs", this, SLOT(saveAsCanvas()));
	d->actions << Util::createAction("paintfield.file.close", this, SLOT(closeCanvas()));
	d->actions << Util::createAction("paintfield.file.newCanvasIntoDocument", this, SLOT(newCanvasIntoDocument()));
	d->actions << Util::createAction("paintfield.file.export", this, SLOT(exportCanvas()));
	
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
}

Canvas::~Canvas()
{
	*d->documentRefCount -= 1;
	
	if (*d->documentRefCount == 0)
	{
		delete d->documentRefCount;
		d->document->deleteLater();
	}
	
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

void Canvas::setScale(double scale)
{
	if (d->scale != scale)
	{
		d->scale = scale;
		emit scaleChanged(scale);
	}
}

void Canvas::setRotation(double rotation)
{
	if (d->rotation != rotation)
	{
		d->rotation = rotation;
		emit rotationChanged(rotation);
	}
}

void Canvas::setTranslation(const QPoint &translation)
{
	if (d->translation != translation)
	{
		d->translation = translation;
		emit translationChanged(translation);
	}
}

void Canvas::setMirrored(bool mirrored)
{
	if (d->mirrored != mirrored)
	{
		d->mirrored = mirrored;
		emit mirroredChanged(mirrored);
	}
}

void Canvas::setRetinaMode(bool mode)
{
	if (d->retinaMode != mode)
	{
		d->retinaMode = mode;
		emit retinaModeChanged(mode);
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

QItemSelectionModel *Canvas::selectionModel()
{
	return d->selectionModel;
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

void Canvas::setView(CanvasView *view)
{
	d->view = view;
}

CanvasView *Canvas::view()
{
	return d->view;
}

void Canvas::addExtensions(const CanvasExtensionList &extensions)
{
	for (CanvasExtension *extension : extensions)
		addActions(extension->actions());
	d->extensions += extensions;
}

void Canvas::onSetCurrent()
{
	d->view->setFocus();
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

Canvas *Canvas::fromNew(Workspace *workspace)
{
	NewDocumentDialog dialog;
	if (dialog.exec() != QDialog::Accepted)
		return 0;
	
	auto layer = new RasterLayer(tr("Untitled Layer"));
	
	auto document = new Document(appController()->unduplicatedNewFileTempName(), dialog.documentSize(), {layer});
	return new Canvas(document, workspace);
}

Canvas *Canvas::fromOpen(Workspace *workspace)
{
	QString filePath = FileDialog::getOpenFilePath(0, tr("Open"), tr("PaintField Document"), {"pfield"});
	
	if (filePath.isEmpty())	// cancelled
		return 0;
	
	return fromSavedFile(filePath, workspace);
}

Canvas *Canvas::fromNewFromImageFile(Workspace *workspace)
{
	QString filePath = FileDialog::getOpenFilePath(0, tr("Open"), tr("Image File"), ImageImporter::importableExtensions());
	
	if (filePath.isEmpty())
		return 0;
	
	return fromImageFile(filePath, workspace);
}

Canvas *Canvas::fromFile(const QString &path, Workspace *workspace)
{
	QFileInfo fileInfo(path);
	
	if (fileInfo.suffix() == "pfield")
		return fromSavedFile(path, workspace);
	else
		return fromImageFile(path, workspace);
}

Canvas *Canvas::fromSavedFile(const QString &path, Workspace *workspace)
{
	DocumentLoader loader;
	auto document = loader.load(path, 0);
	
	if (document == 0)
	{
		showMessageBox(QMessageBox::Warning, tr("Failed to open file."), QString());
		return 0;
	}
	
	return new Canvas(document, workspace);
}

Canvas *Canvas::fromImageFile(const QString &path, Workspace *workspace)
{
	QSize size;
	
	auto layer = RasterLayer::createFromImageFile(path, &size);
	if (!layer)
		return 0;
	
	auto document = new Document(appController()->unduplicatedNewFileTempName(), size, {layer});
	return new Canvas(document, workspace);
}

bool Canvas::saveAsCanvas()
{
	Document *document = d->document;
	
	QString filePath = FileDialog::getSaveFilePath(0, tr("Save As"), tr("PaintField Document"), "pfield");
	
	if (filePath.isEmpty())
		return false;
	
	QFileInfo fileInfo(filePath);
	QFileInfo dirInfo(fileInfo.dir().path());
	
	if (!dirInfo.isWritable())
	{
		showMessageBox(QMessageBox::Warning, tr("The specified folder is not writable."), tr("Save in another folder."));
		return false;
	}
	
	DocumentSaver saver(document);
	
	if (!saver.save(filePath))
	{
		showMessageBox(QMessageBox::Warning, tr("Failed to save file."), QString());
		return false;
	}
	return true;
}

bool Canvas::saveCanvas()
{
	Document *document = d->document;
	
	if (document->filePath().isEmpty())	// first save
		return saveAsCanvas();
	
	if (!document->isModified())
		return true;
	
	DocumentSaver saver(document);
	if (!saver.save(document->filePath()))
	{
		showMessageBox(QMessageBox::Warning, tr("Failed to save file."), QString());
		return false;
	}
	return true;
}

bool Canvas::closeCanvas()
{
	if (*d->documentRefCount == 1)
	{
		Document *document = d->document;
		
		if (document->isModified())
		{
			int ret = showMessageBox(QMessageBox::NoIcon,
									 tr("Do you want to save your changes on \"%1\"?").arg(document->fileName()),
									 tr("The changes will be lost if you don't save them."),
									 QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
									 QMessageBox::Save);
			
			switch (ret)
			{
				case QMessageBox::Save:
					if (!saveCanvas())
						return false;
					break;
				case QMessageBox::Discard:
					break;
				case QMessageBox::Cancel:
				default:
					return false;
			}
		}
	}
	
	emit shouldBeDeleted(this);
	return true;
}

void Canvas::newCanvasIntoDocument()
{
	auto newCanvas = new Canvas(this, d->workspace);
	
	workspace()->addAndShowCanvas(newCanvas);
	workspace()->setCurrentCanvas(newCanvas);
}

bool Canvas::exportCanvas()
{
	ExportDialog dialog;
	
	if (!dialog.exec())
		return false;
	
	Surface surface;
	
	{
		LayerRenderer renderer;
		surface = renderer.renderToSurface(layerModel()->rootLayer()->children(), document()->tileKeys());
	}
	
	QString path = FileDialog::getSaveFilePath(0, tr("Export"), dialog.currentText(), dialog.currentFormat());
	
	if (path.isEmpty())
		return false;
	
	ImageExporter exporter(dialog.currentFormat());
	exporter.setSurface(surface, document()->size());
	exporter.setQuality(dialog.currentQuality());
	
	if (!exporter.save(path))
	{
		showMessageBox(QMessageBox::Warning, tr("Failed to export file."), QString());
		return false;
	}
	
	return true;
}

}
