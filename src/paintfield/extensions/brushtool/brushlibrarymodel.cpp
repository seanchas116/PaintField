#include "brushlibrarymodel.h"

#include "paintfield/core/json.h"
#include "paintfield/core/appcontroller.h"
#include "paintfield/core/application.h"
#include "paintfield/core/settingsmanager.h"

#include <QItemSelectionModel>

namespace PaintField {

BrushLibraryModel::BrushLibraryModel(QObject *parent) :
    LibraryModel(parent)
{
	this->addRootPath(QDir(appController()->settingsManager()->builtinDataDir()).filePath("Contents/Brush Presets"), "Built-in");
	this->addRootPath(QDir(appController()->settingsManager()->userDataDir()).filePath("Contents/Brush Presets"), "User");

	m_selectionModel = new QItemSelectionModel(this, this);

	auto setCurrentItem = [this](QStandardItem *item) {
		m_selectionModel->setCurrentIndex(this->indexFromItem(item), QItemSelectionModel::SelectCurrent);
	};

	connect(appController()->app(), &Application::tabletPointerChanged, this, [this, setCurrentItem](const TabletPointerInfo &curr, const TabletPointerInfo &prev){
		m_itemHash[prev] = this->itemFromIndex(m_selectionModel->currentIndex());

		if (m_itemHash.contains(curr)) {
			setCurrentItem(m_itemHash[curr]);
		} else {
			if (curr.type == QTabletEvent::Eraser)
				setCurrentItem(m_defaultEraserItem);
			else
				setCurrentItem(m_defaultPenItem);
		}
	});

	auto onCurrentChanged = [this](const QModelIndex &index, const QModelIndex &prev) {
		emit currentPathChanged(pathFromIndex(index), pathFromIndex(prev));
	};

	connect(m_selectionModel, &QItemSelectionModel::currentChanged, this, onCurrentChanged);
	onCurrentChanged(m_selectionModel->currentIndex(), QModelIndex());

	m_defaultPenItem = this->itemFromIndex(this->findIndex({"Built-in", "Simple Brush", "Simple Brush"}));
	m_defaultEraserItem = this->itemFromIndex(this->findIndex({"Built-in", "Pen", "Eraser"}));
	setCurrentItem(m_defaultPenItem);
}

QString BrushLibraryModel::currentPath()
{
	return pathFromIndex(m_selectionModel->currentIndex());
}

QVariant BrushLibraryModel::data(const QModelIndex &index, int role) const
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

void BrushLibraryModel::reload()
{
	auto item = itemFromIndex(m_selectionModel->currentIndex());
	updateDirItem(itemDir(item));
}

}
