#include <QMimeData>
#include <QByteArray>
#include <QDataStream>

#include "layerscene.h"
#include "grouplayer.h"

#include "layeritemmodel.h"

namespace PaintField {

struct LayerItemModel::Data
{
	LayerScene *scene = 0;
};

LayerItemModel::LayerItemModel(LayerScene *scene, QObject *parent) :
	QAbstractItemModel(parent),
	d(new Data)
{
	d->scene = scene;
	connect(scene, SIGNAL(layerAboutToBeInserted(LayerConstRef,int)), this, SLOT(onLayerAboutToBeInserted(LayerConstRef,int)));
	connect(scene, SIGNAL(layerInserted(LayerConstRef,int)), this, SLOT(onLayerInserted(LayerConstRef,int)));
	connect(scene, SIGNAL(layerAboutToBeRemoved(LayerConstRef,int)), this, SLOT(onLayerAboutToBeRemoved(LayerConstRef,int)));
	connect(scene, SIGNAL(layerRemoved(LayerConstRef,int)), this, SLOT(onLayerRemoved(LayerConstRef,int)));
	connect(scene, SIGNAL(layerChanged(LayerConstRef)), this, SLOT(onLayerPropertyChanged(LayerConstRef)));
}

LayerItemModel::~LayerItemModel()
{
	delete d;
}

QVariant LayerItemModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();
	
	auto value = layerForIndex(index)->property(normalizeRole(role));
	
	if (role == Qt::CheckStateRole)
		return value.toBool() ? Qt::Checked : Qt::Unchecked;
	
	return value;
}

bool LayerItemModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if (!index.isValid())
		return false;
	
	d->scene->setLayerProperty(layerForIndex(index), value, normalizeRole(role));
	return true;
}

Qt::ItemFlags LayerItemModel::flags(const QModelIndex &index) const
{
	const Qt::ItemFlags layerFlags = Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable;
	const Qt::ItemFlags groupFlags = layerFlags | Qt::ItemIsDropEnabled;
	
	if (layerForIndex(index)->isType<GroupLayer>())
		return groupFlags;
	else
		return layerFlags;
}

QModelIndex LayerItemModel::index(int row, int column, const QModelIndex &parent) const
{
	if (!hasIndex(row, column, parent) )
		return QModelIndex();	// root
	
	return indexForLayer(layerForIndex(parent)->child(row));
}

QModelIndex LayerItemModel::parent(const QModelIndex &child) const
{
	if (!child.isValid())
		return QModelIndex();
	
	return indexForLayer(layerForIndex(child)->parent());
}

int LayerItemModel::rowCount(const QModelIndex &parent) const
{
	return layerForIndex(parent)->count();
}

int LayerItemModel::columnCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent)
	return 1;
}

Qt::DropActions LayerItemModel::supportedDropActions() const
{
	return Qt::MoveAction | Qt::CopyAction;
}

static const QString _layerRefMimeType = "application/x-paintfield-layer-ref";

QStringList LayerItemModel::mimeTypes() const
{
	return { _layerRefMimeType };
}

QMimeData *LayerItemModel::mimeData(const QModelIndexList &indexes) const
{
	auto mimeData = new QMimeData();
	
	{
		QByteArray data;
		QDataStream stream(&data, QIODevice::WriteOnly);
		
		for (auto index : indexes)
			stream << d->scene->pathForLayer(layerForIndex(index));
		
		mimeData->setData(_layerRefMimeType, data);
	}
	
	return mimeData;
}

bool LayerItemModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
	Q_UNUSED(column)
	
	if (action == Qt::IgnoreAction)
		return true;
	
	auto parentLayer = layerForIndex(parent);
	
	if (data->hasFormat(_layerRefMimeType))
	{
		if (row == -1)
			row = rowCount(parent);
		
		QByteArray encodedData = data->data(_layerRefMimeType);
		QDataStream stream(&encodedData, QIODevice::ReadOnly);
		
		QList<LayerConstRef> layers;
		
		while (!stream.atEnd())
		{
			QList<int> path;
			stream >> path;
			
			auto layer = d->scene->layerForPath(path);
			if (!layer)
			{
				PAINTFIELD_WARNING << "failed to decode";
				return false;
			}
			
			layers << layer;
			
			if (action == Qt::MoveAction && layer->root() != d->scene->rootLayer())
				action = Qt::CopyAction;
		}
		
		switch (action)
		{
			default:
			case Qt::CopyAction:
				d->scene->copyLayers(layers, parentLayer, row);
				break;
			case Qt::MoveAction:
				d->scene->moveLayers(layers, parentLayer, row);
				break;
		}
		
		return true;
	}
	return false;
}

LayerConstRef LayerItemModel::layerForIndex(const QModelIndex &index) const
{
	if (index.isValid())
		return static_cast<const Layer *>(index.internalPointer())->shared_from_this();
	else
		return d->scene->rootLayer();
}

LayerConstRef LayerItemModel::layerExceptRootForIndex(const QModelIndex &index) const
{
	if (index.isValid())
		return static_cast<const Layer *>(index.internalPointer())->shared_from_this();
	else
		return nullptr;
}

QModelIndex LayerItemModel::indexForLayer(const LayerConstRef &layer) const
{
	if (!layer)
		return QModelIndex();
	
	if (layer == d->scene->rootLayer())
		return QModelIndex();
	else
		return createIndex(layer->index(), 0, const_cast<Layer *>(layer.get()));
}

QList<LayerConstRef> LayerItemModel::layersForIndexes(const QModelIndexList &indexes) const
{
	QList<LayerConstRef> layers;
	layers.reserve(indexes.size());
	
	for (const auto &index : indexes)
		layers << layerForIndex(index);
	
	return layers;
}

QModelIndexList LayerItemModel::indexesFromLayers(const QList<LayerConstRef> &layers) const
{
	QModelIndexList indexes;
	indexes.reserve(layers.size());
	
	for (const auto &layer : layers)
		indexes << indexForLayer(layer);
	
	return indexes;
}

int LayerItemModel::normalizeRole(int role) const
{
	switch (role)
	{
		case Qt::EditRole:
		case Qt::DisplayRole:
			return PaintField::RoleName;
		case Qt::DecorationRole:
			return PaintField::RoleThumbnail;
		case Qt::CheckStateRole:
			return PaintField::RoleVisible;
		default:
			return role;
	}
}

void LayerItemModel::onLayerAboutToBeInserted(const LayerConstRef &parent, int index)
{
	beginInsertRows(indexForLayer(parent), index, index);
}

void LayerItemModel::onLayerInserted(const LayerConstRef &parent, int index)
{
	Q_UNUSED(parent)
	Q_UNUSED(index)
	endInsertRows();
}

void LayerItemModel::onLayerAboutToBeRemoved(const LayerConstRef &parent, int index)
{
	beginRemoveRows(indexForLayer(parent), index, index);
}

void LayerItemModel::onLayerRemoved(const LayerConstRef &parent, int index)
{
	Q_UNUSED(parent)
	Q_UNUSED(index)
	endRemoveRows();
}

void LayerItemModel::onLayerPropertyChanged(const LayerConstRef &layer)
{
	auto idx = indexForLayer(layer);
	emit dataChanged(idx, idx);
}


} // namespace PaintField
