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
	connect(scene, SIGNAL(layerAboutToBeInserted(LayerRef,int)), this, SLOT(onLayerAboutToBeInserted(LayerRef,int)));
	connect(scene, SIGNAL(layerInserted(LayerRef,int)), this, SLOT(onLayerInserted(LayerRef,int)));
	connect(scene, SIGNAL(layerAboutToBeRemoved(LayerRef,int)), this, SLOT(onLayerAboutToBeRemoved(LayerRef,int)));
	connect(scene, SIGNAL(layerRemoved(LayerRef,int)), this, SLOT(onLayerRemoved(LayerRef,int)));
	connect(scene, SIGNAL(layerPropertyChanged(LayerRef)), this, SLOT(onLayerPropertyChanged(LayerRef)));
}

LayerItemModel::~LayerItemModel()
{
	delete d;
}

QVariant LayerItemModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();
	
	auto value = layerForIndex(index).pointer()->property(normalizeRole(role));
	
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
	
	if (layerForIndex(index).pointer()->isType<GroupLayer>())
		return groupFlags;
	else
		return layerFlags;
}

QModelIndex LayerItemModel::index(int row, int column, const QModelIndex &parent) const
{
	if (!hasIndex(row, column, parent) )
		return QModelIndex();	// root
	
	return indexForLayer(layerForIndex(parent).child(row));
}

QModelIndex LayerItemModel::parent(const QModelIndex &child) const
{
	if (!child.isValid())
		return QModelIndex();
	
	return indexForLayer(layerForIndex(child).parent());
}

int LayerItemModel::rowCount(const QModelIndex &parent) const
{
	return layerForIndex(parent).count();
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
			stream << reinterpret_cast<uint64_t>(layerForIndex(index).pointer());
		
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
		
		LayerRefList layers;
		
		while (!stream.atEnd())
		{
			uint64_t value;
			stream >> value;
			
			auto layer = reinterpret_cast<const Layer *>(value);
			if (!layer)
			{
				PAINTFIELD_WARNING << "failed to decode";
				return false;
			}
			layers << LayerRef(layer);
			
			if (action == Qt::MoveAction && layer->root() != d->scene->rootLayer().pointer())
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

LayerRef LayerItemModel::layerForIndex(const QModelIndex &index) const
{
	if (index.isValid())
		return static_cast<const Layer *>(index.internalPointer());
	else
		return d->scene->rootLayer();
}

LayerRef LayerItemModel::layerExceptRootForIndex(const QModelIndex &index) const
{
	if (index.isValid())
		return static_cast<const Layer *>(index.internalPointer());
	else
		return LayerRef();
}

QModelIndex LayerItemModel::indexForLayer(const LayerRef &ref) const
{
	if (!ref.isValid())
		return QModelIndex();
	
	if (ref == d->scene->rootLayer())
		return QModelIndex();
	else
		return createIndex(ref.index(), 0, const_cast<Layer *>(ref.pointer()));
}

LayerRefList LayerItemModel::layersForIndexes(const QModelIndexList &indexes) const
{
	LayerRefList layers;
	layers.reserve(indexes.size());
	
	for (auto index : indexes)
		layers << layerForIndex(index);
	
	return layers;
}

QModelIndexList LayerItemModel::indexesFromLayers(const LayerRefList &layers) const
{
	QModelIndexList indexes;
	indexes.reserve(layers.size());
	
	for (auto layer : layers)
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

void LayerItemModel::onLayerAboutToBeInserted(const LayerRef &parent, int index)
{
	beginInsertRows(indexForLayer(parent), index, index);
}

void LayerItemModel::onLayerInserted(const LayerRef &parent, int index)
{
	Q_UNUSED(parent)
	Q_UNUSED(index)
	endInsertRows();
}

void LayerItemModel::onLayerAboutToBeRemoved(const LayerRef &parent, int index)
{
	beginRemoveRows(indexForLayer(parent), index, index);
}

void LayerItemModel::onLayerRemoved(const LayerRef &parent, int index)
{
	Q_UNUSED(parent)
	Q_UNUSED(index)
	endRemoveRows();
}

void LayerItemModel::onLayerPropertyChanged(const LayerRef &layer)
{
	auto idx = indexForLayer(layer);
	emit dataChanged(idx, idx);
}


} // namespace PaintField
