#include <QtGui>

#include "mlpainter.h"
#include "mlimageio.h"

#include "newdocumentdialog.h"
#include "exportdialog.h"

#include "../action/actionmodule.h"

#include "../document/rasterlayer.h"
#include "../document/documentmodule.h"

#include "documentmanagercontroller.h"

namespace PaintField
{

DocumentManagerController::DocumentManagerController(QObject *parent) :
    QObject(parent)
{
	ActionManager *actionManager = ActionModule::actionManager();
	
	actionManager->addAction(new QAction(this), "newDocument", this, SLOT(newAndAddDocument()));
	actionManager->addAction(new QAction(this), "openDocument", this, SLOT(openAndAddDocument()));
}

Document *DocumentManagerController::newDocument()
{
	NewDocumentDialog dialog;
	if (dialog.exec() != QDialog::Accepted) {
		return 0;
	}
	
	RasterLayer *layer = new RasterLayer(tr("Untitled Layer"));
	
	return new Document(tr("Untitled"), dialog.documentSize(), layer);
	
}

Document *DocumentManagerController::openDocument()
{
	QString filePath = QFileDialog::getOpenFileName(0,
	                                                tr("Open"),
	                                                QDir::homePath(),
	                                                tr("PaintField Project (*.pfproj)"));
	if (filePath.isEmpty())	// cancelled
		return 0;
	Document *document = Document::open(filePath);
	if (document == 0) {	// failed to open
		QMessageBox::warning(0, QString(), tr("Failed to open file."));
	}
	return document;
}

bool DocumentManagerController::closeDocument(Document *document)
{
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
				if (!saveDocument(document))
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
	
	DocumentModule::documentManager()->removeDocument(document);
	
	return true;
}

bool DocumentManagerController::saveDocument(Document *document)
{
	// first save
	if (document->fileName().isEmpty())
	{
		return saveAsDocument(document);
	}
	
	// file is not modified
	if (!document->isModified())
		return true;
	
	if (!document->save())
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

bool DocumentManagerController::saveAsDocument(Document *document)
{
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
	
	if (!document->saveAs(filePath))
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

bool DocumentManagerController::exportDocument(Document *document)
{
	ExportDialog dialog;
	if (dialog.exec() != QDialog::Accepted)
		return false;	// cancelled
	
	QString filePath = QFileDialog::getSaveFileName(0,
	                                                tr("Export"),
	                                                QDir::homePath(),
	                                                dialog.currentText());
	if (filePath.isEmpty())
		return false;	// cancelled
	
	Malachite::ImageExporter exporter(document->layerModel()->render(), document->size(), dialog.currentFormat());
	
	bool result = exporter.save(filePath, dialog.currentQuality());
	
	if (!result)
		QMessageBox::warning(0, QString(), tr("Failed to export file."));
	
	return result;
}

void DocumentManagerController::newAndAddDocument()
{
	Document *document = newDocument();
	
	if (document)
		DocumentModule::documentManager()->addDocument(document);
}

void DocumentManagerController::openAndAddDocument()
{
	Document *document = openDocument();
	
	if (document)
		DocumentModule::documentManager()->addDocument(document);
}

}

