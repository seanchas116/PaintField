#include <QtGui>

#include "application.h"

#include "documentio.h"

#include "dialogs/exportdialog.h"
#include "dialogs/newdocumentdialog.h"

#include "workspacecontroller.h"

#include "canvascontroller.h"

namespace PaintField
{

CanvasController::CanvasController(Document *document, WorkspaceController *parent) :
    QObject(parent)
{
	_actionManager = new ActionManager(this);
	
	document->setParent(this);
	
	_view.reset(new CanvasView(document));
	
	connect(parent->toolManager(), SIGNAL(currentToolFactoryChanged(ToolFactory*)), _view.data(), SLOT(setToolFactory(ToolFactory*)));
	_view->setToolFactory(parent->toolManager()->currentToolFactory());
	
	_actionManager->addAction("paintfield.file.save", this, SLOT(saveCanvas()));
	_actionManager->addAction("paintfield.file.saveAs", this, SLOT(saveAsCanvas()));
	_actionManager->addAction("paintfield.file.close", this, SLOT(closeCanvas()));
}

CanvasController *CanvasController::fromNew(WorkspaceController *parent)
{
	NewDocumentDialog dialog;
	if (dialog.exec() != QDialog::Accepted) {
		return 0;
	}
	
	RasterLayer *layer = new RasterLayer(tr("Untitled Layer"));
	
	Document *document = new Document(tr("Untitled"), dialog.documentSize(), layer);
	return new CanvasController(document, parent);
}

CanvasController *CanvasController::fromOpen(WorkspaceController *parent)
{
	QString filePath = QFileDialog::getOpenFileName(0,
	                                                tr("Open"),
	                                                QDir::homePath(),
	                                                tr("PaintField Project (*.pfproj)"));
	if (filePath.isEmpty())	// cancelled
		return 0;
	
	DocumentIO documentIO(filePath);
	if (!documentIO.openUnzip())
	{
		QMessageBox::warning(0, QString(), tr("Failed to open file."));
	}
	
	Document *document = documentIO.load(0);
	if (document == 0)
	{	// failed to open
		QMessageBox::warning(0, QString(), tr("Failed to open file."));
	}
	
	return new CanvasController(document, parent);
}

bool CanvasController::saveAsCanvas()
{
	Document *document = this->document();
	
	QString filePath = QFileDialog::getSaveFileName(0,
	                                                tr("Save As"),
	                                                QDir::homePath(),
	                                                tr("PaintField Project (*.pfproj)"));
	if (filePath.isEmpty())
		return false;
	
	QFileInfo fileInfo(filePath);
	QFileInfo dirInfo(fileInfo.dir().path());
	if (!dirInfo.isWritable())
	{
		QMessageBox messageBox;
		messageBox.setIcon(QMessageBox::Warning);
		messageBox.setWindowTitle(QString());
		messageBox.setText(tr("The specified folder is not writable."));
		messageBox.setInformativeText(tr("Save in another folder."));
		messageBox.exec();
		return false;
	}
	
	DocumentIO documentIO(filePath);
	
	if (!documentIO.saveAs(document, filePath))
	{
		QMessageBox messageBox;
		messageBox.setIcon(QMessageBox::Warning);
		messageBox.setWindowTitle(QString());
		messageBox.setText(tr("Failed to save the file."));
		messageBox.exec();
		return false;
	}
	return true;
}

bool CanvasController::saveCanvas()
{
	Document *document = this->document();
	
	// first save
	if (document->fileName().isEmpty())
	{
		return saveAsCanvas();
	}
	
	// file is not modified
	if (!document->isModified())
		return true;
	
	DocumentIO documentIO(document->filePath());
	if (!documentIO.save(document))
	{
		QMessageBox messageBox;
		messageBox.setIcon(QMessageBox::Warning);
		messageBox.setWindowTitle(tr("Error"));
		messageBox.setText(tr("Cannot save file"));
		messageBox.exec();
		return false;
	}
	return true;
}

bool CanvasController::closeCanvas()
{
	Document *document = this->document();
	
	if (document->isModified())
	{
		QMessageBox messageBox;
		messageBox.setText(tr("Do you want to save your changes?"));
		messageBox.setInformativeText(tr("The changes will be lost if you don't save them."));
		messageBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
		messageBox.setDefaultButton(QMessageBox::Save);
		int ret = messageBox.exec();
		
		switch (ret)
		{
			case QMessageBox::Save:
				if (!saveCanvas())
					return false;
				break;
			case QMessageBox::Discard:
				break;
			case QMessageBox::Cancel:
				return false;
			default:
				Q_ASSERT(0);
				break;
		}
	}
	
	return true;
}

}
