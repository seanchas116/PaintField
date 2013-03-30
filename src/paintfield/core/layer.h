#ifndef FSLAYER_H
#define FSLAYER_H

#include <QList>
#include <QPixmap>
#include <QVariant>
#include <tuple>

#include <Malachite/Surface>
#include <Malachite/Misc>
#include <Malachite/BlendMode>
#include <Malachite/Container>

#include "util.h"
#include "global.h"

namespace Malachite
{
class Painter;
}

namespace PaintField
{

class Layer;
typedef QList<Layer *> LayerList;
typedef QList<const Layer *> LayerConstList;

class LayerRef;

class Layer
{
public:
	
	Layer(const QString &name = QString());
	virtual ~Layer();
	
	LayerList children() { return _children; }
	LayerConstList children() const { return Malachite::constList(_children); }
	
	const Layer *child(int index) const;
	Layer *child(int index) { return const_cast<Layer *>(static_cast<const Layer *>(this)->child(index)); }
	
	const Layer *sibling(int index) const { return _parent->child(index); }
	Layer *sibling(int index) { return _parent->child(index); }
	
	const Layer *parent() const { return _parent; }
	Layer *parent() { return _parent; }
	
	/**
	 * @return The root layer of this layer
	 */
	const Layer *root() const;
	
	/**
	 * @return The root layer of this layer
	 */
	Layer *root() { return const_cast<Layer *>(static_cast<const Layer *>(this)->root()); }
	
	/**
	 * This function is faster than isAncestorOfSafe().
	 * @param layer
	 * @return Whether this layer is an ancestor of the "layer"
	 */
	bool isAncestorOf(const Layer *layer) const;
	
	/**
	 * This function is slower but safer than isAncestorOf() because it even works if "layer" is already deleted.
	 * @param layer
	 * @return Whether this layer is an ancestor of "layer"
	 */
	bool isAncestorOfSafe(const Layer *layer) const;
	
	/**
	 * @param row
	 * @return Whether this layer contains a child layer with row index = "row"
	 */
	bool contains(int index) const { return (0 <= index && index < _children.size()) ? true : false; }
	
	/**
	 * @param layer
	 * @return Wheter this layer contains "layer"
	 */
	bool contains(Layer *layer) const { return _children.contains(layer); }
	
	/**
	 * @param row
	 * @return Whether this contains a child layer with row index = "row".
	 */
	bool insertable(int index) const { return (0 <= index && index <= _children.size()) ? true : false; }
	
	/**
	 * @return How many child layers this layer has
	 */
	int count() const { return _children.size(); }
	
	/**
	 * @return How many child layers "this" parent has
	 */
	int siblingCount() const { return _parent->count(); }
	
	/**
	 * @param child
	 * @return The index of a child layer
	 */
	int indexOf(const Layer *child) const { return _children.indexOf(const_cast<Layer *>(child)); }
	
	/**
	 * @return The index of this layer in its parent.
	 */
	int index() const { return _parent ? _parent->indexOf(this) : 0; }
	
	/**
	 * Inserts a child layer.
	 * @param row
	 * @param child
	 * @return Whether succeeded
	 */
	bool insert(int index, Layer *child);
	
	/**
	 * Inserts child layers.
	 * @param row
	 * @param children
	 * @return Whether succeeded
	 */
	bool insert(int index, const LayerList &children);
	
	/**
	 * Prepends a child layer.
	 * @param child
	 */
	void prepend(Layer *child) { insert(0, child); }
	
	void prepend(const LayerList &layers) { insert(0, layers); }
	
	/**
	 * Appends a child layer.
	 * @param child
	 */
	void append(Layer *child) { insert(count(), child); }
	
	void append(const LayerList &layers) { insert(count(), layers); }
	
	/**
	 * Take a child layer.
	 * Its parent will be 0.
	 * @param row
	 * @return The taken layer
	 */
	Layer *take(int index);
	
	/**
	 * Take all child layers.
	 * @return The taken layers
	 */
	LayerList takeAll();
	
	/**
	 * Replace a child layer.
	 * This function does both insertChild() and takeChild().
	 * @param row
	 * @param child
	 * @return The replaced layer
	 */
	Layer *replace(int index, Layer *child);
	
	/**
	 * Removes a child.
	 * @param row
	 * @return whether succeeded
	 */
	bool remove(int index);
	
	bool shift(int start, int end, int shiftCount);
	
	/**
	 * Clones this layer.
	 * Calls createAnother to duplicate the layer and use encode / decode to copy data.
	 * @return 
	 */
	Layer *clone() const;
	
	/**
	 * Clones this layer and its descendants.
	 * @return The cloned layer
	 */
	Layer *cloneRecursive() const;
	
	/**
	 * Creates an unduplicated child name (eg "Layer 1").
	 * @param name A base name (eg "Layer")
	 * @return The unduplicated child name
	 */
	QString unduplicatedChildName(const QString &name) const { return Util::unduplicatedName(childNames(), name); }
	
	QStringList childNames() const;
	
	/**
	 * Sets a property.
	 * @param data
	 * @param role
	 * @return Whether succeeded
	 */
	virtual bool setProperty(const QVariant &data, int role);
	
	/**
	 * @param role
	 * @return The property
	 */
	virtual QVariant property(int role) const;
	
	void setName(const QString &name) { _name = name; }
	QString name() const { return _name; }
	
	void setVisible(bool visible) { _isVisible = visible; }
	bool isVisible() const { return _isVisible; }
	
	void setLocked(bool locked) { _isLocked = locked; }
	bool isLocked() const { return property(PaintField::RoleLocked).toBool(); }
	
	void setThumbnail(const QPixmap &thumbnail) { _thumbnail = thumbnail; }
	QPixmap thumbnail() const { return _thumbnail; }
	
	void setOpacity(double opacity) { _opacity = opacity; }
	double opacity() const { return _opacity; }
	
	void setBlendMode(Malachite::BlendMode mode) { _blendMode = mode; }
	Malachite::BlendMode blendMode() const { return _blendMode; }
	
	void setThumbnailDirty(bool x) { _isThumbnailDirty = x; }
	bool isThumbnailDirty() const { return _isThumbnailDirty; }
	
	/**
	 * Updates only this layer's thumbnail
	 * @param documentSize
	 */
	virtual void updateThumbnail(const QSize &documentSize) { Q_UNUSED(documentSize) }
	
	/**
	 * Updates the thumbnail of each of ths and this descendant layers.
	 * @param documentSize
	 */
	void updateThumbnailRecursive(const QSize &documentSize);
	
	/**
	 * Updates the thumbnail of each of ths and this descendant layers, if its isThumbnailDirty() is true.
	 * @param size
	 */
	void updateDirtyThumbnailRecursive(const QSize &size);
	
	const Layer *descendantAt(const QPoint &pos) const;
	
	virtual QPointSet tileKeys() const { return QPointSet(); }
	QPointSet tileKeysRecursive() const;
	
	void encodeRecursive(QDataStream &stream) const;
	static Layer *decodeRecursive(QDataStream &stream);
	
	/**
	 * Creates another instance of the layer.
	 * The properties does not have to be copied.
	 * @return 
	 */
	virtual Layer *createAnother() const = 0;
	
	virtual bool canHaveChildren() const { return false; }
	
	template <class T> bool isType() const { return dynamic_cast<const T *>(this); }
	
	virtual void encode(QDataStream &stream) const;
	virtual void decode(QDataStream &stream);
	
	virtual QVariantMap saveProperies() const;
	virtual void loadProperties(const QVariantMap &map);
	
	virtual bool hasDataToSave() const { return false; }
	virtual void saveDataFile(QDataStream &stream) const { Q_UNUSED(stream) }
	virtual void loadDataFile(QDataStream &stream) { Q_UNUSED(stream) }
	virtual QString dataSuffix() const { return "data"; }
	
	virtual void render(Malachite::Painter *painter) const { Q_UNUSED(painter) }
	
	virtual bool includes(const QPoint &pos) const { Q_UNUSED(pos) return false; }
	
protected:
	
private:
	
	friend class LayerRef;
	
	Layer *_parent = 0;	// 0 : root item
	LayerList _children;
	
	QString _name;
	bool _isLocked = false, _isVisible = true;
	double _opacity = 1.0;
	Malachite::BlendMode _blendMode;
	QPixmap _thumbnail;
	
	bool _isThumbnailDirty = false;
	
	mutable QList<LayerRef *> _indexes;
};

class LayerFactory
{
public:
	LayerFactory() {}
	virtual ~LayerFactory() {}
	
	virtual QString name() const = 0;
	virtual Layer *create() const = 0;
	virtual const ::std::type_info &typeInfo() const = 0;
};

class LayerRef
{
public:
	
	LayerRef() :
		_layer(0)
	{}
	
	LayerRef(const Layer *layer)
	{
		setLayer(layer);
	}
	
	LayerRef(const LayerRef &other)
	{
		setLayer(other.pointer());
	}
	
	~LayerRef()
	{
		clear();
	}
	
	LayerRef &operator=(const LayerRef &other)
	{
		setLayer(other.pointer());
		return *this;
	}
	
	const Layer *pointer() const { return _layer; }
	
	int index() const
	{
		Q_ASSERT(_layer);
		return _layer->index();
	}
	
	int count() const
	{
		Q_ASSERT(_layer);
		return _layer->count();
	}
	
	QList<LayerRef> children() const;
	
	LayerRef child(int index) const
	{
		Q_ASSERT(_layer);
		return _layer->child(index);
	}
	
	LayerRef sibling(int index) const
	{
		Q_ASSERT(_layer);
		return _layer->sibling(index);
	}
	
	LayerRef parent() const
	{
		Q_ASSERT(_layer);
		return _layer->parent();
	}
	
	bool isValid() const { return _layer; }
	
	void setLayer(const Layer *layer)
	{
		clear();
		
		if (layer)
		{
			layer->_indexes << this;
			_layer = layer;
		}
	}
	
	void clear()
	{
		if (_layer)
		{
			_layer->_indexes.removeAll(this);
			_layer = 0;
		}
	}
	
	bool operator==(const LayerRef &other) const
	{
		return _layer == other._layer;
	}
	
	bool operator!=(const LayerRef &other) const { return !operator==(other); }
	
	operator bool() const { return isValid(); }
	
	const Layer &operator*() const { return *_layer; }
	const Layer *operator->() const { return _layer; }
	
private:
	
	friend class Layer;
	const Layer *_layer = 0;
};

typedef QList<LayerRef> LayerRefList;

}

#endif // FSLAYER_H
