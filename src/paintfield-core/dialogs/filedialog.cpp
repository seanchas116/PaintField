#include <QtGui>

#include "../debug.h"
#include "../appcontroller.h"

#include "filedialog.h"

namespace PaintField {

QString FileDialog::getFilePath(QWidget *parent, const QString &title, Mode mode, const QHash<QString, QStringList> &filterTextToSuffixes)
{
	QString lastDialogPath = appController()->lastFileDialogPath();
	
	if (lastDialogPath.isEmpty())
	{
		lastDialogPath = QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation);
	}
	
	QStringList filters;
	
	for (auto iter = filterTextToSuffixes.begin(); iter != filterTextToSuffixes.end(); ++iter)
	{
		QString filter = iter.key();
		
		filter += " (";
		for (auto suffix : iter.value())
		{
			filter = filter + "*." + suffix + " ";
		}
		filter.chop(1);
		filter += ")";
		
		filters << filter;
	}
	
	QFileDialog fileDialog(parent);
	
	fileDialog.setDirectory(lastDialogPath);
	//fileDialog.setOption(QFileDialog::DontUseNativeDialog, true);
	fileDialog.setWindowTitle(title);
	fileDialog.setFilters(filters);
	
	switch (mode)
	{
		default:
		case OpenFile:
			fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
			fileDialog.setFileMode(QFileDialog::ExistingFile);
			break;
		case SaveFile:
			fileDialog.setAcceptMode(QFileDialog::AcceptSave);
			fileDialog.setConfirmOverwrite(true);
			fileDialog.setFileMode(QFileDialog::AnyFile);
			fileDialog.setDefaultSuffix(filterTextToSuffixes.constBegin()->first());
			break;
	}
	
	if (!fileDialog.exec())
		return QString();
	
	if (fileDialog.selectedFiles().size() == 0)
		return QString();
	
	QString result = fileDialog.selectedFiles().first();
	
	if (!result.isEmpty())
		lastDialogPath = fileDialog.directory().path();
	
	appController()->setLastFileDialogPath(lastDialogPath);
	return result;
}

QString FileDialog::getFilePath(QWidget *parent, const QString &title, Mode mode, const QString &filterText, const QStringList &filterSuffixes)
{
	QHash<QString, QStringList> filterHash;
	filterHash[filterText] = filterSuffixes;
	
	return getFilePath(parent, title, mode, filterHash);
}

QString FileDialog::getOpenFilePath(QWidget *parent, const QString &title, const QString &filterText, const QStringList &filterSuffixes)
{
	return getFilePath(parent, title, OpenFile, filterText, filterSuffixes);
}

QString FileDialog::getSaveFilePath(QWidget *parent, const QString &title, const QString &filterText, const QString &filterSuffix)
{
	return getFilePath(parent, title, SaveFile, filterText, {filterSuffix});
}

} // namespace PaintField
