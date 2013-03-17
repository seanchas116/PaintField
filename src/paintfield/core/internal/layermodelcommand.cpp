#include "../layerrenderer.h"
#include "../document.h"
#include "../rasterlayer.h"

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
	layer->setThumbnailDirty(true);
	
	if (_edit->modifiedKeys().isEmpty())
		enqueueTileUpdate(model()->document()->tileKeys());
	else
		enqueueTileUpdate(_edit->modifiedKeys());
}

void LayerModelEditCommand::undo()
{
	Layer *layer = layerForPath(_path);
	_edit->undo(layer);
	layer->setThumbnailDirty(true);
	
	if (_edit->modifiedKeys().isEmpty())
		enqueueTileUpdate(model()->document()->tileKeys());
	else
		enqueueTileUpdate(_edit->modifiedKeys());
}


LayerModelAddCommand::LayerModelAddCommand(Layer *layer, const LayerPath &newParentPath, int newRow, LayerModel *document, QUndoCommand *parent) :
    LayerModelCommand(document, parent),
	_layer(layer),
    _newParentPath(newParentPath),
    _newRow(newRow)
{
	Q_ASSERT(layer);
	layer->updateThumbnailRecursive(model()->document()->size());
}

void LayerModelAddCommand::redo()
{
	auto parent = layerForPath(_newParentPath);
	enqueueTileUpdate(_layer->tileKeysRecursive());
	insertLayer(parent, _newRow, _layer.take());
}

void LayerModelAddCommand::undo()
{
	auto parent = layerForPath(_newParentPath);
	_layer.reset(takeLayer(parent, _newRow));
	enqueueTileUpdate(_layer->tileKeysRecursive());
}


void LayerModelRemoveCommand::redo()
{
	_layer.reset(layerForPath(_path));
	_row = _layer->index();
	
	takeLayer(_layer->parent(), _row);
	
	enqueueTileUpdate(_layer->tileKeysRecursive());
}

void LayerModelRemoveCommand::undo()
{
	enqueueTileUpdate(_layer->tileKeysRecursive());
	
	insertLayer(layerForPath(_path.parentPath()), _row, _layer.take());
}


LayerModelMoveCommand::LayerModelMoveCommand(const LayerPath &oldPath, const LayerPath &newPath, int newRow, LayerModel *document, QUndoCommand *parent) :
	LayerModelCommand(document, parent),
	_newParentPath(newPath.parentPath()),
	_oldParentPath(oldPath.parentPath()),
	_newRow(newRow),
	_newName(newPath.name()),
	_oldName(oldPath.name())
{
	_oldRow = layerForPath(oldPath)->index();
	
	if (_newParentPath == _oldParentPath && _newRow > _oldRow)
		_newRow--;
}

void LayerModelMoveCommand::redo()
{
	if (noMove())
		return;
	
	auto layer = takeLayer(layerForPath(_oldParentPath), _oldRow);
	
	layer->setName(_newName);
	
	insertLayer(layerForPath(_newParentPath), _newRow, layer);
	
	enqueueTileUpdate(layer->tileKeysRecursive());
}

void LayerModelMoveCommand::undo()
{
	if (noMove())
		return;
	
	auto layer = takeLayer(layerForPath(_newParentPath), _newRow);
	
	layer->setName(_oldName);
	
	insertLayer(layerForPath(_oldParentPath), _oldRow, layer);
	
	enqueueTileUpdate(layer->tileKeysRecursive());
}

void LayerModelCopyCommand::redo()
{
	auto parent = layerForPath(_newParentPath);
	auto clone = layerForPath(_path)->cloneRecursive();
	
	clone->setName(_newName);
	
	insertLayer(parent, _newRow, clone);
	
	enqueueTileUpdate(clone->tileKeysRecursive());
}

void LayerModelCopyCommand::undo()
{
	auto parent = layerForPath(_newParentPath);
	
	QScopedPointer<Layer> layer(takeLayer(parent, _newRow));
	
	enqueueTileUpdate(layer->tileKeysRecursive());
}


LayerModelMergeCommand::LayerModelMergeCommand(const LayerPath &parentPath, int row, int count, const QString &name, LayerModel *model, QUndoCommand *parent) :
	LayerModelCommand(model, parent),
	_parentPath(parentPath),
	_row(row),
	_count(count),
	_name(name),
	_group(new GroupLayer())
{}

void LayerModelMergeCommand::redo()
{
	Layer *parent = layerForPath(_parentPath);
	
	for (int i = 0; i < _count; ++i)
		_group->append(takeLayer(parent, _row));
	
	LayerRenderer renderer;
	
	auto newLayer = new RasterLayer(_name);
	newLayer->setSurface(renderer.renderToSurface(_group.data()));
	newLayer->updateThumbnail(model()->document()->size());
	
	insertLayer(parent, _row, newLayer);
}

void LayerModelMergeCommand::undo()
{
	Layer *parent = layerForPath(_parentPath);
	
	delete takeLayer(parent, _row);
	
	for (int i = 0; i < _count; ++i)
		insertLayer(parent, _row + i, _group->take(0));
}

}

