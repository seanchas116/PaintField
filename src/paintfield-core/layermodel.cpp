#include <QtGui>
#include <Malachite/Painter>

#include "debug.h"
#include "util.h"
#include "document.h"
#include "layerrenderer.h"
#include "internal/layermodelcommand.h"

#include "layermodel.h"

namespace PaintField
{

class DuplicatedNameResolver
{
public:
	DuplicatedNameResolver(const QStringList &existingNames) : _existingNames(existingNames) {}
	
	QString resolve(const QString &name)
	{
		QString newName = unduplicatedName(_existingNames, name);
		_existingNames << newName;
		return newName;
	}
	
private:
	
	QStringList _existingNames;
};

LayerModel::LayerModel(const LayerList &layers, Document *parent) :
    QAbstractItemModel(parent),
    _document(parent),
    _rootLayer(new ModelRootLayer(this) ),
    _skipNextUpdate(false)
{
	connect(this, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SIGNAL(layerMetadataChanged(QModelIndex)));
	connect(_document, SIGNAL(modified()), this, SLOT(update()));
	
	_rootLayer->insertChildren(0, layers);
	_rootLayer->updateThumbnailRecursive(_document->size());
}

void LayerModel::editLayer(const QModelIndex &index, LayerEdit *edit, const QString &description)
{
	if (!checkIndex(index))
	{
		PAINTFIELD_WARNING << "invalid index";
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
		PAINTFIELD_WARNING << "invalid index";
		return;
	}
	
	auto command = new QUndoCommand(description);
	
	DuplicatedNameResolver nameResolver(childNames(parent));
	
	for (int i = 0; i < layers.size(); ++i)
	{
		layers.at(i)->setName(nameResolver.resolve(layers.at(i)->name()));
		new LayerModelAddCommand(layers.at(i), pathForIndex(parent), row + i, this, command);
	}
	
	pushCommand(command);
}

void LayerModel::newLayer(Layer::Type type, const QModelIndex &parent, int row)
{
	if (!checkIndex(parent))
	{
		PAINTFIELD_WARNING << "invalid index";
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
			
			PAINTFIELD_WARNING << "unsupported layer type";
			return;
	}
	
	addLayer(layer, parent, row, description);
}

void LayerModel::removeLayers(const QModelIndexList &indexes)
{
	auto command = new QUndoCommand(tr("Remove Layers"));
	
	for (const QModelIndex &index : indexes)
	{
		if (!checkIndex(index) || !index.isValid())
		{
			PAINTFIELD_WARNING << "invalid index";
			continue;
		}
		new LayerModelRemoveCommand(pathForIndex(index), this, command);
	}
	
	pushCommand(command);
}

void LayerModel::copyLayers(const QModelIndexList &indexes, const QModelIndex &parent, int row)
{
	copyOrMoveLayers(indexes, parent, row, true);
}

void LayerModel::moveLayers(const QModelIndexList &indexes, const QModelIndex &parent, int row)
{
	copyOrMoveLayers(indexes, parent, row, false);
}

void LayerModel::copyOrMoveLayers(const QModelIndexList &indexes, const QModelIndex &parent, int row, bool copy)
{
	if (!checkIndex(parent))
	{
		PAINTFIELD_WARNING << "invalid index";
		return;
	}
	
	auto command = new QUndoCommand(copy ? tr("Copy Layers") : tr("Move Layers"));
	
	LayerPath newParentPath = pathForIndex(parent);
	
	DuplicatedNameResolver nameResolver(childNames(parent));
	
	for (int i = 0; i < indexes.size(); ++i)
	{
		QModelIndex index = indexes.at(i);
		
		if (!checkIndex(index) || !index.isValid())
		{
			PAINTFIELD_WARNING << "invalid index";
			continue;
		}
		
		LayerPath oldPath = pathForIndex(index);
		LayerPath newPath;
		
		if (!copy && oldPath.parentPath() == newParentPath)
			newPath = oldPath;
		else
			newPath = newParentPath.childPath(nameResolver.resolve(oldPath.name()));
		
		if (copy)
			new LayerModelCopyCommand(oldPath, newPath, row + i, this, command);
		else
			new LayerModelMoveCommand(oldPath, newPath, row + i, this, command);
	}
	pushCommand(command);
}

void LayerModel::renameLayer(const QModelIndex &index, const QString &newName)
{
	if (!checkIndex(index))
	{
		PAINTFIELD_WARNING << "invalid index";
		return;
	}
	
	DuplicatedNameResolver nameResolver(childNames(index.parent()));
	auto command = new LayerModelPropertyChangeCommand(pathForIndex(index), nameResolver.resolve(newName), PaintField::RoleName, this);
	command->setText(tr("Rename Layer"));
	pushCommand(command);
}

void LayerModel::mergeLayers(const QModelIndex &parent, int from, int to)
{
	auto parentLayer = layerForIndex(parent);
	{
		if (!checkLayer(parentLayer))
		{
			PAINTFIELD_WARNING << ": invalid parent";
			return;
		}
		if (from > to || !parentLayer->contains(from) || !parentLayer->contains(to))
		{
			PAINTFIELD_WARNING << ": invalid row";
			return;
		}
	}
	
	DuplicatedNameResolver nameResolver(parentLayer->childNames());
	
	auto command = new LayerModelMergeCommand(pathForIndex(parent), from, to - from + 1, nameResolver.resolve(tr("Merged Layer")), this);
	command->setText(tr("Merge Layers"));
	
	pushCommand(command);
}

/*
QMap<int, QVariant> LayerModel::itemData(const QModelIndex &index) const
{
	auto layer = layerForIndex(index);
	
	QMap<int, QVariant> map;
	map[Qt::EditRole] = layer->property(PaintField::RoleName);
	map[Qt::DecorationRole] = layer->property(PaintField::RoleThumbnail);
	map[Qt::CheckStateRole] = layer->property(PaintField::RoleVisible);
	
	return map;
}*/

QVariant LayerModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();
	
	auto value = layerForIndex(index)->property(normalizeItemRole(role));
	
	if (role == Qt::CheckStateRole)
		return value.toBool() ? Qt::Checked : Qt::Unchecked;
	
	return value;
}

bool LayerModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	return setData(index, value, role, QString());
}

bool LayerModel::setData(const QModelIndex &index, const QVariant &value, int role, const QString &description)
{
	QString text = description;
	
	if (!index.isValid())
		return false;
	
	role = normalizeItemRole(role);
	
	// set description text if possible
	switch (role)
	{
		case PaintField::RoleName:
			if (description.isNull())
				text = tr("Rename Layer");
			break;
		case PaintField::RoleVisible:
			if (description.isNull())
				text = tr("Change visibility");
			break;
		case PaintField::RoleBlendMode:
			if (description.isNull())
				text = tr("Change Blend Mode");
			break;
		case PaintField::RoleOpacity:
			if (description.isNull())
				text = tr("Change Opacity");
			break;
		default:
			break;
	}
	
	if (data(index, role) == value)
		return true;
	
	if (role == PaintField::RoleName)
		renameLayer(index, value.toString());
	else
		editLayer(index, new LayerPropertyEdit(value, role), text);
	 
	return true;
}

Qt::ItemFlags LayerModel::flags(const QModelIndex &index) const
{
	Qt::ItemFlags layerFlags = Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable;
	Qt::ItemFlags groupFlags = layerFlags | Qt::ItemIsDropEnabled;
	
	if (layerForIndex(index)->type() == Layer::TypeGroup)
		return groupFlags;
	else
		return layerFlags;
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
	
	for (const QModelIndex &index : indexes)
		stream << reinterpret_cast<quint64>(index.internalPointer());
	
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
	
	while (!stream.atEnd())
	{
		quint64 p;
		stream >> p;
		indexes << indexForLayer(reinterpret_cast<Layer *>(p));
	}
	
	switch (action)
	{
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
		layer = static_cast<const Layer *>(index.internalPointer());
		
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
	for (const QModelIndex &index : indexes)
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
		PAINTFIELD_WARNING << "invalid layer";
		return QModelIndex();
	}
	return layer != rootLayer() ? createIndex(layer->row(), 0, const_cast<Layer *>(layer) ) : QModelIndex();
}

const Layer *LayerModel::layerForPath(const LayerPath &path) const
{
	const Layer *layer = rootLayer();
	for (const QString &name : path)
	{
		layer = layer->child(name);
		if (!layer)
			return 0;
	}
	return layer;
}

LayerPath LayerModel::pathForLayer(const Layer *layer) const
{
	if (!checkLayer(layer))
	{
		PAINTFIELD_WARNING << "invalid layer";
		return LayerPath();
	}
	
	LayerPath path;
	while (layer != rootLayer())
	{
		path.prepend(layer->name());
		layer = layer->parent();
	}
	return path;
}

LayerPathList LayerModel::pathsForIndexes(const QModelIndexList &indexes) const
{
	LayerPathList paths;
	
	for (const QModelIndex &index : indexes)
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

int LayerModel::normalizeItemRole(int role) const
{
	switch (role)
	{
		case Qt::EditRole: case Qt::DisplayRole:
			return PaintField::RoleName;
		case Qt::DecorationRole:
			return PaintField::RoleThumbnail;
		case Qt::CheckStateRole:
			return PaintField::RoleVisible;
		default:
			return role;
	}
}

void LayerModel::pushCommand(QUndoCommand *command)
{
	_document->undoStack()->push(command);
}

void LayerModel::update()
{
	if (_skipNextUpdate)
		_skipNextUpdate = false;
	else
		emit tilesUpdated(_updatedTiles);
	
	_updatedTiles.clear();
}

}

