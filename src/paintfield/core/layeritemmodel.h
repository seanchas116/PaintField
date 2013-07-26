#pragma once

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
	LayerConstRef layerForIndex(const QModelIndex &index) const;
	
	/**
	 * Returns the layer that is associated with the given index.
	 * If the index is invalid, returns the invalid layer.
	 * @param index
	 * @return 
	 */
	LayerConstRef layerExceptRootForIndex(const QModelIndex &index) const;
	
	QModelIndex indexForLayer(const LayerConstRef &layer) const;
	
	QList<LayerConstRef> layersForIndexes(const QModelIndexList &indexes) const;
	QModelIndexList indexesFromLayers(const QList<LayerConstRef> &layers) const;
	
	int normalizeRole(int role) const;
	
signals:
	
public slots:
	
private slots:
	
	void onLayerAboutToBeInserted(const LayerConstRef &parent, int index);
	void onLayerInserted(const LayerConstRef &parent, int index);
	void onLayerAboutToBeRemoved(const LayerConstRef &parent, int index);
	void onLayerRemoved(const LayerConstRef &parent, int index);
	
	void onLayerPropertyChanged(const LayerConstRef &layer);
	
private:
	
	struct Data;
	Data *d;
};

} // namespace PaintField

