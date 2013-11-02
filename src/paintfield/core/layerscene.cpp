#include <functional>
#include <tuple>
#include <QUndoCommand>
#include <QTimer>
#include <QItemSelectionModel>

#include "document.h"
#include "rasterlayer.h"
#include "grouplayer.h"
#include "layeredit.h"
#include "layerrenderer.h"
#include "layeritemmodel.h"

#include "layerscene.h"

using namespace std;
using namespace Malachite;

namespace PaintField {

class LayerSceneCommand : public QUndoCommand
{
public:
	
	typedef QList<int> Path;
	
	static void sortPathList(QList<Path> &pathList)
	{
		auto compare = [](const Path &p1, const Path &p2)
		{
			return std::lexicographical_compare(p1.begin(), p1.end(), p2.begin(), p2.end());
		};
		
		std::sort(pathList.begin(), pathList.end(), compare);
		
		// this does't work in gcc 4.8
		//boost::sort(pathList, boost::lexicographical_compare<Path, Path>);
	}
	
	LayerSceneCommand(LayerScene *scene, QUndoCommand *parent) :
		QUndoCommand(parent),
		_scene(scene)
	{}
	
	void insertLayer(const LayerRef &parent, int index, const LayerRef &layer)
	{
		PAINTFIELD_DEBUG << parent << index << layer;
		
		emit _scene->layerAboutToBeInserted(parent, index);
		parent->insert(index, layer);
		emit _scene->layerInserted(parent, index);
		
		enqueueTileUpdate(layer->tileKeysRecursive());
	}
	
	LayerRef takeLayer(const LayerRef &parent, int index)
	{
		emit _scene->layerAboutToBeRemoved(parent, index);
		auto layer = parent->take(index);
		emit _scene->layerRemoved(parent, index);
		
		enqueueTileUpdate(layer->tileKeysRecursive());
		
		return layer;
	}
	
	void emitLayerChanged(const LayerConstRef &layer)
	{
		emit _scene->layerChanged(layer);
	}
	
	LayerScene *scene() { return _scene; }
	
	void enqueueTileUpdate(const QPointSet &keys)
	{
		_scene->enqueueTileUpdate(keys);
	}
	
	LayerRef layerForPath(const Path &path)
	{
		return constSPCast<Layer>(_scene->layerForPath(path));
	}
	
	static Path pathForLayer(const LayerConstRef &layer)
	{
		return LayerScene::pathForLayer(layer);
	}
	
	QList<LayerRef> layersForPaths(const QList<Path> &paths)
	{
		QList<LayerRef> layers;
		layers.reserve(paths.size());
		for (auto &path : paths)
			layers << layerForPath(path);
		return layers;
	}
	
	QList<Path> pathsForLayers(const QList<LayerConstRef> &layers)
	{
		QList<Path> paths;
		paths.reserve(layers.size());
		for (auto &layer : layers)
			paths << pathForLayer(layer);
		return paths;
	}
	
private:
	
	LayerScene *_scene = 0;
	LayerRef _insertParent, _removeParent;
};

class LayerSceneEditCommand : public LayerSceneCommand
{
public:
	
	LayerSceneEditCommand(const LayerConstRef &layer, LayerEdit *edit, LayerScene *scene, QUndoCommand *parent = 0) :
		LayerSceneCommand(scene, parent),
		_path(pathForLayer(layer)),
		_edit(edit)
	{}
	
	void redo()
	{
		redoUndo(true);
	}
	
	void undo()
	{
		redoUndo(false);
	}
	
private:
	
	void redoUndo(bool redo)
	{
		auto layer = layerForPath(_path);
		
		if (redo)
			_edit->redo(layer);
		else
			_edit->undo(layer);
		
		layer->setThumbnailDirty(true);
		enqueueTileUpdate(_edit->modifiedKeys());
		
		emitLayerChanged(layer);
	}
	
	Path _path;
	QScopedPointer<LayerEdit> _edit;
};

class LayerScenePropertyChangeCommand : public LayerSceneCommand
{
public:
	
	LayerScenePropertyChangeCommand(const LayerConstRef &layer, const QVariant &data, int role, bool mergeOn, LayerScene *scene, QUndoCommand *parent = 0) :
		LayerSceneCommand(scene, parent),
		_path(pathForLayer(layer)),
		_data(data),
		_role(role),
	    _mergeOn(mergeOn)
	{}
	
	void redo()
	{
		change();
	}
	
	void undo()
	{
		change();
	}
	
	int id() const
	{
		return _role;
	}
	
	bool mergeWith(const QUndoCommand *cmd)
	{
		if (!_mergeOn)
			return false;
		
		auto other = dynamic_cast<const LayerScenePropertyChangeCommand *>(cmd);
		
		if (!other || !other->_mergeOn)
			return false;
		
		if (other->_path != _path || other->_role != _role || other->text() != text())
			return false;
		
		return true;
	}
	
private:
	
	void change()
	{
		auto layer = layerForPath(_path);
		
		enqueueLayerTileUpdate(layer);
		
		auto old = layer->property(_role);
		layer->setProperty(_data, _role);
		_data = old;
		
		enqueueLayerTileUpdate(layer);
		
		emitLayerChanged(layer);
	}
	
	void enqueueLayerTileUpdate(const LayerConstRef &layer)
	{
		switch (_role)
		{
			case RoleName:
			case RoleLocked:
				break;
			default:
				enqueueTileUpdate(layer->tileKeysRecursive());
				break;
		}
	}
	
	Path _path;
	QVariant _data;
	int _role;
	bool _mergeOn;
};

class LayerSceneAddCommand : public LayerSceneCommand
{
public:
	
	LayerSceneAddCommand(const LayerRef &layer, const LayerConstRef &parentRef, int index, LayerScene *scene, QUndoCommand *parent) :
		LayerSceneCommand(scene, parent),
		_layer(layer),
		_parentPath(pathForLayer(parentRef)),
		_index(index)
	{}
	
	void redo()
	{
		auto parent = layerForPath(_parentPath);
		insertLayer(parent, _index, _layer);
	}
	
	void undo()
	{
		auto parent = layerForPath(_parentPath);
		_layer = takeLayer(parent, _index);
	}
	
private:
	
	LayerRef _layer;
	Path _parentPath;
	int _index;
};

class LayerSceneRemoveCommand : public LayerSceneCommand
{
public:
	
	LayerSceneRemoveCommand(const LayerConstRef &layer, LayerScene *scene, QUndoCommand *parent) :
		LayerSceneCommand(scene, parent),
		_ref(layer)
	{
	}
	
	void redo()
	{
		if (!_pathsSet)
		{
			auto path = pathForLayer(_ref);
			_parentPath = path;
			_parentPath.removeLast();
			_index = path.last();
		}
		
		_layer = takeLayer(layerForPath(_parentPath), _index);
	}
	
	void undo()
	{
		insertLayer(layerForPath(_parentPath), _index, _layer);
	}
	
private:
	
	LayerConstRef _ref;
	
	bool _pathsSet = false;
	Path _parentPath;
	LayerRef _layer;
	int _index;
};

enum InsertionType
{
	InsertionTypeBefore,
	InsertionTypeAppendAsChild
};

class LayerSceneCopyCommand : public LayerSceneCommand
{
public:
	
	LayerSceneCopyCommand(const LayerConstRef &layer, const LayerConstRef &parentLayer, int index, const QString &newName, LayerScene *scene, QUndoCommand *parent) :
		LayerSceneCommand(scene, parent),
		_layer(layer),
		_parent(parentLayer),
		_index(index),
		_newName(newName)
	{
	}
	
	void redo()
	{
		if (!_pathsSet)
		{
			_layerPath = pathForLayer(_layer);
			_parentPath = pathForLayer(_parent);
			_pathsSet = true;
		}
		
		auto parent = layerForPath(_parentPath);
		auto layer = layerForPath(_layerPath);
		
		auto clone = layer->cloneRecursive();
		clone->setName(_newName);
		
		insertLayer(parent, _index, clone);
	}
	
	void undo()
	{
		auto parent = layerForPath(_parentPath);
		takeLayer(parent, _index);
	}
	
private:
	
	LayerConstRef _layer, _parent;
	
	bool _pathsSet = false;
	Path _layerPath, _parentPath;
	int _index;
	QString _newName;
};

class LayerSceneMoveCommand : public LayerSceneCommand
{
public:
	
	LayerSceneMoveCommand(const LayerConstRef &layer, const LayerConstRef &parentLayer, int index, const QString &newName, LayerScene *scene, QUndoCommand *parent) :
		LayerSceneCommand(scene, parent),
		_layer(layer),
		_parent(parentLayer),
		_index(index),
		_newName(newName)
	{
	}

	void redo()
	{
		if (!_pathsSet)
		{
			_layerPath = pathForLayer(_layer);
			_parentPath = pathForLayer(_parent);
			_pathsSet = true;
		}
		
		move();
	}
	
	void undo()
	{
		move();
	}
	
private:
	
	void move()
	{
		PAINTFIELD_DEBUG << _layerPath << _parentPath << _index;
		
		auto layer = layerForPath(_layerPath);
		int oldIndex = layer->index();
		auto oldParent = layer->parent();
		
		auto parent = layerForPath(_parentPath);
		int index = _index;
		
		takeLayer(oldParent, oldIndex);
		
		if (parent == oldParent)
		{
			if (index > oldIndex)
				--index;
			else if (oldIndex > index)
				++oldIndex;
		}
		
		insertLayer(parent, index, layer);
		
		auto oldName = layer->name();
		layer->setName(_newName);
		
		_index = oldIndex;
		_parentPath = pathForLayer(oldParent);
		_layerPath = pathForLayer(layer);
		_newName = oldName;
		
		PAINTFIELD_DEBUG << _layerPath << _parentPath << _index;
	}
	
	LayerConstRef _layer, _parent;
	
	bool _pathsSet = false;
	Path _layerPath, _parentPath;
	int _index;
	QString _newName;
};

/*
class LayerSceneMergeCommand : public LayerSceneCommand
{
public:
	
	LayerSceneMergeCommand(const QList<LayerConstPtr> &layers, const QString &newName, LayerScene *scene, QUndoCommand *parent) :
		LayerSceneCommand(scene, parent),
		_pathList(pathsForLayers(layers)),
		_newName(newName)
	{
		sortPathList(_pathList);
		
		auto lowest = layerForPath(_pathList.first());
		_parentPath = pathForLayer(lowest->parent());
		_index = lowest->index();
	}
	
	void redo()
	{
		auto layers = layersForPaths(_pathList);
		
		// make result layer
		auto merged = makeSP<RasterLayer>(_newName);
		{
			LayerRenderer renderer;
			merged->setSurface(renderer.renderToSurface(Malachite::constList(layers)));
		}
		
		// insert result layer
		auto parent = layerForPath(_parentPath);
		insertLayer(parent, _index, merged);
		_insertedPath = pathForLayer(merged);
		
		// remove original layers
		for (auto &layer : layers)
			takeLayer(layer->parent(), layer->index());
		
		_layers = layers;
	}
	
	void undo()
	{
		// remove merged layer
		auto merged = layerForPath(_insertedPath);
		takeLayer(merged->parent(), merged->index());
		
		// insert original layers
		// _pathList is already sorted lexicographically
		for (auto &path : _pathList)
		{
			Path layerParentPath = path;
			layerParentPath.removeLast();
			int layerIndex = path.last();
			
			auto layerParent = layerForPath(layerParentPath);
			insertLayer(layerParent, layerIndex, _layers.takeFirst());
		}
	}
	
private:
	
	QList<Path> _pathList;
	int _index;
	Path _parentPath;
	QString _newName;
	Path _insertedPath;
	
	QList<LayerPtr> _layers;
};*/

class LayerSceneMergeCommand : public LayerSceneCommand
{
public:
	
	LayerSceneMergeCommand(const LayerConstRef &parentRef, int index, int count, const QString &newName, LayerScene *scene, QUndoCommand *parent) :
		LayerSceneCommand(scene, parent),
		_index(index),
		_count(count),
		_newName(newName)
	{
		_parentPath = pathForLayer(parentRef);
	}
	
	void redo()
	{
		auto parent = layerForPath(_parentPath);
		
		for (int i = 0; i < _count; ++i)
			_layers << takeLayer(parent, _index);
		
		LayerRenderer renderer;
		
		auto newLayer = makeSP<RasterLayer>(_newName);
		newLayer->setSurface(renderer.renderToSurface(Malachite::constList(_layers)));
		newLayer->updateThumbnail(scene()->document()->size());
		
		insertLayer(parent, _index, newLayer);
	}
	
	void undo()
	{
		auto parent = layerForPath(_parentPath);
		
		takeLayer(parent, _index);
		
		for (int i = 0; i < _count; ++i)
			insertLayer(parent, _index + i, _layers.takeFirst());
	}
	
private:
	
	Path _parentPath;
	int _index, _count;
	QString _newName;
	
	QList<LayerRef> _layers;
};

struct LayerScene::Data
{
	SP<GroupLayer> rootLayer;
	Document *document = 0;
	QPointSet updatedKeys;
	QPointSet thumbnailDirtyKeys;
	
	QTimer *thumbnailUpdateTimer = 0;
	
	LayerItemModel *itemModel = 0;
	QItemSelectionModel *selectionModel = 0;
	
	LayerConstRef current;
	
	/**
	 * Checks if this scene contains the layer.
	 * @param layer
	 * @return 
	 */
	bool checkLayer(const LayerConstRef &layer)
	{
		return layer && layer->root() == rootLayer && layer != rootLayer;
	}
	
	bool checkLayers(const QList<LayerConstRef> &layers)
	{
		for (auto &layer : layers)
			if (!checkLayer(layer))
				return false;
		return true;
	}
	
	/**
	 * Checks if this scene contains the layer.
	 * This version returns true if the layer is the root.
	 * @param layer
	 * @return 
	 */
	bool checkParentLayer(const LayerConstRef &layer)
	{
		return layer && layer->root() == rootLayer;
	}
};

LayerScene::LayerScene(const QList<LayerRef> &layers, Document *document) :
	QObject(document),
	d(new Data)
{
	connect(this, SIGNAL(layerChanged(LayerConstRef)), this, SLOT(onLayerPropertyChanged(LayerConstRef)));
	
	d->document = document;
	connect(d->document, SIGNAL(modified()), this, SLOT(update()));
	
	{
		auto root = makeSP<GroupLayer>();
		root->insert(0, layers);
		root->updateThumbnailRecursive(document->size());
		d->rootLayer = root;
	}
	
	{
		auto t = new QTimer(this);
		t->setInterval(500);
		t->setSingleShot(true);
		connect(t, SIGNAL(timeout()), this, SLOT(updateDirtyThumbnails()));
		d->thumbnailUpdateTimer = t;
	}
	
	{
		auto im = new LayerItemModel( this, this );
		auto sm = new QItemSelectionModel(im, this );
		
		connect( sm, SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(onCurrentIndexChanged(QModelIndex,QModelIndex)) );
		connect( sm, SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(onItemSelectionChanged(QItemSelection,QItemSelection)) );
		
		sm->setCurrentIndex( im->index( 0, 0, QModelIndex() ), QItemSelectionModel::Current );
		
		d->itemModel = im;
		d->selectionModel = sm;
	}
}

LayerScene::~LayerScene()
{
	delete d;
}

class DuplicatedNameResolver
{
public:
	
	enum Type
	{
		TypeAdd,
		TypeMove
	};
	
	DuplicatedNameResolver(const LayerConstRef &parent, Type type) :
		_type(type),
		_parent(parent),
		_names(parent->childNames())
	{
	}
	
	QString resolve(const LayerConstRef &layer)
	{
		QString original = layer->name();
		
		if (_type == TypeMove && layer->parent() == _parent)
			return original;
		
		QString name = Util::unduplicatedName(_names, original);
		_names << name;
		return name;
	}
	
private:
	
	Type _type;
	LayerConstRef _parent;
	QStringList _names;
};

void LayerScene::addLayers(const QList<LayerRef> &layers, const LayerConstRef &parent, int index, const QString &description)
{
	if (!d->checkParentLayer(parent))
	{
		PAINTFIELD_WARNING << "invalid parent";
		return;
	}
	
	DuplicatedNameResolver resolver(parent, DuplicatedNameResolver::TypeAdd);
	auto command = new QUndoCommand(description);
	
	for (const auto &layer : layers)
	{
		layer->updateThumbnailRecursive(d->document->size());
		layer->setName(resolver.resolve(layer));
		new LayerSceneAddCommand(layer, parent, index++, this, command);
	}
	
	pushCommand(command);
}

void LayerScene::removeLayers(const QList<LayerConstRef> &layers, const QString &description)
{
	auto command = new QUndoCommand(description.isEmpty() ? tr("Remove Layers") : description);
	for (const auto &layer : layers)
		new LayerSceneRemoveCommand(layer, this, command);
	pushCommand(command);
}

void LayerScene::moveLayers(const QList<LayerConstRef> &layers, const LayerConstRef &parent, int index)
{
	if (!d->checkLayers(layers) || !d->checkParentLayer(parent))
	{
		PAINTFIELD_WARNING << "invalid parent";
		return;
	}
	
	int newIndex = index;
	
	DuplicatedNameResolver resolver(parent, DuplicatedNameResolver::TypeMove);
	
	auto command = new QUndoCommand(tr("Move Layers"));
	
	for (const auto &layer : layers)
	{
		new LayerSceneMoveCommand(layer, parent, newIndex, resolver.resolve(layer), this, command);
		
		if (layer->parent() == parent && layer->index() < index)
			--newIndex;
		++newIndex;
	}
	
	pushCommand(command);
}

void LayerScene::copyLayers(const QList<LayerConstRef> &layers, const LayerConstRef &parent, int index)
{
	if (!d->checkLayers(layers) || !d->checkParentLayer(parent))
	{
		PAINTFIELD_WARNING << "invalid parent";
		return;
	}
	
	int newIndex = index;
	
	DuplicatedNameResolver resolver(parent, DuplicatedNameResolver::TypeAdd);
	
	auto command = new QUndoCommand(tr("Move Layers"));
	
	for (const auto &layer : layers)
	{
		new LayerSceneCopyCommand(layer, parent, newIndex, resolver.resolve(layer), this, command);
		++newIndex;
	}
	
	pushCommand(command);
}

void LayerScene::mergeLayers(const LayerConstRef &parent, int index, int count)
{
	if (!d->checkParentLayer(parent))
	{
		PAINTFIELD_WARNING << "invalid parent";
		return;
	}
	
	QString mergedName;
	
	for (int i = index; i < index + count; ++i)
	{
		mergedName += parent->child(i)->name();
		if (i != index + count - 1)
			mergedName += " + ";
	}
	
	auto command = new LayerSceneMergeCommand(parent, index, count, mergedName, this, 0);
	command->setText(tr("Merge Layers"));
	pushCommand(command);
}

void LayerScene::editLayer(const LayerConstRef &layer, LayerEdit *edit, const QString &description)
{
	PAINTFIELD_DEBUG << d->rootLayer->children();
	PAINTFIELD_DEBUG << layer;
	PAINTFIELD_DEBUG << layer->parent();
	
	if (!d->checkLayer(layer))
	{
		PAINTFIELD_WARNING << "invalid layer";
		return;
	}
	
	if (layer->isLocked())
		return;
	
	auto command = new LayerSceneEditCommand(layer, edit, this, 0);
	command->setText(description);
	pushCommand(command);
}

void LayerScene::setLayerProperty(const LayerConstRef &layer, const QVariant &data, int role, const QString &description, bool mergeOn)
{
	if (!d->checkLayer(layer))
	{
		PAINTFIELD_WARNING << "invalid layer";
		return;
	}
	if (layer->isLocked() && role != RoleLocked)
	{
		PAINTFIELD_WARNING << "layer locked";
		return;
	}
	if (layer->property(role) == data)
		return;
	
	PAINTFIELD_DEBUG << "set layer property" << data;
	
	QString text = description;
	
	if (text.isEmpty())
	{
		// set description text if possible
		switch (role)
		{
			case PaintField::RoleName:
				text = tr("Rename Layer");
				break;
			case PaintField::RoleVisible:
				text = tr("Change visibility");
				break;
			case PaintField::RoleBlendMode:
				text = tr("Change Blend Mode");
				break;
			case PaintField::RoleOpacity:
				text = tr("Change Opacity");
				break;
			default:
				break;
		}
	}
	
	auto command = new LayerScenePropertyChangeCommand(layer, data, role, mergeOn, this, 0);
	command->setText(text);
	pushCommand(command);
}

LayerConstRef LayerScene::rootLayer() const
{
	return d->rootLayer;
}

Document *LayerScene::document()
{
	return d->document;
}

LayerItemModel *LayerScene::itemModel()
{
	return d->itemModel;
}

QItemSelectionModel *LayerScene::itemSelectionModel()
{
	return d->selectionModel;
}

LayerConstRef LayerScene::current() const
{
	return d->current;
}

QList<LayerConstRef> LayerScene::selection() const
{
	return d->itemModel->layersForIndexes(d->selectionModel->selection().indexes());
}

LayerConstRef LayerScene::layerForPath(const QList<int> &path)
{
	auto layer = rootLayer();
	
	for (int index : path)
		layer = layer->child(index);
	
	return layer;
}

QList<int> LayerScene::pathForLayer(const LayerConstRef &layer)
{
	QList<int> path;
	auto l = layer;
	while (l->parent())
	{
		path.prepend(l->index());
		l = l->parent();
	}
	return path;
}

void LayerScene::abortThumbnailUpdate()
{
	d->thumbnailUpdateTimer->stop();
}

void LayerScene::update()
{
	PAINTFIELD_DEBUG << d->updatedKeys;
	emit tilesUpdated(d->updatedKeys);
	d->thumbnailDirtyKeys = d->updatedKeys;
	d->thumbnailUpdateTimer->start();
	d->updatedKeys.clear();
}

void LayerScene::setCurrent(const LayerConstRef &layer)
{
	d->selectionModel->setCurrentIndex(d->itemModel->indexForLayer(layer), QItemSelectionModel::Current);
}

void LayerScene::setSelection(const QList<LayerConstRef> &layers)
{
	d->selectionModel->clearSelection();
	
	for (auto layer : layers)
		d->selectionModel->select(d->itemModel->indexForLayer(layer), QItemSelectionModel::Select);
}

void LayerScene::enqueueTileUpdate(const QPointSet &keys)
{
	d->updatedKeys |= keys;
}

void LayerScene::updateDirtyThumbnails()
{
	d->rootLayer->updateDirtyThumbnailRecursive(d->document->size());
	emit thumbnailsUpdated(d->thumbnailDirtyKeys);
}

LayerRef LayerScene::mutableRootLayer()
{
	return d->rootLayer;
}

void LayerScene::pushCommand(QUndoCommand *command)
{
	PAINTFIELD_DEBUG << "pushing command" << command->text();
	d->document->undoStack()->push(command);
}

void LayerScene::onCurrentIndexChanged(const QModelIndex &now, const QModelIndex &old)
{
	d->current = d->itemModel->layerExceptRootForIndex(now);
	emit currentChanged(d->current, d->itemModel->layerExceptRootForIndex(old));
}

void LayerScene::onItemSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
	emit selectionChanged(d->itemModel->layersForIndexes(selected.indexes()), d->itemModel->layersForIndexes(deselected.indexes()));
}

void LayerScene::onLayerPropertyChanged(const LayerConstRef &layer)
{
	if (layer == d->current)
		emit currentLayerChanged();
}

} // namespace PaintField
