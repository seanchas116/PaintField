#include <QtGui>
#include <Malachite/ImageIO>

#include "appcontroller.h"
#include "toolmanager.h"
#include "documentio.h"
#include "workspacecontroller.h"
#include "module.h"
#include "layerrenderer.h"
#include "modulemanager.h"

#include "dialogs/filedialog.h"
#include "dialogs/messagebox.h"
#include "dialogs/exportdialog.h"
#include "dialogs/newdocumentdialog.h"
#include "canvasview.h"

#include "canvascontroller.h"

using namespace Malachite;

namespace PaintField
{

struct CanvasController::Data
{
	WorkspaceController *workspace = 0;
	Document *document = 0;
	int *documentRefCount = 0;
	
	QItemSelectionModel *selectionModel = 0;
	
	ScopedQObjectPointer<CanvasView> view;
	QActionList actions;
	CanvasModuleList modules;
};

CanvasController::CanvasController(Document *document, WorkspaceController *parent) :
    QObject(parent),
    d(new Data)
{
	d->workspace = parent;
	d->document = document;
	d->documentRefCount = new int(0);
	
	document->setParent(0);
	
	commonInit();
}

CanvasController::CanvasController(CanvasController *other, WorkspaceController *parent) :
    QObject(parent),
    d(new Data)
{
	d->workspace = parent;
	d->document = other->document();
	d->documentRefCount = other->d->documentRefCount;
	
	commonInit();
}

void CanvasController::commonInit()
{
	(*d->documentRefCount)++;
	
	// create selection model
	
	d->selectionModel = new QItemSelectionModel(d->document->layerModel(), this);
	d->selectionModel->setCurrentIndex(d->document->layerModel()->index(0, QModelIndex()), QItemSelectionModel::Current);
	
	// create actions
	
	d->actions << createAction("paintfield.file.save", this, SLOT(saveCanvas()));
	d->actions << createAction("paintfield.file.saveAs", this, SLOT(saveAsCanvas()));
	d->actions << createAction("paintfield.file.close", this, SLOT(closeCanvas()));
	d->actions << createAction("paintfield.file.export", this, SLOT(exportCanvas()));
	
	auto undoAction  = d->document->undoStack()->createUndoAction(this);
	undoAction->setObjectName("paintfield.edit.undo");
	d->actions << undoAction;
	
	auto redoAction = d->document->undoStack()->createRedoAction(this);
	redoAction->setObjectName("paintfield.edit.redo");
	d->actions << redoAction;
	
	d->view.reset(new CanvasView(this, 0));
	
	addModules(appController()->moduleManager()->createCanvasModules(this, this));
	
	setWorkspace(d->workspace);
}

CanvasController::~CanvasController()
{
	(*d->documentRefCount)--;
	
	if (*d->documentRefCount == 0)
	{
		delete d->documentRefCount;
		d->document->deleteLater();
	}
	
	delete d;
}

void CanvasController::setWorkspace(WorkspaceController *workspace)
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

WorkspaceController *CanvasController::workspace()
{
	return d->workspace;
}

Document *CanvasController::document()
{
	return d->document;
}

QItemSelectionModel *CanvasController::selectionModel()
{
	return d->selectionModel;
}

void CanvasController::addActions(const QActionList &actions)
{
	d->actions += actions;
}

QActionList CanvasController::actions()
{
	return d->actions;
}

CanvasModuleList CanvasController::modules()
{
	return d->modules;
}

CanvasView *CanvasController::view()
{
	return d->view.data();
}

void CanvasController::addModules(const CanvasModuleList &modules)
{
	for (CanvasModule *module : modules)
		addActions(module->actions());
	d->modules += modules;
}

void CanvasController::onSetCurrent()
{
	d->view->setFocus();
}

void CanvasController::onToolChanged(const QString &name)
{
	d->view->setTool(createTool(appController()->modules(), workspace()->modules(), modules(), name, d->view.data()));
}

CanvasController *CanvasController::fromNew()
{
	NewDocumentDialog dialog;
	if (dialog.exec() != QDialog::Accepted)
		return 0;
	
	RasterLayer *layer = new RasterLayer(tr("Untitled Layer"));
	
	Document *document = new Document(appController()->unduplicatedNewFileTempName(), dialog.documentSize(), {layer});
	return new CanvasController(document);
}

CanvasController *CanvasController::fromOpen()
{
	QString filePath = FileDialog::getOpenFilePath(0, tr("Open"), tr("PaintField Document"), {"pfield"});
	
	if (filePath.isEmpty())	// cancelled
		return 0;
	
	return fromSavedFile(filePath);
}

CanvasController *CanvasController::fromNewFromImageFile()
{
	QString filePath = FileDialog::getOpenFilePath(0, tr("Open"), tr("Image File"), ImageImporter::importableExtensions());
	
	if (filePath.isEmpty())
		return 0;
	
	return fromImageFile(filePath);
}

CanvasController *CanvasController::fromFile(const QString &path)
{
	QFileInfo fileInfo(path);
	
	if (fileInfo.suffix() == "pfield")
		return fromSavedFile(path);
	else
		return fromImageFile(path);
}

CanvasController *CanvasController::fromSavedFile(const QString &path)
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
	
	return new CanvasController(document);
}

CanvasController *CanvasController::fromImageFile(const QString &path)
{
	QSize size;
	
	auto layer = Layer::createFromImageFile(path, &size);
	if (!layer)
		return 0;
	
	auto document = new Document(appController()->unduplicatedNewFileTempName(), size, {layer});
	return new CanvasController(document);
}

bool CanvasController::saveAsCanvas()
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

bool CanvasController::saveCanvas()
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

bool CanvasController::closeCanvas()
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

bool CanvasController::exportCanvas()
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
	
	ImageExporter exporter(surface, document()->size(), dialog.currentFormat());
	if (!exporter.save(path, dialog.currentQuality()))
	{
		showMessageBox(QMessageBox::Warning, tr("Failed to export file."), QString());
		return false;
	}
	
	return true;
}

}
