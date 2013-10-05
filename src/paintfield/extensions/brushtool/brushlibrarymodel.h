#pragma once

#include "paintfield/core/tabletpointerinfo.h"
#include "paintfield/core/librarymodel.h"

class QItemSelectionModel;

namespace PaintField {

class BrushLibraryModel : public LibraryModel
{
	Q_OBJECT
public:
	explicit BrushLibraryModel(QObject *parent = 0);

	QItemSelectionModel *selectionModel() { return m_selectionModel; }
	QString currentPath();
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
	
signals:

	void currentPathChanged(const QString &path, const QString &old);
	
public slots:

	void reload();
	
private:

	QItemSelectionModel *m_selectionModel;
	QHash<TabletPointerInfo, QStandardItem *> m_itemHash;
	QStandardItem *m_defaultPenItem = nullptr;
	QStandardItem *m_defaultEraserItem = nullptr;
};

}

