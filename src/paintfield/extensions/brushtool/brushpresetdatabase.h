#pragma once

#include <QObject>

class QStandardItemModel;
class QItemSelectionModel;

namespace PaintField
{

class BrushPresetItem;

class BrushPresetDatabase : public QObject
{
	Q_OBJECT
public:
	explicit BrushPresetDatabase(QObject *parent = 0);
	~BrushPresetDatabase();

	void load(const QString &filePath);
	void save(const QString &filePath) const;

	QStandardItemModel *model();
	QItemSelectionModel *selectionModel();

	BrushPresetItem *currentPreset();

signals:

	void currentPresetChanged(BrushPresetItem *item);

public slots:

private:

	struct Data;
	QScopedPointer<Data> d;
};

}
