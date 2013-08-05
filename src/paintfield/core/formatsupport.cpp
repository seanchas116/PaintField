#include "dialogs/filedialog.h"
#include "dialogs/messagebox.h"
#include "dialogs/formatexportdialog.h"
#include "document.h"
#include <QLabel>
#include <QFormLayout>

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

bool FormatSupport::write(QIODevice *device, const QList<LayerConstRef> &layers, const QSize &size, const QVariant &option)
{
	Q_UNUSED(device);
	Q_UNUSED(layers);
	Q_UNUSED(size);
	Q_UNUSED(option);
	return false;
}

QWidget *FormatSupport::createExportingOptionWidget()
{
	auto l = new QFormLayout();
	l->addRow(new QLabel(tr("[None]")));
	
	auto w = new QWidget();
	w->setLayout(l);
	
	return w;
}

QVariant FormatSupport::exportingOptionForWidget(QWidget *widget)
{
	Q_UNUSED(widget);
	return QVariant();
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
	
	{
		QFileInfo fileInfo(filepath);
		
		auto suffix = fileInfo.suffix();
		
		if (name)
			*name = fileInfo.baseName();
		
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
	
	return true;
}

bool FormatSupport::importFromFileDialog(QWidget *parent, const QString &dialogTitle, const QList<FormatSupport *> &formatSupports, QList<LayerRef> *layers, QSize *size, QString *name, QString *path)
{
	if (formatSupports.size() < 1)
		return false;
	
	QHash<QString, QStringList> filter;
	
	for (auto format : formatSupports)
		filter[format->shortDescription()] = format->suffixes();
	
	auto filepath = FileDialog::getFilePath(parent, dialogTitle, FileDialog::OpenFile, filter);
	if (filepath.isEmpty())
		return true;
	
	if (path)
		*path = filepath;
	
	return importFromFile(filepath, formatSupports, layers, size, name);
}

bool FormatSupport::exportToFile(const QString &filepath, FormatSupport *format, const QList<LayerConstRef> &layers, const QSize &size, const QVariant &option)
{
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
	
	if (!file.open(QIODevice::WriteOnly) || !format->write(&file, layers, size, option))
	{
		MessageBox::show(QMessageBox::Warning, tr("Failed to write file."), QString());
		return false;
	}
	
	return true;
}

bool FormatSupport::exportToFileDialog(QWidget *parent, const QString &dialogTitle, bool showOptions, const QList<FormatSupport *> &formatSupports, const QList<LayerConstRef> &layers, const QSize &size)
{
	// todo: buffered save
	
	if (formatSupports.size() < 1)
		return false;
	
	FormatSupport *exportingFormat;
	QVariant option;
	
	if (showOptions)
	{
		FormatExportDialog dialog(formatSupports, parent);
		dialog.setWindowTitle(dialogTitle);
		
		if (dialog.exec() != QDialog::Accepted)
			return true;
		
		exportingFormat = dialog.currentFormatSupport();
		option = exportingFormat->exportingOptionForWidget(dialog.currentOptionWidget());
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
	
	return exportToFile(filepath, exportingFormat, layers, size, option);
}

} // namespace PaintField
