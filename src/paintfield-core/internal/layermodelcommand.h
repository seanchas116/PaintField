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
	
	void emitDataChanged(Layer *layer)
	{
		Q_ASSERT(layer);
		QModelIndex index = _model->indexForLayer(layer);
		_model->emitDataChanged(index, index);
	}
	
	void enqueueTileUpdate(const QPointSet &tileKeys) { _model->enqueueTileUpdate(tileKeys); }
	
	LayerModel *model() { return _model; }
	
	Layer *layerForPath(const LayerPath &path) { return _model->nonConstLayer(_model->layerForPath(path)); }
	
	LayerPath parentPath(const LayerPath &path)
	{
		Q_ASSERT(path.size());
		
		LayerPath result = path;
		result.removeLast();
		return result;
	}
	
	LayerPathList sortPathList(const LayerPathList &paths)
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
	
	LayerModelAddCommand(const LayerList &layers, const LayerPath &parentPath, int row, LayerModel *model, QUndoCommand *parent = 0);
	~LayerModelAddCommand();
	
	void redo();
	void undo();
	
private:
	
	bool _hasLayers;
	LayerList _layers;
	LayerPath _parentPath;
	int _row;
};

class LayerModelRemoveCommand : public LayerModelCommand
{
public:
	
	LayerModelRemoveCommand(const LayerPathList &paths, LayerModel *model, QUndoCommand *parent = 0);
	~LayerModelRemoveCommand();
	
	void redo();
	void undo();
	
private:
	
	LayerPathList _paths;
	LayerList _layers;
};

class LayerModelCopyCommand : public LayerModelCommand
{
public:
	
	LayerModelCopyCommand(const LayerPathList &paths, const LayerPath &parentPath, int row, LayerModel *model, QUndoCommand *parent = 0);
	
	void redo();
	void undo();
	
private:
	
	LayerPathList _paths;
	LayerPath _parentPath;
	int _row;
};

class LayerModelMoveCommand : public LayerModelCommand
{
public:
	
	LayerModelMoveCommand(const LayerPathList &paths, const LayerPath &parentPath, int row, LayerModel *model, QUndoCommand *parent = 0);
	
	void redo();
	void undo();
	
private:
	
	LayerPathList _paths;
	LayerPath _dstParentPath;
	int _dstRow;
};

class LayerModelMergeCommand : public LayerModelCommand
{
public:
	
	LayerModelMergeCommand(const LayerPath &parentPath, int row, int count, LayerModel *model, QUndoCommand *parent = 0);
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
