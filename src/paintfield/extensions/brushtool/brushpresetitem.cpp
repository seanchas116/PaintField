#include "brushpresetitem.h"
#include "paintfield/core/util.h"
#include <QLocale>

namespace PaintField {

BrushPresetItem::BrushPresetItem(bool isGroup)
{
	setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsEnabled);
	setGroup(isGroup);
	setSizeHint(QSize(24,24));
}

BrushPresetItem::BrushPresetItem(const QVariantMap &map) :
	BrushPresetItem()
{
	setVariantMap(map);
}

QStandardItem *BrushPresetItem::clone() const
{
	return new BrushPresetItem();
}

void BrushPresetItem::setData(const QVariant &value, int role)
{
	switch (role) {
		case RoleVariantMap:
			setVariantMap(value.toMap());
			break;
		case RoleStroker:
			mStroker = value.toString();
			break;
		case RoleParameters:
			mParameters = value.toMap();
			break;
		default:
			QStandardItem::setData(value, role);
			break;
	}
}

QVariant BrushPresetItem::data(int role) const
{
	switch (role) {
		case RoleVariantMap:
			return toVariantMap();
		case RoleStroker:
			return mStroker;
		case RoleParameters:
			return mParameters;
		case RoleGroup:
			return mIsGroup;
		default:
			return QStandardItem::data(role);
	}
}

void BrushPresetItem::read(QDataStream &in)
{
	QStandardItem::read(in);
	bool group;
	in >> group >> mStroker >> mParameters;
	setGroup(group);
}

void BrushPresetItem::write(QDataStream &out) const
{
	QStandardItem::write(out);
	out << mIsGroup << mStroker << mParameters;
}

void BrushPresetItem::setVariantMap(const QVariantMap &map)
{
	auto type = map["type"].toString();

	auto titleVariant = map["title"];
	if (titleVariant.canConvert(QMetaType::QString)) {
		this->setText(titleVariant.toString());
	} else if (titleVariant.canConvert(QMetaType::QVariantMap)) {
		auto titleMap = titleVariant.toMap();
		for (auto i = titleMap.begin(); i != titleMap.end(); ++i) {
			PAINTFIELD_DEBUG << i.key();
			if (QLocale::system().language() == QLocale(i.key()).language()) {
				this->setText(i.value().toString());
			}
		}
	}

	if (type == "group") {
		setGroup(true);
		mStroker = QString();
		mParameters = QVariantMap();
	} else { // "preset"
		setGroup(false);
		auto preset = map["preset"].toMap();
		mStroker = preset["stroker"].toString();
		mParameters = preset["parameters"].toMap();
	}
}

QVariantMap BrushPresetItem::toVariantMap() const
{
	QVariantMap map;
	map["type"] = mIsGroup ? "group" : "preset";
	map["title"] = this->text();

	if (!mIsGroup) {
		QVariantMap preset;
		preset["stroker"] = mStroker;
		preset["parameters"] = mParameters;
		map["preset"] = preset;
	}

	return map;
}

int BrushPresetItem::type() const
{
	return TypeBrushPreset;
}

void BrushPresetItem::setGroup(bool isGroup)
{
	auto flags = this->flags();
	mIsGroup = isGroup;
	if (isGroup) {
		this->setIcon(QPixmap(":/icons/24x24/folder.png"));
		flags |= Qt::ItemIsDropEnabled;
	} else {
		this->setIcon(QIcon());
		flags &= ~Qt::ItemIsDropEnabled;
	}
	this->setFlags(flags);
}

QList<QStandardItem *> BrushPresetItem::loadItemTree(const QVariantList &list, QStandardItem *&current)
{
	QList<QStandardItem *> result;

	for (const auto &variant : list) {
		auto map = variant.toMap();
		auto item = new BrushPresetItem(map);
		if (item->isGroup()) {
			item->appendRows(loadItemTree(map["children"].toList(), current));
		}
		if (map["current"].toBool()) {
			current = item;
		}
		result << item;
	}
	return result;
}

QVariantList BrushPresetItem::saveItemTree(const QList<QStandardItem *> &items, QStandardItem *current)
{
	QVariantList result;
	for (auto item : items) {
		QVariantMap map = item->data(BrushPresetItem::RoleVariantMap).toMap();
		if (item->data(BrushPresetItem::RoleGroup).toBool()) {
			map["children"] = saveItemTree(Util::itemChildren(item), current);
		}
		if (item == current) {
			map["current"] = true;
		}
		result << map;
	}
	return result;
}

} // namespace PaintField
