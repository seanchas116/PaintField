#include "paintfield/core/json.h"

#include "brushlibrarymodel.h"

namespace PaintField {

BrushLibraryModel::BrushLibraryModel(QObject *parent) :
    LibraryModel(parent)
{
}

QVariant  BrushLibraryModel::data(const QModelIndex &index, int role) const
{
	auto value = LibraryModel::data(index, role);
	
	switch (role)
	{
		case Qt::DisplayRole:
		case Qt::EditRole:
		{
			if (itemFromIndex(index)->type() == LibraryItemType::File)
				// hide extension
				return value.toString().section('.', 0, -2);
			else
				break;
		}
		default:
			break;
	}
	return value;
}

}
