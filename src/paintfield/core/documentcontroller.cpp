#include <QApplication>
#include <QFileInfo>
#include <QClipboard>
#include <Malachite/ImageIO>

#include "layerscene.h"
#include "layerrenderer.h"
#include "rasterlayer.h"
#include "appcontroller.h"
#include "formatsupport.h"
#include "formatsupportmanager.h"
#include "document.h"
#include "dialogs/newdocumentdialog.h"
#include "dialogs/filedialog.h"
#include "dialogs/messagebox.h"
#include "dialogs/exportdialog.h"

#include "documentcontroller.h"

using namespace Malachite;

namespace PaintField {

QString DocumentController::getOpenSavedFilePath()
{
	return FileDialog::getOpenFilePath(0, tr("Open"), tr("PaintField Document"), {"pfield"});
}

Document *DocumentController::createFromNewDialog(QWidget *dialogParent)
{
	NewDocumentDialog dialog(dialogParent);
	if (dialog.exec() != QDialog::Accepted)
		return 0;
	
	auto layer = std::make_shared<RasterLayer>(tr("New Layer"));
	return new Document(appController()->unduplicatedFileTempName(tr("Untitled")), dialog.documentSize(), {layer});
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

Document *DocumentController::createFromImportDialog()
{
	QList<LayerRef> layers;
	QSize size;
	QString name;
	
	bool result = FormatSupport::importFromFileDialog(
			0,
			tr("Import"),
			appController()->formatSupportManager()->formatSupports(),
			&layers,
			&size,
			&name
	);
	
	if (result)
		return new Document(appController()->unduplicatedFileTempName(name), size, layers);
	else
		return nullptr;
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
	PAINTFIELD_DEBUG << path;
	
	QList<LayerRef> layers;
	QSize size;
	QString name;
	
	bool result = FormatSupport::importFromFile(
			path,
			{appController()->formatSupportManager()->paintFieldFormatSupport()},
			&layers,
			&size,
			&name
	);
	
	if (!result)
		MessageBox::show(QMessageBox::Warning, tr("Failed to open file."), QString());
	
	auto document = new Document(name, size, layers, 0);
	document->setFilePath(path);
	
	return document;
}

Document *DocumentController::createFromImageFile(const QString &path)
{
	QList<LayerRef> layers;
	QSize size;
	QString name;
	
	bool result = FormatSupport::importFromFile(
			path,
			appController()->formatSupportManager()->formatSupports(),
			&layers,
			&size,
			&name
	);
	
	if (result)
		return new Document(appController()->unduplicatedFileTempName(name), size, layers);
	else
		return nullptr;
}

bool DocumentController::confirmClose(Document *document)
{
	if (!document->isModified())
		return true;
	
	int ret = MessageBox::show(
			QMessageBox::NoIcon,
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
	return FormatSupport::exportToFileDialog(
			0,
			tr("Save As"),
			false,
			{appController()->formatSupportManager()->paintFieldFormatSupport()},
			document->layerScene()->topLevelLayers(),
			document->size()
	);
}

bool DocumentController::save(Document *document)
{
	if (!document->isModified())
		return true;
	
	if (document->filePath().isEmpty())	// first save
		return saveAs(document);
	
	auto result = FormatSupport::exportToFile(
			document->filePath(),
			appController()->formatSupportManager()->paintFieldFormatSupport(),
			document->layerScene()->topLevelLayers(),
			document->size()
	);
	
	if (!result)
	{
		MessageBox::show(QMessageBox::Warning, tr("Failed to save file."), QString());
		return false;
	}
	
	document->setModified(false);
	
	return true;
}

bool DocumentController::exportToImage(Document *document, QWidget *dialogParent)
{
	return FormatSupport::exportToFileDialog(
			dialogParent,
			tr("Export"),
			true,
			appController()->formatSupportManager()->formatSupports(),
			document->layerScene()->topLevelLayers(),
			document->size()
	);
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
	return exportToImage(_document, _dialogParent.data());
}

} // namespace PaintField
