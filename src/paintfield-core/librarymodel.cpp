#include <QDebug>

#include "librarymodel.h"

namespace PaintField {

LibraryModel::LibraryModel(QObject *parent) :
    QStandardItemModel(parent)
{
}

QStandardItem *LibraryModel::itemDir(QStandardItem *item)
{
	forever
	{
		if (!item)
			return 0;
		
		if (item->type() == LibraryItemType::Root || item->type() == LibraryItemType::Dir)
			return item;
		
		item = item->parent();
	}
}

QString LibraryModel::pathFromItem(QStandardItem *item)
{
	if (!item)
	{
		PAINTFIELD_DEBUG << "invalid item";
		return QString();
	}
	
	if (item->type() == LibraryItemType::Root)
	{
		return static_cast<LibraryRootItem *>(item)->path();
	}
	else
	{
		auto getAncestorList = [](QStandardItem *item)
		{
			QList<QStandardItem *> list;
			
			while (item)
			{
				list.prepend(item);
				item = item->parent();
			}
			
			return list;
		};
		
		auto ancestors = getAncestorList(item);
		
		auto root = dynamic_cast<LibraryRootItem *>(ancestors.first());
		Q_CHECK_PTR(root);
		
		auto dir = QDir(root->path());
		
		for (auto ancestor : ancestors.mid(1, ancestors.size()-2))
			dir.cd(ancestor->text());
		
		return dir.filePath(ancestors.last()->text());
	}
}

LibraryItem *LibraryModel::libraryItem(QStandardItem *item)
{
	return dynamic_cast<LibraryItem *>(item);
}

void LibraryModel::addRootPath(const QString &path, const QString &displayText)
{
	auto dir = QDir(path);
	
	if (!dir.exists())
	{
		PAINTFIELD_DEBUG << "directory" << path << "does not exist";
		return;
	}
	
	auto item = new LibraryRootItem(path, displayText);
	item->appendRows(createChildItems(dir));
	
	invisibleRootItem()->appendRow(item);
}

void LibraryModel::updateDirItem(QStandardItem *item)
{
	if (!item)
		return;
	
	if (item->type() == LibraryItemType::Dir || item->type() == LibraryItemType::Root)
	{
		auto dir = QDir(pathFromItem(item));
		
		item->removeRows(0, item->rowCount());
		item->appendRows(createChildItems(dir));
	}
}

QStandardItem *LibraryModel::createTreeRecursive(const QDir &dir)
{
	auto item = new LibraryItem(dir.dirName(), LibraryItemType::Dir);
	item->appendRows(createChildItems(dir));
	return item;
}

QList<QStandardItem *> LibraryModel::createChildItems(const QDir &dir)
{
	QList<QStandardItem *> items;
	
	for (const QFileInfo &fileInfo : dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot))
	{
		if (fileInfo.isDir())
			items.append(createTreeRecursive(QDir(fileInfo.filePath())));
		else if (fileInfo.isFile())
			items.append(new LibraryItem(fileInfo.fileName(), LibraryItemType::File));
	}
	
	return items;
}

QModelIndex LibraryModel::findIndex(const QString &text, const QModelIndex &parent) const
{
	int count = rowCount(parent);
	
	for (int i = 0; i < count; ++i)
	{
		QModelIndex index = this->index(i, 0, parent);
		
		if (data(index) == text)
			return index;
	}
	
	return QModelIndex();
}

QModelIndex LibraryModel::findIndex(const QStringList &texts, const QModelIndex &parent) const
{
	if (texts.size() == 0)
		return QModelIndex();
	
	QModelIndex child = findIndex(texts.first(), parent);
	
	if (child.isValid())
	{
		if (texts.size() == 1)
			return child;
		else
			return findIndex(texts.mid(1), child);
	}
	else
		return QModelIndex();
}

} // namespace PaintField
