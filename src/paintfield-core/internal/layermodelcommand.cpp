#include "../layerrenderer.h"
#include "../document.h"

#include "layermodelcommand.h"

namespace PaintField
{

LayerModelEditCommand::LayerModelEditCommand(const LayerPath &path, LayerEdit *edit, LayerModel *document, QUndoCommand *parent) :
    LayerModelCommand(document, parent),
    _path(path),
    _edit(edit)
{
	const Layer *layer = document->layerForPath(path);
	Q_ASSERT(layer);
	Q_ASSERT(edit);
	setText(edit->name());
	edit->saveUndoState(layer);
}

void LayerModelEditCommand::redo()
{
	Layer *layer = layerForPath(_path);
	_edit->redo(layer);
	//layer->updateThumbnail(document()->size());
	layer->setThumbnailDirty(true);
	emitDataChanged(layer);
	
	if (_edit->modifiedKeys().isEmpty())
		enqueueTileUpdate(model()->document()->tileKeys());
	else
		enqueueTileUpdate(_edit->modifiedKeys());
}

void LayerModelEditCommand::undo()
{
	Layer *layer = layerForPath(_path);
	_edit->undo(layer);
	//layer->updateThumbnail(document()->size());
	layer->setThumbnailDirty(true);
	emitDataChanged(layer);
	
	if (_edit->modifiedKeys().isEmpty())
		enqueueTileUpdate(model()->document()->tileKeys());
	else
		enqueueTileUpdate(_edit->modifiedKeys());
}

LayerModelAddCommand::LayerModelAddCommand(const LayerList &layers, const LayerPath &parentPath, int row, LayerModel *model, QUndoCommand *parent) :
    LayerModelCommand(model, parent),
    _hasLayers(true),
    _layers(layers),
    _parentPath(parentPath),
    _row(row)
{
}

LayerModelAddCommand::~LayerModelAddCommand()
{
	if (_hasLayers)
		qDeleteAll(_layers);
}

void LayerModelAddCommand::redo()
{
	Layer *parent = layerForPath(_parentPath);
	int count = _layers.size();
	
	beginInsertLayers(parent, _row, _row + count - 1);
	
	for (int i = 0; i < count; ++i)
		parent->insertChild(_row + i, _layers.at(i));
	
	endInsertLayers();
	
	_hasLayers = false;
}

void LayerModelAddCommand::undo()
{
	Layer *parent = layerForPath(_parentPath);
	int count = _layers.size();
	
	beginRemoveLayers(parent, _row, _row + count - 1);
	
	for (int i = 0; i < count; ++i)
		parent->takeChild(_row);
	
	endRemoveLayers();
	
	_hasLayers = true;
}



LayerModelRemoveCommand::LayerModelRemoveCommand(const LayerPathList &paths, LayerModel *model, QUndoCommand *parent) :
    LayerModelCommand(model, parent)
{
	_paths = sortPathList(paths);
}

LayerModelRemoveCommand::~LayerModelRemoveCommand()
{
	qDeleteAll(_layers);
}

void LayerModelRemoveCommand::redo()
{
	QListIterator<LayerPath> iter(_paths);
	iter.toBack();
	
	while (iter.hasPrevious())
	{
		Layer *layer = layerForPath(iter.previous());
		Layer *parent = layer->parent();
		int row = layer->row();
		
		beginRemoveLayers(parent, row, row);
		
		_layers.prepend(parent->takeChild(row));
		
		endRemoveLayers();
	}
}

void LayerModelRemoveCommand::undo()
{
	QListIterator<LayerPath> iter(_paths);
	iter.toBack();
	
	while (iter.hasPrevious())
	{
		LayerPath path = iter.previous();
		int row = path.last();
		LayerPath parentPath = path;
		parentPath.removeLast();
		
		Layer *parent = layerForPath(parentPath);
		
		parent->insertChild(row, _layers.takeFirst());
	}
}

LayerModelCopyCommand::LayerModelCopyCommand(const LayerPathList &paths, const LayerPath &parentPath, int row, LayerModel *model, QUndoCommand *parent) :
    LayerModelCommand(model, parent),
    _parentPath(parentPath),
    _row(row)
{
	_paths = sortPathList(paths);
}

void LayerModelCopyCommand::redo()
{
	LayerList layers;
	
	foreach (const LayerPath &path, _paths)
		layers << layerForPath(path)->cloneRecursive();
	
	Layer *parent = layerForPath(_parentPath);
	
	beginInsertLayers(parent, _row, _row + _paths.size() - 1);
	
	foreach (Layer *layer, layers)
		parent->insertChild(_row, layer);
	
	endInsertLayers();
}

void LayerModelCopyCommand::undo()
{
	Layer *parent = layerForPath(_parentPath);
	
	beginRemoveLayers(parent, _row, _row + _paths.size() - 1);
	
	for (int i = 0; i < _paths.size(); ++i)
		parent->removeChild(_row + i);
	
	endRemoveLayers();
}

LayerModelMoveCommand::LayerModelMoveCommand(const LayerPathList &paths, const LayerPath &parentPath, int row, LayerModel *model, QUndoCommand *parent) :
    LayerModelCommand(model, parent),
    _dstParentPath(parentPath),
    _dstRow(row)
{
	_paths = sortPathList(paths);
	
	foreach (const LayerPath &path, _paths)
	{
		LayerPath srcParentPath = path;
		srcParentPath.removeLast();
		
		if (srcParentPath == _dstParentPath && path.last() < row)
			_dstRow--;
	}
}

void LayerModelMoveCommand::redo()
{
	int count = _paths.size();
	
	Layer *dstParent = layerForPath(_dstParentPath);
	
	LayerList layers;
	
	foreach (const LayerPath &path, _paths)
	{
		Layer *layer = layerForPath(path);
		Layer *parent = layer->parent();
		int row = layer->row();
		
		beginRemoveLayers(parent, row, row);
		layers << parent->takeChild(row);
		endRemoveLayers();
	}
	
	beginInsertLayers(dstParent, _dstRow, _dstRow + count - 1);
	
	for (int i = 0; i < count; ++i)
		dstParent->insertChild(_dstRow + i, layers.at(i));
	
	endInsertLayers();
}

void LayerModelMoveCommand::undo()
{
	int count = _paths.size();
	
	Layer *dstParent = layerForPath(_dstParentPath);
	
	LayerList layers;
	
	beginRemoveLayers(dstParent, _dstRow, _dstRow + count - 1);
	
	for (int i = 0; i < count; ++i)
		layers << dstParent->takeChild(_dstRow);
	
	endRemoveLayers();
	
	for (int i = 0; i < count; ++i)
	{
		LayerPath pathParent = _paths.at(i);
		pathParent.removeLast();
		int row = _paths.at(i).last();
		
		Layer *parent = layerForPath(pathParent);
		beginInsertLayers(parent, row, row);
		parent->insertChild(row, layers.at(i));
		endInsertLayers();
	}
}

LayerModelMergeCommand::LayerModelMergeCommand(const LayerPath &parentPath, int row, int count, LayerModel *model, QUndoCommand *parent) :
    LayerModelCommand(model, parent),
    _parentPath(parentPath),
    _row(row),
    _count(count)
{
}

LayerModelMergeCommand::~LayerModelMergeCommand()
{
	qDeleteAll(_oldLayers);
}

void LayerModelMergeCommand::redo()
{
	Layer *parent = layerForPath(_parentPath);
	
	beginRemoveLayers(parent, _row, _row + _count - 1);
	
	for (int i = 0; i < _count; ++i)
	{
		Layer *layer = parent->takeChild(_row);
		_oldLayers << layer;
	}
	
	endRemoveLayers();
	
	LayerRenderer renderer;
	
	RasterLayer *newLayer = new RasterLayer(QObject::tr("Merged Layer"));
	newLayer->setSurface(renderer.render(Malachite::blindCast<LayerConstList>(_oldLayers)));
	
	beginInsertLayers(parent, _row, _row);
	
	parent->insertChild(_row, newLayer);
	
	endInsertLayers();
}

void LayerModelMergeCommand::undo()
{
	Layer *parent = layerForPath(_parentPath);
	
	beginRemoveLayers(parent, _row, _row);
	
	parent->removeChild(_row);
	
	endRemoveLayers();
	
	beginInsertLayers(parent, _row, _row + _count - 1);
	
	for (int i = 0; i < _count; ++i)
	{
		parent->insertChild(_row + i, _oldLayers[i]);
	}
	
	endInsertLayers();
	
	_oldLayers.clear();
}

}

