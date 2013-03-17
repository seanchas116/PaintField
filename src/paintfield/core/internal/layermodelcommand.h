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
	
	void beginInsertLayers(Layer *parent, int start, int end) { Q_ASSERT(parent); _model->beginInsertRows(_model->indexForLayer(parent), start, end); }
	void endInsertLayers() { _model->endInsertRows(); }
	void beginRemoveLayers(Layer *parent, int start, int end) { Q_ASSERT(parent); _model->beginRemoveRows(_model->indexForLayer(parent), start, end); }
	void endRemoveLayers() { _model->endRemoveRows(); }
	
	void insertLayer(Layer *parent, int row, Layer *layer)
	{
		Q_ASSERT(parent);
		beginInsertLayers(parent, row, row);
		parent->insertChild(row, layer);
		endInsertLayers();
	}
	
	Layer *takeLayer(Layer *parent, int row)
	{
		Q_ASSERT(parent);
		beginRemoveLayers(parent, row, row);
		auto layer = parent->takeChild(row);
		endRemoveLayers();
		return layer;
	}
	
	void emitDataChanged(Layer *layer)
	{
		Q_ASSERT(layer);
		QModelIndex index = _model->indexForLayer(layer);
		_model->emitDataChanged(index, index);
	}
	
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
	    LayerModelEditCommand(path, new LayerPropertyEdit(property, role), document, parent)
	{}
};


class LayerModelAddCommand : public LayerModelCommand
{
public:
	LayerModelAddCommand(Layer *layer, const LayerPath &newParentPath, int newRow, LayerModel *model, QUndoCommand *parent = 0);
	
	void redo();
	void undo();
	
private:
	QScopedPointer<Layer> _layer;
	LayerPath _newParentPath;
	int _newRow;
};

class LayerModelRemoveCommand : public LayerModelCommand
{
public:
	LayerModelRemoveCommand(const LayerPath &path, LayerModel *document, QUndoCommand *parent) :
		LayerModelCommand(document, parent),
		_path(path)
	{}
	
	void redo();
	void undo();
	
private:
	LayerPath _path;
	QScopedPointer<Layer> _layer;
	int _row;
};


class LayerModelMoveCommand : public LayerModelCommand
{
public:
	LayerModelMoveCommand(const LayerPath &oldPath, const LayerPath &newPath, int newRow, LayerModel *model, QUndoCommand *parent);
	
	void redo();
	void undo();
	
private:
	
	bool noMove() const { return _newParentPath == _oldParentPath && _newRow == _oldRow; }
	
	LayerPath _newParentPath, _oldParentPath;
	int _newRow, _oldRow;
	QString _newName, _oldName;
};


class LayerModelCopyCommand : public LayerModelCommand
{
public:
	LayerModelCopyCommand(const LayerPath &path, const LayerPath &newPath, int newRow, LayerModel *model, QUndoCommand *parent = 0) :
		LayerModelCommand(model, parent),
		_path(path),
		_newParentPath(newPath.parentPath()),
		_newName(newPath.name()),
		_newRow(newRow)
	{}
	
	void redo();
	void undo();
	
private:
	LayerPath _path;
	LayerPath _newParentPath;
	QString _newName;
	int _newRow;
};

class LayerModelMergeCommand : public LayerModelCommand
{
public:
	
	LayerModelMergeCommand(const LayerPath &parentPath, int row, int count, const QString &name, LayerModel *model, QUndoCommand *parent = 0);
	
	void redo();
	void undo();
	
private:
	
	LayerPath _parentPath;
	int _row, _count;
	QString _name;
	QScopedPointer<GroupLayer> _group;
};

}

#endif // FSLayerModelCOMMAND_H
