#include <functional>
#include <tuple>
#include <QUndoCommand>
#include <QTimer>
#include <QItemSelectionModel>
#include <boost/range/adaptors.hpp>

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
	
	LayerSceneCommand(LayerScene *scene, QUndoCommand *parent) :
		QUndoCommand(parent),
		_scene(scene)
	{}
	
	void insertLayer(Layer *parent, int index, Layer *layer)
	{
		PAINTFIELD_DEBUG << parent << index << layer;
		
		emit _scene->layerAboutToBeInserted(parent, index);
		parent->insert(index, layer);
		emit _scene->layerInserted(parent, index);
		
		enqueueTileUpdate(layer->tileKeysRecursive());
	}
	
	Layer *takeLayer(Layer *parent, int index)
	{
		emit _scene->layerAboutToBeRemoved(parent, index);
		auto layer = parent->take(index);
		emit _scene->layerRemoved(parent, index);
		
		enqueueTileUpdate(layer->tileKeysRecursive());
		
		return layer;
	}
	
	void emitLayerPropertyChanged(Layer *layer)
	{
		emit _scene->layerPropertyChanged(layer);
	}
	
	LayerScene *scene() { return _scene; }
	
	void enqueueTileUpdate(const QPointSet &keys)
	{
		_scene->enqueueTileUpdate(keys);
	}
	
	Layer *layerForPath(const Path &path)
	{
		auto layer = _scene->mutableRootLayer();
		
		for (int index : path)
			layer = layer->child(index);
		
		return layer;
	}
	
	static Path pathForLayer(LayerRef layer)
	{
		Path path;
		while (layer.parent().isValid())
		{
			path.prepend(layer.index());
			layer = layer.parent();
		}
		return path;
	}
	
private:
	
	LayerScene *_scene = 0;
	LayerRef _insertParent, _removeParent;
};

class LayerSceneEditCommand : public LayerSceneCommand
{
public:
	
	LayerSceneEditCommand(const LayerRef &layer, LayerEdit *edit, LayerScene *scene, QUndoCommand *parent = 0) :
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
	}
	
	Path _path;
	QScopedPointer<LayerEdit> _edit;
};

class LayerScenePropertyChangeCommand : public LayerSceneCommand
{
public:
	
	LayerScenePropertyChangeCommand(const LayerRef &layer, const QVariant &data, int role, LayerScene *scene, QUndoCommand *parent = 0) :
		LayerSceneCommand(scene, parent),
		_path(pathForLayer(layer)),
		_data(data),
		_role(role)
	{}
	
	void redo()
	{
		change();
	}
	
	void undo()
	{
		change();
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
		
		emitLayerPropertyChanged(layer);
	}
	
	void enqueueLayerTileUpdate(const Layer *layer)
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
};

class LayerSceneAddCommand : public LayerSceneCommand
{
public:
	
	LayerSceneAddCommand(Layer *layer, const LayerRef &parentRef, int index, LayerScene *scene, QUndoCommand *parent) :
		LayerSceneCommand(scene, parent),
		_layer(layer),
		_parentPath(pathForLayer(parentRef)),
		_index(index)
	{}
	
	void redo()
	{
		auto parent = layerForPath(_parentPath);
		insertLayer(parent, _index, _layer.take());
	}
	
	void undo()
	{
		auto parent = layerForPath(_parentPath);
		_layer.reset(takeLayer(parent, _index));
	}
	
private:
	
	QScopedPointer<Layer> _layer;
	Path _parentPath;
	int _index;
};

class LayerSceneRemoveCommand : public LayerSceneCommand
{
public:
	
	LayerSceneRemoveCommand(const LayerRef &layer, LayerScene *scene, QUndoCommand *parent) :
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
		
		_layer.reset(takeLayer(layerForPath(_parentPath), _index));
	}
	
	void undo()
	{
		insertLayer(layerForPath(_parentPath), _index, _layer.take());
	}
	
private:
	
	LayerRef _ref;
	
	bool _pathsSet = false;
	Path _parentPath;
	QScopedPointer<Layer> _layer;
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
	
	LayerSceneCopyCommand(const LayerRef &layer, const LayerRef &parentLayer, int index, const QString &newName, LayerScene *scene, QUndoCommand *parent) :
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
		delete takeLayer(parent, _index);
	}
	
private:
	
	LayerRef _layer, _parent;
	
	bool _pathsSet = false;
	Path _layerPath, _parentPath;
	int _index;
	QString _newName;
};

class LayerSceneMoveCommand : public LayerSceneCommand
{
public:
	
	LayerSceneMoveCommand(const LayerRef &layer, const LayerRef &parentLayer, int index, const QString &newName, LayerScene *scene, QUndoCommand *parent) :
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
	
	LayerRef _layer, _parent;
	
	bool _pathsSet = false;
	Path _layerPath, _parentPath;
	int _index;
	QString _newName;
};

class LayerSceneMergeCommand : public LayerSceneCommand
{
public:
	
	LayerSceneMergeCommand(const LayerRef &parentRef, int index, int count, const QString &newName, LayerScene *scene, QUndoCommand *parent) :
		LayerSceneCommand(scene, parent),
		_index(index),
		_count(count),
		_newName(newName),
		_group(new GroupLayer())
	{
		_parentPath = pathForLayer(parentRef);
	}
	
	void redo()
	{
		auto parent = layerForPath(_parentPath);
		
		for (int i = 0; i < _count; ++i)
			_group->append(takeLayer(parent, _index));
		
		LayerRenderer renderer;
		
		auto newLayer = new RasterLayer(_newName);
		newLayer->setSurface(renderer.renderToSurface(_group.data()));
		newLayer->updateThumbnail(scene()->document()->size());
		
		insertLayer(parent, _index, newLayer);
	}
	
	void undo()
	{
		auto parent = layerForPath(_parentPath);
		
		delete takeLayer(parent, _index);
		
		for (int i = 0; i < _count; ++i)
			insertLayer(parent, _index + i, _group->take(0));
	}
	
private:
	
	Path _parentPath;
	int _index, _count;
	QString _newName;
	
	QScopedPointer<GroupLayer> _group;
};

struct LayerScene::Data
{
	QScopedPointer<GroupLayer> rootLayer;
	Document *document = 0;
	QPointSet updatedKeys;
	
	QTimer *thumbnailUpdateTimer = 0;
	
	LayerItemModel *itemModel = 0;
	QItemSelectionModel *selectionModel = 0;
	
	LayerRef current;
};

LayerScene::LayerScene(const LayerList &layers, Document *document) :
	QObject(document),
	d(new Data)
{
	connect(this, SIGNAL(layerPropertyChanged(LayerRef)), this, SLOT(onLayerPropertyChanged(LayerRef)));
	
	d->document = document;
	connect(d->document, SIGNAL(modified()), this, SLOT(update()));
	
	{
		auto root = new GroupLayer();
		root->insert(0, layers);
		root->updateThumbnailRecursive(document->size());
		d->rootLayer.reset(root);
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
	
	DuplicatedNameResolver(const LayerRef &parent, Type type) :
		_type(type),
		_parent(parent),
		_names(parent.pointer()->childNames())
	{
	}
	
	QString resolve(const Layer *layer)
	{
		QString original = layer->name();
		
		if (_type == TypeMove && layer->parent() == _parent.pointer())
			return original;
		
		QString name = Util::unduplicatedName(_names, original);
		_names << name;
		return name;
	}
	
private:
	
	Type _type;
	LayerRef _parent;
	QStringList _names;
};

void LayerScene::addLayers(const LayerList &layers, const LayerRef &parent, int index, const QString &description)
{
	if (!parent.isValid())
		return;
	
	DuplicatedNameResolver resolver(parent, DuplicatedNameResolver::TypeAdd);
	auto command = new QUndoCommand(description);
	
	for (auto layer : layers)
	{
		layer->updateThumbnailRecursive(d->document->size());
		layer->setName(resolver.resolve(layer));
		new LayerSceneAddCommand(layer, parent, index++, this, command);
	}
	
	pushCommand(command);
}

void LayerScene::removeLayers(const LayerRefList &refs, const QString &description)
{
	auto command = new QUndoCommand(description.isEmpty() ? tr("Remove Layers") : description);
	for (auto ref : refs)
		new LayerSceneRemoveCommand(ref, this, command);
	pushCommand(command);
}

void LayerScene::moveLayers(const LayerRefList &refs, const LayerRef &parent, int index)
{
	int newIndex = index;
	
	DuplicatedNameResolver resolver(parent, DuplicatedNameResolver::TypeMove);
	
	auto command = new QUndoCommand(tr("Move Layers"));
	
	for (auto ref : refs)
	{
		new LayerSceneMoveCommand(ref, parent, newIndex, resolver.resolve(ref.pointer()), this, command);
		
		if (ref.parent() == parent && ref.index() < index)
			--newIndex;
		++newIndex;
	}
	
	pushCommand(command);
}

void LayerScene::copyLayers(const LayerRefList &refs, const LayerRef &parent, int index)
{
	int newIndex = index;
	
	DuplicatedNameResolver resolver(parent, DuplicatedNameResolver::TypeAdd);
	
	auto command = new QUndoCommand(tr("Move Layers"));
	
	for (auto ref : refs)
	{
		new LayerSceneCopyCommand(ref, parent, newIndex, resolver.resolve(ref.pointer()), this, command);
		++newIndex;
	}
	
	pushCommand(command);
}

void LayerScene::mergeLayers(const LayerRef &parent, int index, int count)
{
	PAINTFIELD_DEBUG << index << count;
	
	QString mergedName;
	
	for (int i = index; i < index + count; ++i)
	{
		mergedName += parent.child(i).pointer()->name();
		if (i != index + count - 1)
			mergedName += " + ";
	}
	
	auto command = new LayerSceneMergeCommand(parent, index, count, mergedName, this, 0);
	command->setText(tr("Merge Layers"));
	pushCommand(command);
}

void LayerScene::editLayer(const LayerRef &ref, LayerEdit *edit, const QString &description)
{
	if (ref->isLocked())
		return;
	
	auto command = new LayerSceneEditCommand(ref, edit, this, 0);
	command->setText(description);
	pushCommand(command);
}

void LayerScene::setLayerProperty(const LayerRef &ref, const QVariant &data, int role, const QString &description)
{
	if (ref->isLocked() && role != RoleLocked)
		return;
	
	if (ref->property(role) == data)
		return;
	
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
	
	auto command = new LayerScenePropertyChangeCommand(ref, data, role, this, 0);
	command->setText(text);
	pushCommand(command);
}

LayerRef LayerScene::rootLayer() const
{
	return d->rootLayer.data();
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

LayerRef LayerScene::current() const
{
	return d->current;
}

LayerRefList LayerScene::selection() const
{
	return d->itemModel->layersForIndexes(d->selectionModel->selection().indexes());
}

void LayerScene::abortThumbnailUpdate()
{
	d->thumbnailUpdateTimer->stop();
}

void LayerScene::update()
{
	emit tilesUpdated(d->updatedKeys);
	d->thumbnailUpdateTimer->start();
	d->updatedKeys.clear();
}

void LayerScene::setCurrent(const LayerRef &layer)
{
	d->selectionModel->setCurrentIndex(d->itemModel->indexForLayer(layer), QItemSelectionModel::Current);
}

void LayerScene::setSelection(const LayerRefList &layers)
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
	emit thumbnailsUpdated();
}

Layer *LayerScene::mutableRootLayer()
{
	return d->rootLayer.data();
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

void LayerScene::onLayerPropertyChanged(const LayerRef &layer)
{
	if (layer == d->current)
		emit currentLayerPropertyChanged();
}

} // namespace PaintField
