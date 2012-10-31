#ifndef FSLayerModelCOMMAND_H
#define FSLayerModelCOMMAND_H

#include <QUndoCommand>
#include "layermodel.h"
#include "layeredit.h"

namespace PaintField {

class LayerModelCommand : public QUndoCommand
{
public:
	LayerModelCommand(LayerModel *model, QUndoCommand *parent)
	    : QUndoCommand(parent),
	      _model(model)
	{
		Q_ASSERT(_model);
	}
	
protected:
	
	void beginInsertLayers(Layer *parent, int start, int end)
	{
		Q_ASSERT(parent);
		_model->beginInsertRows(_model->indexForLayer(parent), start, end);
	}
	void endInsertLayers() { _model->endInsertRows(); }
	
	void beginRemoveLayers(Layer *parent, int start, int end)
	{
		Q_ASSERT(parent);
		_model->beginRemoveRows(_model->indexForLayer(parent), start, end);
	}
	void endRemoveLayers() { _model->endRemoveRows(); }
	
	void insertLayer(Layer *parent, int row, Layer *layer);
	Layer *takeLayer(Layer *parent, int row);
	
	void emitDataChanged(Layer *layer);
	void emitDataChanged(Layer *parent, int start, int end);
	
	void enqueueTileUpdate(const QPointSet &tileKeys) { _model->enqueueTileUpdate(tileKeys); }
	
	LayerModel *model() { return _model; }
	
	Layer *layerForPath(const LayerPath &path) { return _model->nonConstLayer(_model->layerForPath(path)); }
	
private:
	LayerModel *_model;
};


class LayerModelEditCommand : public LayerModelCommand
{
public:
	LayerModelEditCommand(const LayerPath &path, LayerEdit *edit, LayerModel *document, QUndoCommand *parent = 0);
	
	void redo();
	void undo();
	
private:
	LayerPath _path;
	QScopedPointer<LayerEdit> _edit;
};

class LayerModelPropertyChangeCommand : public LayerModelEditCommand
{
public:
	
	LayerModelPropertyChangeCommand(const LayerPath &path, const QVariant &property, int role, LayerModel *document, QUndoCommand *parent = 0) :
	    LayerModelEditCommand(path, new FSLayerPropertyEdit(property, role), document, parent)
	{}
};

class LayerModelAddCommand : public LayerModelCommand
{
public:
	
	LayerModelAddCommand(Layer *layer, const LayerPath &parentPath, int row, LayerModel *model, QUndoCommand *parent) :
		LayerModelCommand(model, parent),
		_layer(layer),
		_parentPath(parentPath),
		_row(row)
	{}
	
	void redo();
	void undo();
	
private:
	
	QScopedPointer<Layer> _layer;
	LayerPath _parentPath;
	int _row;
};

class LayerModelMultipleAddCommand : public QUndoCommand
{
public:
	LayerModelMultipleAddCommand(const LayerList &layers, const LayerPath &parentPath, int row, LayerModel *model, QUndoCommand *parent = 0);
};

class LayerModelRemoveCommand : public LayerModelCommand
{
public:
	
	LayerModelRemoveCommand(const LayerPath &parentPath, int row, LayerModel *model, QUndoCommand *parent) :
		LayerModelCommand(model, parent),
		_parentPath(parentPath),
		_row(row)
	{}
	
	void redo();
	void undo();
	
private:
	
	QScopedPointer<Layer> _layer;
	LayerPath _parentPath;
	int _row;
};

class LayerModelMultipleRemoveCommand : public QUndoCommand
{
public:
	LayerModelMultipleRemoveCommand(const LayerPathList &paths, LayerModel *model, QUndoCommand *parent = 0);
};

class LayerModelCopyCommand : public LayerModelCommand
{
public:
	
	LayerModelCopyCommand(const LayerPath &oldParentPath, int oldRow, const LayerPath &newParentPath, int newRow, LayerModel *model, QUndoCommand *parent) :
		LayerModelCommand(model, parent),
		_oldParentPath(oldParentPath),
		_newParentPath(newParentPath),
		_oldRow(oldRow),
		_newRow(newRow)
	{}
	
	void redo();
	void undo();
	
private:
	
	LayerPath _oldParentPath, _newParentPath;
	int _oldRow, _newRow;
};

class LayerModelMultipleCopyCommand : public QUndoCommand
{
public:
	
	LayerModelMultipleCopyCommand(const LayerPathList &paths, const LayerPath &newParentPath, int newRow, LayerModel *model, QUndoCommand *parent = 0);
};

class LayerModelMoveCommand : public LayerModelCommand
{
public:
	
	LayerModelMoveCommand(const LayerPath &oldParentPath, int oldRow, const LayerPath &newParentPath, int newRow, LayerModel *model, QUndoCommand *parent) :
		LayerModelCommand(model, parent),
		_oldParentPath(oldParentPath),
		_newParentPath(newParentPath),
		_oldRow(oldRow),
		_newRow(newRow)
	{}
	
	void redo();
	void undo();
	
	void move(Layer *oldParent, int oldRow, Layer *newParent, int newRow);
	
private:
	
	LayerPath _oldParentPath, _newParentPath;
	int _oldRow, _newRow;
};

class LayerModelMultipleMoveCommand : public QUndoCommand
{
public:
	LayerModelMultipleMoveCommand(const LayerPathList &paths, const LayerPath &newParentPath, int newRow, LayerModel *model, QUndoCommand *parent = 0);
};

class LayerModelMergeCommand : public LayerModelCommand
{
public:
	
	LayerModelMergeCommand(const LayerPath &parentPath, int row, int count, LayerModel *model, QUndoCommand *parent = 0) :
		LayerModelCommand(model, parent),
		_parentPath(parentPath),
		_row(row),
		_count(count)
	{}
	
	~LayerModelMergeCommand();
	
	void redo();
	void undo();
	
private:
	
	LayerPath _parentPath;
	int _row, _count;
	LayerList _oldLayers;
};

}

#endif // FSLayerModelCOMMAND_H
