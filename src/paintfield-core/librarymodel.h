#ifndef PAINTFIELD_LIBRARYMODEL_H
#define PAINTFIELD_LIBRARYMODEL_H

#include <QStandardItemModel>
#include <QDir>

namespace PaintField {

enum LibraryItemType
{
	File = QStandardItem::UserType,
	Dir,
	Root
};

class LibraryRootItem : public QStandardItem
{
public:
	
	LibraryRootItem(const QString &path, const QString &text) :
	    QStandardItem(text),
	    _path(path)
	{
		setEditable(false);
	}
	
	QString path() const { return _path; }
	int type() const override { return LibraryItemType::Root; }
	
private:
	
	QString _path;
};

class LibraryItem : public QStandardItem
{
public:
	
	LibraryItem(const QString &name, LibraryItemType type) :
	    QStandardItem(name),
	    _type(type)
	{
		setEditable(false);
	}
	
	int type() const override { return _type; }
	
private:
	
	LibraryItemType _type;
};

class LibraryModel : public QStandardItemModel
{
	Q_OBJECT
public:
	
	explicit LibraryModel(QObject *parent = 0);
	
	/**
	 * @param item
	 * @return The directory item which contains "item"
	 */
	QStandardItem *itemDir(QStandardItem *item);
	
	/**
	 * Same as pathFromItem(itemDir(item)).
	 */
	QString dirPathFromItem(QStandardItem *item) { return pathFromItem(itemDir(item)); }
	
	/**
	 * @param item
	 * @return The path of the item
	 */
	QString pathFromItem(QStandardItem *item);
	
	/**
	 * @param index
	 * @return The path of the item index
	 */
	QString pathFromIndex(const QModelIndex &index) { return pathFromItem(itemFromIndex(index)); }
	
	LibraryItem *libraryItem(QStandardItem *item);
	LibraryItem *libraryItemFromIndex(const QModelIndex &index) { return libraryItem(itemFromIndex(index)); }
	
	void addRootPath(const QString &path, const QString &displayText);
	void updateDirItem(QStandardItem *item);
	void updateDirItem(const QModelIndex &index) { updateDirItem(itemFromIndex(index)); }
	
	static QStandardItem *createTreeRecursive(const QDir &dir);
	static QList<QStandardItem *> createChildItems(const QDir &dir);
	
signals:
	
public slots:
	
};

} // namespace PaintField

#endif // PAINTFIELD_LIBRARYMODEL_H
