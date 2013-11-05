#pragma once

#include <QStandardItem>

namespace PaintField {

class BrushPresetItem : public QStandardItem
{
public:

	enum Role
	{
		RoleVariantMap = Qt::UserRole,
		RoleStroker,
		RoleParameters,
		RoleGroup
	};

	enum Type
	{
		TypeBrushPreset = QStandardItem::UserType
	};

	BrushPresetItem(bool isGroup = false);
	BrushPresetItem(const QVariantMap &map);

	QStandardItem *clone() const override;

	void setData(const QVariant &value, int role) override;
	QVariant data(int role) const override;

	void read(QDataStream &in) override;
	void write(QDataStream &out) const override;

	QVariantMap parameters() const { return mParameters; }
	QVariantMap &parametersRef() { return mParameters; }

	QString stroker() const { return mStroker; }
	void setStroker(const QString &stroker) { mStroker = stroker; }

	void setVariantMap(const QVariantMap &map);
	QVariantMap toVariantMap() const;

	int type() const override;

	bool isGroup() const { return mIsGroup; }

	static QList<QStandardItem *> loadItemTree(const QVariantList &list, QStandardItem *&current);
	static QVariantList saveItemTree(const QList<QStandardItem *> &items, QStandardItem *current);

private:

	void setGroup(bool isGroup);

	bool mIsGroup;
	QString mStroker;
	QVariantMap mParameters;
};

} // namespace PaintField
