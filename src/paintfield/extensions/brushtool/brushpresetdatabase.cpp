#include "brushpresetdatabase.h"


#include "paintfield/core/json.h"
#include "paintfield/core/util.h"
#include "brushpresetitem.h"

#include <QStandardItemModel>
#include <QItemSelectionModel>
#include <amulet/int_range.hh>

namespace PaintField
{

struct BrushPresetDatabase::Data
{
	QStandardItemModel *mModel;
	QItemSelectionModel *mSelectionModel;
};

BrushPresetDatabase::BrushPresetDatabase(QObject *parent) :
	QObject(parent),
	d(new Data)
{
	d->mModel = new QStandardItemModel(this);
	d->mSelectionModel = new QItemSelectionModel(d->mModel, this);
	d->mModel->setItemPrototype(new BrushPresetItem());

	connect(d->mSelectionModel, &QItemSelectionModel::currentChanged, this, [this](const QModelIndex &current, const QModelIndex &){
		auto item = dynamic_cast<BrushPresetItem *>(d->mModel->itemFromIndex(current));
		if (item && item->isGroup())
			item = nullptr;
		emit currentPresetChanged(item);
	});
}

BrushPresetDatabase::~BrushPresetDatabase()
{

}

void BrushPresetDatabase::load(const QString &filePath)
{
	QStandardItem *current = nullptr;

	PAINTFIELD_DEBUG << "loading" << filePath;
	auto variant = Json::readFromFile(filePath);
	auto items = BrushPresetItem::loadItemTree(variant.toList(), current);
	PAINTFIELD_DEBUG << "loaded" << items.count() << "items";
	d->mModel->invisibleRootItem()->appendRows(items);

	if (current) {
		d->mSelectionModel->setCurrentIndex(d->mModel->indexFromItem(current), QItemSelectionModel::SelectCurrent);
	}
}

void BrushPresetDatabase::save(const QString &filePath) const
{
	auto current = d->mModel->itemFromIndex(d->mSelectionModel->currentIndex());
	Json::writeIntoFile(filePath, BrushPresetItem::saveItemTree(Util::itemChildren(d->mModel->invisibleRootItem()), current));
}

QStandardItemModel *BrushPresetDatabase::model()
{
	return d->mModel;
}

QItemSelectionModel *BrushPresetDatabase::selectionModel()
{
	return d->mSelectionModel;
}

BrushPresetItem *BrushPresetDatabase::currentPreset()
{
	return dynamic_cast<BrushPresetItem *>(d->mModel->itemFromIndex(d->mSelectionModel->currentIndex()));
}

}
