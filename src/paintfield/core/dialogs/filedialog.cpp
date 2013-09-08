#include <QDesktopServices>

#include "../appcontroller.h"
#include "../settingsmanager.h"

#include "filedialog.h"

namespace PaintField {

QString FileDialog::getFilePath(QWidget *parent, const QString &title, Mode mode, const QList<QPair<QString, QStringList>> &filterTextAndSuffixesList)
{
	QString lastDialogPath = appController()->settingsManager()->lastFileDialogPath();
	
	QStringList filters;

	for (const auto &textAndSuffixes : filterTextAndSuffixesList)
	{
		QString filter = textAndSuffixes.first;

		if (textAndSuffixes.second.size())
		{
			filter += " (";
			for (auto suffix : textAndSuffixes.second)
			{
				filter = filter + "*." + suffix + " ";
			}
			filter.chop(1);
			filter += ")";
		}

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
			fileDialog.setDefaultSuffix(filterTextAndSuffixesList.first().second.first());
			break;
	}
	
	if (!fileDialog.exec())
		return QString();
	
	if (fileDialog.selectedFiles().size() == 0)
		return QString();
	
	appController()->settingsManager()->setLastFileDialogPath(fileDialog.directory().path());
	return fileDialog.selectedFiles().first();
}

QString FileDialog::getFilePath(QWidget *parent, const QString &title, Mode mode, const QString &filterText, const QStringList &filterSuffixes)
{
	return getFilePath(parent, title, mode, { QPair<QString, QStringList>(filterText, filterSuffixes) });
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
