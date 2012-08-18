#ifndef FSDOCUMENTCOMMAND_H
#define FSDOCUMENTCOMMAND_H

#include <QUndoCommand>
#include "fsdocumentmodel.h"
#include "fslayeredit.h"

class FSDocumentCommand : public QUndoCommand
{
public:
	FSDocumentCommand(FSDocumentModel *model, QUndoCommand *parent)
	    : QUndoCommand(parent),
	      _model(model)
	{
		Q_ASSERT(_model);
	}
	
protected:
	
	void beginInsertLayers(FSLayer *parent, int start, int end) { Q_ASSERT(parent); _model->beginInsertRows(_model->indexForLayer(parent), start, end); }
	void endInsertLayers() { _model->endInsertRows(); }
	void beginRemoveLayers(FSLayer *parent, int start, int end) { Q_ASSERT(parent); _model->beginRemoveRows(_model->indexForLayer(parent), start, end); }
	void endRemoveLayers() { _model->endRemoveRows(); }
	
	void emitDataChanged(FSLayer *layer)
	{
		Q_ASSERT(layer);
		QModelIndex index = _model->indexForLayer(layer);
		_model->emitDataChanged(index, index);
	}
	
	void enqueueTileUpdate(const QPointSet &tileKeys) { _model->enqueueTileUpdate(tileKeys); }
	
	FSDocumentModel *document() { return _model; }
	
	FSLayer *layerForPath(const FSLayerPath &path) { return _model->nonConstLayer(_model->layerForPath(path)); }
	
	FSLayerPath parentPath(const FSLayerPath &path)
	{
		Q_ASSERT(path.size());
		
		FSLayerPath result = path;
		result.removeLast();
		return result;
	}
	
private:
	FSDocumentModel *_model;
};


class FSDocumentEditCommand : public FSDocumentCommand
{
public:
	FSDocumentEditCommand(const FSLayerPath &path, FSLayerEdit *edit, FSDocumentModel *document, QUndoCommand *parent = 0) :
		FSDocumentCommand(document, parent),
		_path(path),
		_edit(edit)
	{
		const FSLayer *layer = document->layerForPath(path);
		Q_ASSERT(layer);
		Q_ASSERT(edit);
		setText(edit->name());
		edit->saveUndoState(layer);
	}
	
	void redo();
	void undo();
	
private:
	FSLayerPath _path;
	QScopedPointer<FSLayerEdit> _edit;
};


class FSDocumentAddCommand : public FSDocumentCommand
{
public:
	FSDocumentAddCommand(const FSLayer *layer, const FSLayerPath &newParentPath, int newRow, FSDocumentModel *document, QUndoCommand *parent = 0) : 
		FSDocumentCommand(document, parent),
		_newParentPath(newParentPath),
		_newRow(newRow)
	{
		Q_ASSERT(layer);
		_layer.reset(layer->clone());
	}
	
	void redo();
	void undo();
	
private:
	QScopedPointer<FSLayer> _layer;
	FSLayerPath _newParentPath;
	int _newRow;
};

class FSDocumentRemoveCommand : public FSDocumentCommand
{
public:
	FSDocumentRemoveCommand(const FSLayerPath &path, FSDocumentModel *document, QUndoCommand *parent) :
		FSDocumentCommand(document, parent),
		_path(path)
	{}
	
	void redo();
	void undo();
	
private:
	FSLayerPath _path;
	QScopedPointer<FSLayer> _layer;
	int _row;
};


class FSDocumentMoveCommand : public FSDocumentCommand
{
public:
	FSDocumentMoveCommand(const FSLayerPath &oldPath, const FSLayerPath &newPath, int newRow, FSDocumentModel *document, QUndoCommand *parent) :
		FSDocumentCommand(document, parent),
		_newRow(newRow)
	{
		FSLayer *layer = layerForPath(oldPath);
		_oldRow = layer->row();
		
		_oldParentPath = oldPath;
		_oldParentPath.removeLast();
		_oldName = oldPath.last();
		
		_newParentPath = newPath;
		_newParentPath.removeLast();
		_newName = newPath.last();
		
		if (_newParentPath == _oldParentPath && _newRow > _oldRow)
			_newRow--;
	}
	
	void redo();
	void undo();
	
private:
	FSLayerPath _newParentPath, _oldParentPath;
	int _newRow, _oldRow;
	QString _newName, _oldName;
};


class FSDocumentCopyCommand : public FSDocumentCommand
{
public:
	FSDocumentCopyCommand(const FSLayerPath &path, const FSLayerPath &newPath, int newRow, FSDocumentModel *document, QUndoCommand *parent = 0) :
		FSDocumentCommand(document, parent),
		_path(path),
		_newRow(newRow)
	{
		_newParentPath = newPath;
		_newParentPath.removeLast();
		_newName = newPath.last();
	}
	
	void redo();
	void undo();
	
private:
	FSLayerPath _path;
	FSLayerPath _newParentPath;
	QString _newName;
	int _newRow;
};


#endif // FSDOCUMENTCOMMAND_H
