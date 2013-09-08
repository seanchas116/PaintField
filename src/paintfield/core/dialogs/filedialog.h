#pragma once

#include <QString>
#include <QFileDialog>
#include <QHash>

namespace PaintField {

class FileDialog
{
public:
	
	enum Mode
	{
		OpenFile,
		SaveFile
	};
	
	/**
	 * Displays a dialog and gets a file path.
	 * @param parent
	 * @param title The window title
	 * @param mode Open or save
	 * @param filterTextAndSuffixesList Example: { {"Image Files", { "png", "xpm", "jpg" } }, { "Text files", { "txt" } } }
	 * @return The file path the user chose
	 */
	static QString getFilePath(QWidget *parent, const QString &title, Mode mode, const QList<QPair<QString, QStringList>> &filterTextAndSuffixesList);
	
	/**
	 * Displays a dialog and gets a file path.
	 * @param parent
	 * @param title
	 * @param mode
	 * @param filterText Example: "Image Files"
	 * @param filterSuffixes Example: { "png", "xpm", "jpg" }
	 * @return The file path the user chose
	 */
	static QString getFilePath(QWidget *parent, const QString &title, Mode mode, const QString &filterText, const QStringList &filterSuffixes);
	
	static QString getOpenFilePath(QWidget *parent, const QString &title, const QString &filterText, const QStringList &filterSuffixes);
	static QString getSaveFilePath(QWidget *parent, const QString &title, const QString &filterText, const QString &filterSuffix);
};

} // namespace PaintField

