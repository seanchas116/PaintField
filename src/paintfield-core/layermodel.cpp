#include <QtGui>
#include <Malachite/Painter>

#include "util.h"
#include "document.h"
#include "layerrenderer.h"
#include "internal/layermodelcommand.h"

#include "layermodel.h"

namespace PaintField
{

LayerPathList LayerPath::sortLayerPathList(const LayerPathList &paths)
{
	LayerPathList result = paths;
	
	auto mySort = [](const LayerPath &path1, const LayerPath &path2) -> bool
	{
		int i = 0;
		forever
		{
			if (path1.size() == i || path2.size() == i)
				break;
			
			if (path1.at(i) == path2.at(i))
				continue;
			
			return path1.at(i) < path2.at(i);
		}
		return false;
	};
	
	std::sort(result.begin(), result.end(), mySort);
	return result;
}

LayerModel::LayerModel(const LayerList &layers, Document *parent) :
    QAbstractItemModel(parent),
    _document(parent),
    _rootLayer(new ModelRootLayer(this) ),
    _selectionModel(new QItemSelectionModel(this, this) ),
    _skipNextUpdate(false)
{
	connect(_selectionModel, SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SIGNAL(currentIndexChanged(QModelIndex,QModelIndex)));
	connect(this, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SIGNAL(layerMetadataChanged(QModelIndex)));
	
	_rootLayer->insertChildren(0, layers);
	_rootLayer->updateThumbnailRecursive(_document->size());
	
	_selectionModel->setCurrentIndex(index(0, QModelIndex()), QItemSelectionModel::Current);
}

void LayerModel::editLayer(const QModelIndex &index, LayerEdit *edit, const QString &description)
{
	if (!checkIndex(index))
	{
		PAINTFIELD_PRINT_WARNING("invalid index");
		return;
	}
	
	QUndoCommand *command = new LayerModelEditCommand(pathForIndex(index), edit, this);
	command->setText(description);
	pushCommand(command);
}

void LayerModel::addLayers(QList<Layer *> layers, const QModelIndex &parent, int row, const QString &description)
{
	if (!checkIndex(parent))
	{
		PAINTFIELD_PRINT_WARNING("invalid index");
		return;
	}
	
	for (Layer *layer : layers)
		layer->updateThumbnailRecursive(document()->size());
	
	auto command = new LayerModelMultipleAddCommand(layers, pathForIndex(parent), row, this);
	command->setText(description);
	pushCommand(command);
}

void LayerModel::newLayer(Layer::Type type, const QModelIndex &parent, int row)
{
	if (!checkIndex(parent))
	{
		PAINTFIELD_PRINT_WARNING("invalid index");
		return;
	}
	
	Layer *layer;
	QString description;
	
	switch(type)
	{
		case Layer::TypeGroup:
			
			layer = new GroupLayer();
			layer->setName(tr("Untitled Group"));
			description = tr("New Group");
			break;
			
		case Layer::TypeRaster:
			
			layer = new RasterLayer();
			layer->setName(tr("Untitled Layer"));
			description = tr("New Layer");
			break;
			
		default:
			
			PAINTFIELD_PRINT_WARNING("unsupported layer type");
			return;
	}
	
	addLayer(layer, parent, row, description);
}

void LayerModel::removeLayers(const QModelIndexList &indexes)
{
	if (!checkIndexes(indexes))
	{
		PAINTFIELD_PRINT_WARNING("invalid index");
		return;
	}
	
	auto command = new LayerModelMultipleRemoveCommand(pathsForIndexes(indexes), this);
	command->setText(tr("Remove Layers"));
	pushCommand(command);
}

void LayerModel::copyLayers(const QModelIndexList &indexes, const QModelIndex &parent, int row)
{
	if (!checkIndex(parent) || !checkIndexes(indexes))
	{
		PAINTFIELD_PRINT_WARNING("invalid index");
		return;
	}
	
	auto command = new LayerModelMultipleCopyCommand(pathsForIndexes(indexes), pathForIndex(parent), row, this);
	command->setText(tr("Copy Layers"));
	pushCommand(command);
}

void LayerModel::moveLayers(const QModelIndexList &indexes, const QModelIndex &parent, int row)
{
	if (!checkIndex(parent) || !checkIndexes(indexes) || !layerForIndex(parent)->insertable(row))
	{
		PAINTFIELD_PRINT_WARNING("invalid index");
		return;
	}
	
	auto command = new LayerModelMultipleMoveCommand(pathsForIndexes(indexes), pathForIndex(parent), row, this);
	command->setText(tr("Move Layers"));
	pushCommand(command);
}

void LayerModel::renameLayer(const QModelIndex &index, const QString &newName)
{
	if (!checkIndex(index))
	{
		PAINTFIELD_PRINT_WARNING("invalid index");
		return;
	}
	
	QString unduplicatedName = unduplicatedChildName(index.parent(), newName);
	
	auto command = new LayerModelPropertyChangeCommand(pathForIndex(index), unduplicatedName, PaintField::RoleName, this);
	command->setText(tr("Rename Layer"));
	pushCommand(command);
}

void LayerModel::mergeLayers(const QModelIndex &parent, int from, int to)
{
	{
		auto layer = layerForIndex(parent);
		if (!checkLayer(layer))
		{
			qWarning() << Q_FUNC_INFO << ": invalid parent";
			return;
		}
		if (from > to || !layer->contains(from) || !layer->contains(to))
		{
			qWarning() << Q_FUNC_INFO << ": invalid row";
			return;
		}
	}
	
	auto command = new LayerModelMergeCommand(pathForIndex(parent), from, to - from + 1, this);
	command->setText(tr("Merge Layers"));
	
	pushCommand(command);
}

QString LayerModel::unduplicatedChildName(const QModelIndex &index, const QString &name) const
{
	const Layer *layer = layerForIndex(index);
	if (!layer) return QString();
	
	return layer->unduplicatedChildName(name);
}

QVariant LayerModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid() )
		return QVariant();
	
	switch (role) {
	case Qt::EditRole:
	case Qt::DisplayRole:
		role = PaintField::RoleName;
		break;
	case Qt::DecorationRole:
		role = PaintField::RoleThumbnail;
		break;
	default:
		break;
	}
	
	const Layer *layer = layerForIndex(index);
	return layer->property(role);
}

bool LayerModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	return setData(index, value, role, QString());
}

bool LayerModel::setData(const QModelIndex &index, const QVariant &value, int role, const QString &description)
{
	QString text = description;
	
	if (!index.isValid() )
		return false;
	
	switch (role) {
	case Qt::EditRole:
	case Qt::DisplayRole:
		role = PaintField::RoleName;
		
		if (description.isNull())
			text = tr("Rename Layer");
		
		break;
	default:
		break;
	}
	
	if (data(index, role) == value)
		return true;
	
	if (role == PaintField::RoleName)
		renameLayer(index, value.toString());
	else
		editLayer(index, new FSLayerPropertyEdit(value, role), text);
	 
	return true;
}

Qt::ItemFlags LayerModel::flags(const QModelIndex &index) const
{
	if (layerForIndex(index)->type() == Layer::TypeGroup)
		return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsDropEnabled | Qt::ItemIsDragEnabled | Qt::ItemIsEnabled;
	else
		return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsEnabled;
}

QModelIndex LayerModel::index(int row, int column, const QModelIndex &parent) const
{
	if (!hasIndex(row, column, parent) )
		return QModelIndex();	// root
	
	const Layer *childItem = layerForIndex(parent)->child(row);
	return createIndex(row, column, const_cast<Layer *>(childItem) );
}

QModelIndex LayerModel::parent(const QModelIndex &child) const
{
	if (!child.isValid() )
		return QModelIndex();
	
	const Layer *parentItem = layerForIndex(child)->parent();
	
	if (parentItem == rootLayer())
		return QModelIndex();
	
	return createIndex(parentItem->row(), 0, const_cast<Layer *>(parentItem) );
}

int LayerModel::rowCount(const QModelIndex &parent) const
{
	return layerForIndex(parent)->childCount();
}

QStringList LayerModel::mimeTypes() const
{
	QStringList types;
	types << "application/x-freestyle-internal-layer-ref";
	return types;
}

QMimeData *LayerModel::mimeData(const QModelIndexList &indexes) const
{
	QMimeData *mimeData = new QMimeData;
	QByteArray encodedData;
	QDataStream stream(&encodedData, QIODevice::WriteOnly);
	
	foreach (const QModelIndex &index, indexes) {
		stream << reinterpret_cast<quint64>(index.internalPointer() );
	}
	
	mimeData->setData("application/x-freestyle-internal-layer-ref", encodedData);
	
	return mimeData;
}

bool LayerModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
	Q_UNUSED(column);
	
	if (action == Qt::IgnoreAction)
		return true;
	if (!data->hasFormat("application/x-freestyle-internal-layer-ref") )	// uncompatible mime type
		return false;
	
	if (row == -1)
		row = rowCount(parent);
	
	QByteArray encodedData = data->data("application/x-freestyle-internal-layer-ref");
	QDataStream stream(&encodedData, QIODevice::ReadOnly);
	QModelIndexList indexes;
	
	while (!stream.atEnd() ) {
		quint64 p;
		stream >> p;
		indexes << indexForLayer(reinterpret_cast<Layer *>(p));
	}
	
	switch (action) {
	case Qt::CopyAction:
		copyLayers(indexes, parent, row);
		break;
	case Qt::MoveAction:
		moveLayers(indexes, parent, row);
		break;
	default:
		return false;
	}
	return true;
}

const Layer *LayerModel::layerForIndex(const QModelIndex &index) const
{
	const Layer *layer;
	
	if (index.isValid())
	{
		layer = static_cast<Layer *>(index.internalPointer());
		
		if (!checkLayer(layer))
		{
			qWarning() << __PRETTY_FUNCTION__ << ": strange index";
			return 0;
		}
	}
	else
	{
		layer = rootLayer();
	}
	return layer;
}

bool LayerModel::checkIndexes(const QModelIndexList &indexes) const
{
	foreach (const QModelIndex &index, indexes)
	{
		if (!checkIndex(index))
			return false;
	}
	return true;
}

QModelIndex LayerModel::indexForLayer(const Layer *layer) const
{
	if (!checkLayer(layer))
	{
		qWarning() << __PRETTY_FUNCTION__ << ": strange layer";
		return QModelIndex();
	}
	return layer != rootLayer() ? createIndex(layer->row(), 0, const_cast<Layer *>(layer) ) : QModelIndex();
}

const Layer *LayerModel::layerForPath(const LayerPath &path) const
{
	const Layer *layer = rootLayer();
	foreach (int index, path)
	{
		layer = layer->child(index);
		if (!layer)
			return 0;
	}
	return layer;
}

LayerPath LayerModel::pathForLayer(const Layer *layer) const
{
	if (!checkLayer(layer))
	{
		qWarning() << __PRETTY_FUNCTION__ << ": strange layer";
		return LayerPath();
	}
	
	LayerPath path;
	while (layer != rootLayer())
	{
		path.prepend(layer->row());
		layer = layer->parent();
	}
	return path;
}

LayerPathList LayerModel::pathsForIndexes(const QModelIndexList &indexes) const
{
	LayerPathList paths;
	
	foreach (const QModelIndex &index, indexes)
		paths << pathForIndex(index);
	
	return paths;
}

Malachite::Surface LayerModel::render()
{
	LayerRenderer renderer;
	
	return renderer.renderToSurface(rootLayer()->children(), document()->tileKeys());
}

void LayerModel::updateDirtyThumbnails()
{
	_rootLayer->updateDirtyThumbnailRecursive(_document->size());
}

void LayerModel::pushCommand(QUndoCommand *command)
{
	emit modified();
	
	_document->undoStack()->push(command);
	
	if (_skipNextUpdate)
		_skipNextUpdate = false;
	else
		emit tilesUpdated(_updatedTiles);
	
	_updatedTiles.clear();
}

}

