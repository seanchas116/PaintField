#include <QApplication>
#include <QFileInfo>
#include <QClipboard>
#include <Malachite/ImageIO>

#include "layerscene.h"
#include "layerrenderer.h"
#include "documentio.h"
#include "rasterlayer.h"
#include "appcontroller.h"
#include "dialogs/newdocumentdialog.h"
#include "dialogs/filedialog.h"
#include "dialogs/messagebox.h"
#include "dialogs/exportdialog.h"

#include "documentcontroller.h"

using namespace Malachite;

namespace PaintField {

Document *DocumentController::createFromNewDialog()
{
	NewDocumentDialog dialog;
	if (dialog.exec() != QDialog::Accepted)
		return 0;
	
	auto layer = std::make_shared<RasterLayer>(tr("Untitled Layer"));
	return new Document(appController()->unduplicatedFileTempName(tr("Untitled")), dialog.documentSize(), {layer});
}

Document *DocumentController::createFromOpenDialog()
{
	QString filePath = FileDialog::getOpenFilePath(0, tr("Open"), tr("PaintField Document"), {"pfield"});
	
	if (filePath.isEmpty())	// cancelled
		return 0;
	
	return createFromSavedFile(filePath);
}

Document *DocumentController::createFromClipboard()
{
	auto pixmap = qApp->clipboard()->pixmap();
	if (pixmap.isNull())
		return 0;
	
	auto layer = RasterLayer::createFromImage(pixmap.toImage());
	layer->setName(tr("Clipboard"));
	return new Document(appController()->unduplicatedFileTempName(tr("Clipboard")), pixmap.size(), {layer});
}

Document *DocumentController::createFromNewFromImageDialog()
{
	QString filePath = FileDialog::getOpenFilePath(0, tr("Open"), tr("Image File"), ImageImporter::importableExtensions());
	
	if (filePath.isEmpty())
		return 0;
	
	return createFromImageFile(filePath);
}

Document *DocumentController::createFromFile(const QString &path)
{
	QFileInfo fileInfo(path);
	
	if (fileInfo.suffix() == "pfield")
		return createFromSavedFile(path);
	else
		return createFromImageFile(path);
}

Document *DocumentController::createFromSavedFile(const QString &path)
{
	DocumentLoader loader;
	auto document = loader.load(path, 0);
	
	if (!document)
		showMessageBox(QMessageBox::Warning, tr("Failed to open file."), QString());
	
	return document;
}

Document *DocumentController::createFromImageFile(const QString &path)
{
	QString name;
	
	{
		QFileInfo fileInfo(path);
		name = fileInfo.baseName();
	}
	
	QSize size;
	
	auto layer = RasterLayer::createFromImageFile(path, &size);
	if (!layer)
	{
		showMessageBox(QMessageBox::Warning, tr("Failed to open file."), QString());
		return 0;
	}
	
	return new Document(appController()->unduplicatedFileTempName(name), size, {layer});
}

bool DocumentController::confirmClose(Document *document)
{
	if (!document->isModified())
		return true;
	
	int ret = showMessageBox(QMessageBox::NoIcon,
							 tr("Do you want to save your changes on \"%1\"?").arg(document->fileName()),
							 tr("The changes will be lost if you don't save them."),
							 QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
							 QMessageBox::Save);
	
	switch (ret)
	{
		default:
			return false;
		case QMessageBox::Save:
			if (!save(document))
				return false;
			// fall through
		case QMessageBox::Discard:
			return true;
	}
}

bool DocumentController::saveAs(Document *document)
{
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

bool DocumentController::save(Document *document)
{
	if (!document->isModified())
		return true;
	
	if (document->filePath().isEmpty())	// first save
		return saveAs(document);
	
	DocumentSaver saver(document);
	if (!saver.save(document->filePath()))
	{
		showMessageBox(QMessageBox::Warning, tr("Failed to save file."), QString());
		return false;
	}
	return true;
}

bool DocumentController::exportToImage(Document *document)
{
	ExportDialog dialog;
	
	if (!dialog.exec())
		return false;
	
	Surface surface;
	
	{
		LayerRenderer renderer;
		surface = renderer.renderToSurface({document->layerScene()->rootLayer()}, document->tileKeys());
	}
	
	QString path = FileDialog::getSaveFilePath(0, tr("Export"), dialog.currentText(), dialog.currentFormat());
	
	if (path.isEmpty())
		return false;
	
	ImageExporter exporter(dialog.currentFormat());
	exporter.setSurface(surface, document->size());
	exporter.setQuality(dialog.currentQuality());
	
	if (!exporter.save(path))
	{
		showMessageBox(QMessageBox::Warning, tr("Failed to export file."), QString());
		return false;
	}
	
	return true;
}

bool DocumentController::saveAs()
{
	return saveAs(_document);
}

bool DocumentController::save()
{
	return save(_document);
}

bool DocumentController::exportToImage()
{
	return exportToImage(_document);
}

} // namespace PaintField
