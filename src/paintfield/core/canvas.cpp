#include <QtGui>
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
	
	QScopedPointer<Tool> tool;
};

Canvas::Canvas(Document *document, Workspace *parent) :
    QObject(parent),
    d(new Data)
{
	d->workspace = parent;
	d->document = document;
	d->documentRefCount = new int(0);
	
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
	(*d->documentRefCount)++;
	
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
	
	setWorkspace(d->workspace);
}

Canvas::~Canvas()
{
	(*d->documentRefCount)--;
	
	if (*d->documentRefCount == 0)
	{
		delete d->documentRefCount;
		d->document->deleteLater();
	}
	
	delete d;
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

void Canvas::setWorkspace(Workspace *workspace)
{
	if (d->workspace)
		disconnect(d->workspace->toolManager(), 0, this, 0);
	
	setParent(workspace);
	d->workspace = workspace;
	
	if (workspace)
	{
		connect(workspace->toolManager(), SIGNAL(currentToolChanged(QString)), this, SLOT(onToolChanged(QString)));
		onToolChanged(workspace->toolManager()->currentTool());
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

Canvas *Canvas::fromNew()
{
	NewDocumentDialog dialog;
	if (dialog.exec() != QDialog::Accepted)
		return 0;
	
	RasterLayer *layer = new RasterLayer(tr("Untitled Layer"));
	
	Document *document = new Document(appController()->unduplicatedNewFileTempName(), dialog.documentSize(), {layer});
	return new Canvas(document);
}

Canvas *Canvas::fromOpen()
{
	QString filePath = FileDialog::getOpenFilePath(0, tr("Open"), tr("PaintField Document"), {"pfield"});
	
	if (filePath.isEmpty())	// cancelled
		return 0;
	
	return fromSavedFile(filePath);
}

Canvas *Canvas::fromNewFromImageFile()
{
	QString filePath = FileDialog::getOpenFilePath(0, tr("Open"), tr("Image File"), ImageImporter::importableExtensions());
	
	if (filePath.isEmpty())
		return 0;
	
	return fromImageFile(filePath);
}

Canvas *Canvas::fromFile(const QString &path)
{
	QFileInfo fileInfo(path);
	
	if (fileInfo.suffix() == "pfield")
		return fromSavedFile(path);
	else
		return fromImageFile(path);
}

Canvas *Canvas::fromSavedFile(const QString &path)
{
	DocumentIO documentIO(path);
	if (!documentIO.openUnzip())
	{
		showMessageBox(QMessageBox::Warning, tr("Failed to open file."), QString());
		return 0;
	}
	
	Document *document = documentIO.load(0);
	
	if (document == 0)
	{	// failed to open
		showMessageBox(QMessageBox::Warning, tr("Failed to open file."), QString());
		return 0;
	}
	
	return new Canvas(document);
}

Canvas *Canvas::fromImageFile(const QString &path)
{
	QSize size;
	
	auto layer = Layer::createFromImageFile(path, &size);
	if (!layer)
		return 0;
	
	auto document = new Document(appController()->unduplicatedNewFileTempName(), size, {layer});
	return new Canvas(document);
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
	
	DocumentIO documentIO(filePath);
	
	if (!documentIO.saveAs(document, filePath))
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
	
	DocumentIO documentIO(document->filePath());
	if (!documentIO.save(document))
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
	auto newCanvas = new Canvas(this);
	
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
