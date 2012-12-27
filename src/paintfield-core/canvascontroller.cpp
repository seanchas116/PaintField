#include <QtGui>
#include <Malachite/ImageIO>

#include "appcontroller.h"
#include "toolmanager.h"
#include "documentio.h"
#include "workspacecontroller.h"
#include "module.h"

#include "dialogs/messagebox.h"
#include "dialogs/exportdialog.h"
#include "dialogs/newdocumentdialog.h"
#include "canvasview.h"

#include "canvascontroller.h"

using namespace Malachite;

namespace PaintField
{

CanvasController::CanvasController(Document *document, WorkspaceController *parent) :
    QObject(parent),
	_document(document)
{
	_document->setParent(0);
	commonInit();
}

CanvasController::CanvasController(CanvasController *other, WorkspaceController *parent) :
	QObject(parent),
	_document(other->_document)
{
	commonInit();
}

void CanvasController::commonInit()
{
	_selectionModel = new QItemSelectionModel(_document->layerModel(), this);
	_selectionModel->setCurrentIndex(_document->layerModel()->index(0, QModelIndex()), QItemSelectionModel::Current);
	
	_actions << createAction("paintfield.file.save", this, SLOT(saveCanvas()));
	_actions << createAction("paintfield.file.saveAs", this, SLOT(saveAsCanvas()));
	_actions << createAction("paintfield.file.close", this, SLOT(closeCanvas()));
	
	auto undoAction  = _document->undoStack()->createUndoAction(this);
	undoAction->setObjectName("paintfield.edit.undo");
	_actions << undoAction;
	
	auto redoAction = _document->undoStack()->createRedoAction(this);
	redoAction->setObjectName("paintfield.edit.redo");
	_actions << redoAction;
	
	_view.reset(new CanvasView(this, 0));
	
	connect(workspace()->toolManager(), SIGNAL(currentToolChanged(QString)), this, SLOT(onToolChanged(QString)));
	onToolChanged(workspace()->toolManager()->currentTool());
}

CanvasController::~CanvasController(){}

void CanvasController::addModules(const CanvasModuleList &modules)
{
	for (CanvasModule *module : modules)
		addActions(module->actions());
	_modules += modules;
}

void CanvasController::onSetCurrent()
{
	_view->setFocus();
}

void CanvasController::onToolChanged(const QString &name)
{
	_view->setTool(createTool(appController()->modules(), workspace()->modules(), modules(), name, _view.data()));
}

CanvasController *CanvasController::fromNew(WorkspaceController *parent)
{
	NewDocumentDialog dialog;
	if (dialog.exec() != QDialog::Accepted)
		return 0;
	
	RasterLayer *layer = new RasterLayer(tr("Untitled Layer"));
	
	Document *document = new Document(appController()->unduplicatedNewFileTempName(), dialog.documentSize(), {layer});
	return new CanvasController(document, parent);
}

CanvasController *CanvasController::fromOpen(WorkspaceController *parent)
{
	QString filePath = QFileDialog::getOpenFileName(0,
	                                                tr("Open"),
	                                                QDir::homePath(),
	                                                tr("PaintField Project") + " (*.pfield)");
	if (filePath.isEmpty())	// cancelled
		return 0;
	
	return fromSavedFile(filePath, parent);
}

CanvasController *CanvasController::fromNewFromImageFile(WorkspaceController *parent)
{
	QString filePath = QFileDialog::getOpenFileName(0,
	                                                tr("Open"),
	                                                QDir::homePath(),
	                                                tr("Image File") + " " + fileDialogFilterFromExtensions(ImageImporter::importableExtensions()));
	
	if (filePath.isEmpty())
		return 0;
	
	return fromImageFile(filePath, parent);
}

CanvasController *CanvasController::fromFile(const QString &path, WorkspaceController *parent)
{
	QFileInfo fileInfo(path);
	
	if (fileInfo.suffix() == "pfield")
		return fromSavedFile(path, parent);
	else
		return fromImageFile(path, parent);
}

CanvasController *CanvasController::fromSavedFile(const QString &path, WorkspaceController *parent)
{
	DocumentIO documentIO(path);
	if (!documentIO.openUnzip())
	{
		QMessageBox::warning(0, tr("Failed to open file."), QString());
		return 0;
	}
	
	Document *document = documentIO.load(0);
	
	if (document == 0)
	{	// failed to open
		QMessageBox::warning(0, tr("Failed to open file."), QString());
		return 0;
	}
	
	return new CanvasController(document, parent);
}

CanvasController *CanvasController::fromImageFile(const QString &path, WorkspaceController *parent)
{
	QSize size;
	
	auto layer = Layer::createFromImageFile(path, &size);
	if (!layer)
		return 0;
	
	auto document = new Document(appController()->unduplicatedNewFileTempName(), size, {layer});
	return new CanvasController(document, parent);
}

bool CanvasController::saveAsCanvas()
{
	Document *document = this->document();
	
	QString filePath = QFileDialog::getSaveFileName(0,
	                                                tr("Save As"),
	                                                QDir::homePath(),
													tr("PaintField Project") + " (*.pfield)");
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
		QMessageBox::warning(workspace()->view(), tr("Failed to save the file."), QString());
		return false;
	}
	return true;
}

bool CanvasController::saveCanvas()
{
	Document *document = this->document();
	
	if (document->filePath().isEmpty())	// first save
		return saveAsCanvas();
	
	if (!document->isModified())
		return true;
	
	DocumentIO documentIO(document->filePath());
	if (!documentIO.save(document))
	{
		QMessageBox::warning(workspace()->view(), tr("Cannot save file."), QString());
		return false;
	}
	return true;
}

bool CanvasController::closeCanvas()
{
	if (_document.count() == 1)
	{
		Document *document = this->document();
		
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


}
