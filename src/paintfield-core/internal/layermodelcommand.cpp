#include "../layerrenderer.h"
#include "../document.h"

#include "layermodelcommand.h"

namespace PaintField
{

void LayerModelCommand::insertLayer(Layer *parent, int row, Layer *layer)
{
	beginInsertLayers(parent, row, row);
	parent->insertChild(row, layer);
	endInsertLayers();
}

Layer *LayerModelCommand::takeLayer(Layer *parent, int row)
{
	beginRemoveLayers(parent, row, row);
	auto layer = parent->takeChild(row);
	endRemoveLayers();
	return layer;
}

void LayerModelCommand::emitDataChanged(Layer *layer)
{
	auto index = _model->indexForLayer(layer);
	_model->emitDataChanged(index, index);
}

void LayerModelCommand::emitDataChanged(Layer *parent, int start, int end)
{
	if (start > end)
		qSwap(start, end);
	
	auto startIndex = _model->indexForLayer(parent->child(start));
	auto endIndex = _model->indexForLayer(parent->child(end));
	_model->emitDataChanged(startIndex, endIndex);
}

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


void LayerModelAddCommand::redo()
{
	insertLayer(layerForPath(_parentPath), _row, _layer.take());
}

void LayerModelAddCommand::undo()
{
	_layer.reset(takeLayer(layerForPath(_parentPath), _row));
}

LayerModelMultipleAddCommand::LayerModelMultipleAddCommand(const LayerList &layers, const LayerPath &parentPath, int row, LayerModel *model, QUndoCommand *parent) :
	QUndoCommand(parent)
{
	for (Layer *layer : layers)
		new LayerModelAddCommand(layer, parentPath, row++, model, this);
}

void LayerModelRemoveCommand::redo()
{
	_layer.reset(takeLayer(layerForPath(_parentPath), _row));
}

void LayerModelRemoveCommand::undo()
{
	insertLayer(layerForPath(_parentPath), _row, _layer.take());
}

LayerModelMultipleRemoveCommand::LayerModelMultipleRemoveCommand(const LayerPathList &paths, LayerModel *model, QUndoCommand *parent) :
	QUndoCommand(parent)
{
	LayerPathList sorted = LayerPath::sortLayerPathList(paths);
	
	for (const LayerPath &path : Malachite::reverseContainer(sorted))
		new LayerModelRemoveCommand(path.parentPath(), path.row(), model, this);
}

void LayerModelCopyCommand::redo()
{
	auto layer = layerForPath(_oldParentPath)->child(_oldRow)->cloneRecursive();
	insertLayer(layerForPath(_newParentPath), _newRow, layer);
}

void LayerModelCopyCommand::undo()
{
	delete takeLayer(layerForPath(_newParentPath), _newRow);
}

LayerModelMultipleCopyCommand::LayerModelMultipleCopyCommand(const LayerPathList &paths, const LayerPath &newParentPath, int newRow, LayerModel *model, QUndoCommand *parent) :
	QUndoCommand(parent)
{
	LayerPathList sorted = LayerPath::sortLayerPathList(paths);
	
	int i = 0;
	for (LayerPath path : Malachite::reverseContainer(sorted))
	{
		if (path.isAncestorOf(newParentPath) && path.at(newParentPath.size()) > newRow)
			path[newParentPath.size()] += i;
		
		new LayerModelCopyCommand(path.parentPath(), path.row(), newParentPath, newRow, model, this);
		i++;
	}
}

void LayerModelMoveCommand::redo()
{
	move(layerForPath(_oldParentPath), _oldRow, layerForPath(_newParentPath), _newRow);
}

void LayerModelMoveCommand::undo()
{
	move(layerForPath(_newParentPath), _newRow, layerForPath(_oldParentPath), _oldRow);
}

void LayerModelMoveCommand::move(Layer *oldParent, int oldRow, Layer *newParent, int newRow)
{
	if (oldParent == newParent)
	{
		if (oldRow != newRow)
		{
			oldParent->shiftChildren(newRow, oldRow, 1);
			emitDataChanged(oldParent, newRow, oldRow);
		}
		return;
	}
	
	auto layer = takeLayer(oldParent, oldRow);
	insertLayer(newParent, newRow, layer);
}

LayerModelMultipleMoveCommand::LayerModelMultipleMoveCommand(const LayerPathList &paths, const LayerPath &newParentPath, int newRow, LayerModel *model, QUndoCommand *parent) :
	QUndoCommand(parent)
{
	LayerPathList sorted = LayerPath::sortLayerPathList(paths);
	
	int i = 0;
	LayerPath newPath = newParentPath.childPath(newRow);
	for (LayerPath path : Malachite::reverseContainer(sorted))
	{
		if (path.isAncestorOf(newParentPath) && path.at(newParentPath.size()) > newRow)
			path[newParentPath.size()] += i;
		
		new LayerModelMoveCommand(path.parentPath(), path.row(), newPath.parentPath(), newPath.row(), model, this);
		i++;
		
		if (newPath.isAncestorOf(path.parentPath()) && newPath.at(path.parentPath().size()) > path.row())
			newPath[path.parentPath().size()]++;
	}
}

LayerModelMergeCommand::~LayerModelMergeCommand()
{
	qDeleteAll(_oldLayers);
}

void LayerModelMergeCommand::redo()
{
	Layer *parent = layerForPath(_parentPath);
	
	for (int i = 0; i < _count; ++i)
		_oldLayers << takeLayer(parent, _row);
	
	LayerRenderer renderer;
	
	RasterLayer *newLayer = new RasterLayer(QObject::tr("Merged Layer"));
	newLayer->setSurface(renderer.renderToSurface(Malachite::constList(_oldLayers)));
	
	insertLayer(parent, _row, newLayer);
}

void LayerModelMergeCommand::undo()
{
	Layer *parent = layerForPath(_parentPath);
	
	delete takeLayer(parent, _row);
	
	for (int i = 0; i < _count; ++i)
		insertLayer(parent, _row, _oldLayers.takeLast());
}

}

