#ifndef BRUSHLIBRARYMODEL_H
#define BRUSHLIBRARYMODEL_H

#include "paintfield/core/librarymodel.h"

namespace PaintField {

class BrushLibraryModel : public LibraryModel
{
	Q_OBJECT
public:
	explicit BrushLibraryModel(QObject *parent = 0);
	
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	
	QVariantMap loadPreset(const QModelIndex &index);
	bool savePreset(const QModelIndex &parent, const QString &name, const QVariantMap &data);
	
signals:
	
public slots:
	
};

}

#endif // BRUSHLIBRARYMODEL_H
