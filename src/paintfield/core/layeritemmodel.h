#ifndef PAINTFIELD_LAYERITEMMODEL_H
#define PAINTFIELD_LAYERITEMMODEL_H

#include <QAbstractItemModel>
#include "layer.h"

namespace PaintField {

class LayerScene;

class LayerItemModel : public QAbstractItemModel
{
	Q_OBJECT
public:
	explicit LayerItemModel(LayerScene *scene, QObject *parent = 0);
	~LayerItemModel();
	
	QVariant data(const QModelIndex &index, int role) const;
	bool setData(const QModelIndex &index, const QVariant &value, int role);
	Qt::ItemFlags flags(const QModelIndex &index) const;
	QModelIndex index(int row, int column, const QModelIndex &parent) const;
	QModelIndex parent(const QModelIndex &child) const;
	int rowCount(const QModelIndex &parent) const;
	int columnCount(const QModelIndex &parent) const;
	Qt::DropActions supportedDropActions() const;
	QStringList mimeTypes() const;
	QMimeData *mimeData(const QModelIndexList &indexes) const;
	bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent);
	
	/**
	 * Returns the layer that is associated with the given index.
	 * If the index is invalid, returns the root layer.
	 * @param index
	 * @return 
	 */
	LayerRef layerForIndex(const QModelIndex &index) const;
	
	/**
	 * Returns the layer that is associated with the given index.
	 * If the index is invalid, returns the invalid layer.
	 * @param index
	 * @return 
	 */
	LayerRef layerExceptRootForIndex(const QModelIndex &index) const;
	
	QModelIndex indexForLayer(const LayerRef &ref) const;
	
	LayerRefList layersForIndexes(const QModelIndexList &indexes) const;
	QModelIndexList indexesFromLayers(const LayerRefList &layers) const;
	
	int normalizeRole(int role) const;
	
signals:
	
public slots:
	
private slots:
	
	void onLayerAboutToBeInserted(const LayerRef &parent, int index);
	void onLayerInserted(const LayerRef &parent, int index);
	void onLayerAboutToBeRemoved(const LayerRef &parent, int index);
	void onLayerRemoved(const LayerRef &parent, int index);
	
	void onLayerPropertyChanged(const LayerRef &layer);
	
private:
	
	struct Data;
	Data *d;
};

} // namespace PaintField

#endif // PAINTFIELD_LAYERITEMMODEL_H
