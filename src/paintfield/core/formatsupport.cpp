#include "dialogs/filedialog.h"
#include "dialogs/messagebox.h"
#include "dialogs/formatexportdialog.h"

#include "formatsupport.h"

namespace PaintField {

struct FormatSupport::Data
{
	QString shortDescription;
	QString longDescription;
};

FormatSupport::FormatSupport(QObject *parent) :
	QObject(parent),
	d(new Data)
{
}

FormatSupport::~FormatSupport()
{
	delete d;
}

QString FormatSupport::shortDescription() const
{
	return d->shortDescription;
}

QString FormatSupport::longDescription() const
{
	return d->longDescription;
}

bool FormatSupport::canRead() const
{
	return false;
}

bool FormatSupport::canWrite() const
{
	return false;
}

bool FormatSupport::read(QIODevice *device, QList<LayerRef> *layers, QSize *size)
{
	Q_UNUSED(device);
	Q_UNUSED(layers);
	Q_UNUSED(size);
	return false;
}

bool FormatSupport::write(QIODevice *device, const QList<LayerConstRef> &layers, const QSize &size)
{
	Q_UNUSED(device);
	Q_UNUSED(layers);
	Q_UNUSED(size);
	return false;
}

QWidget *FormatSupport::createExportOptionWidget()
{
	return nullptr;
}

void FormatSupport::setExportOptions(QWidget *widget)
{
	Q_UNUSED(widget);
}

void FormatSupport::setShortDescription(const QString &text)
{
	d->shortDescription = text;
}

void FormatSupport::setLongDescription(const QString &text)
{
	d->longDescription = text;
}

bool FormatSupport::importFromFile(const QString &filepath, const QList<FormatSupport *> &formatSupports, QList<LayerRef> *layers, QSize *size, QString *name)
{
	FormatSupport *importingFormat = nullptr;
	
	QString basename;
	
	{
		QFileInfo fileInfo(filepath);
		
		auto suffix = fileInfo.suffix();
		basename = fileInfo.baseName();
		
		for (auto format : formatSupports)
		{
			if (format->suffixes().contains(suffix))
				importingFormat = format;
		}
		
		if (!importingFormat)
		{
			PAINTFIELD_WARNING << "unknown importing format";
			return false;
		}
	}
	
	QFile file(filepath);
	
	if (!file.open(QIODevice::ReadOnly) || !importingFormat->read(&file, layers, size))
	{
		MessageBox::show(QMessageBox::Warning, tr("Failed to read file."), QString());
		return false;
	}
	
	*name = basename;
	return true;
}

bool FormatSupport::importFromFileDialog(QWidget *parent, const QList<FormatSupport *> &formatSupports, QList<LayerRef> *layers, QSize *size, QString *name, const QString &dialogTitle)
{
	if (formatSupports.size() < 1)
		return false;
	
	QHash<QString, QStringList> filter;
	
	for (auto format : formatSupports)
		filter[format->shortDescription()] = format->suffixes();
	
	auto filepath = FileDialog::getFilePath(parent, dialogTitle, FileDialog::OpenFile, filter);
	if (filepath.isEmpty())
		return true;
	
	return importFromFile(filepath, formatSupports, layers, size, name);
}

bool FormatSupport::exportToFileDialog(QWidget *parent, const QList<FormatSupport *> &formatSupports, const QList<LayerConstRef> &layers, const QSize &size, const QString &dialogTitle, bool showOptions)
{
	if (formatSupports.size() < 1)
		return false;
	
	FormatSupport *exportingFormat;
	
	if (showOptions)
	{
		FormatExportDialog dialog(formatSupports, parent);
		dialog.setWindowTitle(dialogTitle);
		
		if (dialog.exec() != QDialog::Accepted)
			return true;
		
		exportingFormat = dialog.currentFormatSupport();
		exportingFormat->setExportOptions(dialog.currentOptionWidget());
	}
	else
	{
		exportingFormat = formatSupports.first();
	}
	
	QHash<QString, QStringList> filter;
	filter[exportingFormat->shortDescription()] = exportingFormat->suffixes();
	
	auto filepath = FileDialog::getFilePath(parent, dialogTitle, FileDialog::SaveFile, filter);
	
	// cancelled
	if (filepath.isEmpty())
		return true;
	
	{
		QFileInfo fileInfo(filepath);
		QFileInfo dirInfo(fileInfo.dir().path());
		
		if (!dirInfo.isWritable())
		{
			MessageBox::show(QMessageBox::Warning, tr("The specified folder is not writable."), tr("Save in another folder."));
			return false;
		}
	}
	
	QFile file(filepath);
	
	if (!file.open(QIODevice::WriteOnly) || !exportingFormat->write(&file, layers, size))
	{
		MessageBox::show(QMessageBox::Warning, tr("Failed to write file."), QString());
		return false;
	}
	
	return true;
}


} // namespace PaintField
