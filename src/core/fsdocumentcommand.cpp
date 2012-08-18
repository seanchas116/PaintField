#include "fsdocumentcommand.h"

void FSDocumentEditCommand::redo()
{
	FSLayer *layer = layerForPath(_path);
	_edit->redo(layer);
	//layer->updateThumbnail(document()->size());
	layer->setThumbnailDirty(true);
	emitDataChanged(layer);
	
	if (_edit->modifiedKeys().isEmpty())
		enqueueTileUpdate(document()->tileKeys());
	else
		enqueueTileUpdate(_edit->modifiedKeys());
}

void FSDocumentEditCommand::undo()
{
	FSLayer *layer = layerForPath(_path);
	_edit->undo(layer);
	//layer->updateThumbnail(document()->size());
	layer->setThumbnailDirty(true);
	emitDataChanged(layer);
	
	if (_edit->modifiedKeys().isEmpty())
		enqueueTileUpdate(document()->tileKeys());
	else
		enqueueTileUpdate(_edit->modifiedKeys());
}


void FSDocumentAddCommand::redo()
{
	FSLayer *parent = layerForPath(_newParentPath);
	FSLayer *newLayer = _layer->clone();
	
	beginInsertLayers(parent, _newRow, _newRow);
	parent->insertChild(_newRow, newLayer);
	newLayer->updateThumbnail(document()->size());
	endInsertLayers();
	
	enqueueTileUpdate(_layer->surface().keys());
}

void FSDocumentAddCommand::undo()
{
	FSLayer *parent = layerForPath(_newParentPath);
	
	beginRemoveLayers(parent, _newRow, _newRow);
	delete parent->takeChild(_newRow);
	endRemoveLayers();
	
	enqueueTileUpdate(_layer->tileKeysRecursive());
}


void FSDocumentRemoveCommand::redo()
{
	_layer.reset(layerForPath(_path));
	_row = _layer->row();
	FSLayer *parent = _layer->parent();
	
	beginRemoveLayers(parent, _row, _row);
	parent->takeChild(_row);
	endRemoveLayers();
	
	enqueueTileUpdate(_layer->tileKeysRecursive());
}

void FSDocumentRemoveCommand::undo()
{
	FSLayerPath parentPath = _path;
	parentPath.removeLast();
	FSLayer *parent = layerForPath(parentPath);
	
	FSLayer *layer = _layer.take();
	
	beginInsertLayers(parent, _row, _row);
	parent->insertChild(_row, layer);
	endInsertLayers();
	
	enqueueTileUpdate(layer->tileKeysRecursive());
}


void FSDocumentMoveCommand::redo()
{
	//if (_newParentPath == _oldParentPath && _newRow == _oldRow)
	//	return;
	
	FSLayer *oldParent = layerForPath(_oldParentPath);
	FSLayer *newParent = layerForPath(_newParentPath);
	
	beginRemoveLayers(oldParent, _oldRow, _oldRow);
	FSLayer *layer = oldParent->takeChild(_oldRow);
	endRemoveLayers();
	
	layer->setName(_newName);
	
	beginInsertLayers(newParent, _newRow, _newRow);
	newParent->insertChild(_newRow, layer);
	endInsertLayers();
	
	enqueueTileUpdate(layer->tileKeysRecursive());
}

void FSDocumentMoveCommand::undo()
{
	if (_newParentPath == _oldParentPath && _newRow == _oldRow)
		return;
	
	FSLayer *oldParent = layerForPath(_oldParentPath);
	FSLayer *newParent = layerForPath(_newParentPath);
	
	beginRemoveLayers(newParent, _newRow, _newRow);
	FSLayer *layer = newParent->takeChild(_newRow);
	endRemoveLayers();
	
	layer->setName(_oldName);
	
	beginInsertLayers(oldParent, _oldRow, _oldRow);
	oldParent->insertChild(_oldRow, layer);
	endInsertLayers();
	
	enqueueTileUpdate(layer->tileKeysRecursive());
}


void FSDocumentCopyCommand::redo()
{
	FSLayer *layer = layerForPath(_path);
	FSLayer *parent = layerForPath(_newParentPath);
	FSLayer *clone = layer->cloneRecursive();
	clone->setName(_newName);
	beginInsertLayers(parent, _newRow, _newRow);
	parent->insertChild(_newRow, clone);
	endInsertLayers();
	enqueueTileUpdate(clone->tileKeysRecursive());
}

void FSDocumentCopyCommand::undo()
{
	FSLayer *parent = layerForPath(_newParentPath);
	beginRemoveLayers(parent, _newRow, _newRow);
	FSLayer *layer = parent->takeChild(_newRow);
	endRemoveLayers();
	enqueueTileUpdate(layer->tileKeysRecursive());
	delete layer;
}
