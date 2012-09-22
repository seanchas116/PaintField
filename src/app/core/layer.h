#ifndef FSLAYER_H
#define FSLAYER_H

#include <QList>
#include <QPixmap>
#include <QVariant>
#include "Malachite/mlsurface.h"
#include "Malachite/mlmisc.h"
#include "global.h"
#include "Malachite/mlblendmode.h"

namespace PaintField {

class Layer;
typedef QList<Layer *> LayerList;
typedef QList<const Layer *> LayerConstList;

class Layer
{
public:
	enum Type {
		TypeGroup,
		TypeRaster
	};
	
	Layer(const QString &name = QString());
	Layer(const Layer &other);
	virtual ~Layer();
	
	LayerList children() { return _childrenList; }
	LayerConstList children() const { return Malachite::constList(_childrenList); }
	
	const Layer *child(int row) const;
	const Layer *child(const QString &name) const;
	Layer *child(int row) { return const_cast<Layer *>(static_cast<const Layer *>(this)->child(row)); }
	Layer *child(const QString &name) { return const_cast<Layer *>(static_cast<const Layer *>(this)->child(name)); }
	
	const Layer *sibling(int row) const { return _parent->child(row); }
	const Layer *sibling(const QString &name) const { return _parent->child(name); }
	Layer *sibling(int row) { return _parent->child(row); }
	Layer *sibling(const QString &name) { return _parent->child(name); }
	
	const Layer *parent() const { return _parent; }
	Layer *parent() { return _parent; }
	
	/**
	  Returns the root layer of "this".
	*/
	const Layer *root() const;
	
	/**
	  Returns the root layer of "this".
	*/
	Layer *root() { return const_cast<Layer *>(static_cast<const Layer *>(this)->root()); }
	
	/**
	  Returns whether "this" is an ancestor of "layer".
	*/
	bool isAncestorOf(const Layer *layer) const;
	
	/**
	  Returns whether "this" is an ancestor of "layer".
	  This function is safer than isAncestorOf() because it even works if "layer" is already deleted.
	*/
	bool isAncestorOfSafe(const Layer *layer) const;
	
	/**
	  Returns whether this contains a child layer with row index = "row".
	*/
	bool contains(int row) const { return (0 <= row && row < _childrenList.size()) ? true : false; }
	
	bool contains(Layer *layer) const { return _childrenList.contains(layer); }
	
	/**
	  Returns whether this contains a child layer with row index = "row".
	  Same as contains().
	*/
	bool insertable(int row) const { return (0 <= row && row <= _childrenList.size()) ? true : false; }
	
	/**
	  Returns how many child layers "this" has.
	*/
	int childCount() const { return _childrenList.size(); }
	
	/**
	  Returns how many child layers "this" parent has.
	*/
	int siblingCount() const { return _parent->childCount(); }
	
	/**
	  Returns the row index of a child layer "child".
	*/
	int rowForChild(const Layer *child) const { return _childrenList.indexOf(const_cast<Layer *>(child)); }
	
	/**
	  Returns the row index of "this" in its parent.
	*/
	int row() const { return _parent ? _parent->rowForChild(this) : 0; }
	
	/**
	  Inserts "child" at "row" as a child layer and returns whether it succeeded.
	*/
	bool insertChild(int row, Layer *child);
	
	/**
	  Inserts "children" at "row" as child layers and returns whether succeeded.
	*/
	bool insertChildren(int row, const LayerList &children);
	
	/**
	  Prepends "child" as a child layer.
	*/
	void prependChild(Layer *child) { insertChild(0, child); }
	
	/**
	  Appends "child" as a child layer.
	*/
	void appendChild(Layer *child) { insertChild(childCount(), child); }
	
	/**
	  Take the child layer at "row" and returns it.
	  Its parent will be 0.
	*/
	Layer *takeChild(int row);
	
	/**
	  Take all children and returns them.
	  All of their parents will be 0;
	*/
	LayerList takeChildren();
	
	/**
	  Replace the child layer at "row" with "child".
	  Returns the replaced layer if succeeded, otherwise 0.
	  This function will do insertChild() and takeChild().
	*/
	Layer *replaceChild(int row, Layer *child);
	
	/**
	  Remove the child at "row" and returns whether succeeded.
	*/
	bool removeChild(int row);
	
	/**
	  Clones "this" and the descendants.
	*/
	Layer *cloneRecursive();
	
	QString unduplicatedChildName(const QString &name) const;
	
	virtual bool setProperty(const QVariant &data, int role);
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
	void setBlendMode(int mode) { _blendMode = mode; }
	int blendMode() const { return _blendMode; }
	
	void setThumbnailDirty(bool x) { _isThumbnailDirty = x; }
	bool isThumbnailDirty() const { return _isThumbnailDirty; }
	
	virtual void updateThumbnail(const QSize &size);
	void updateThumbnailRecursive(const QSize &size);
	void updateDirtyThumbnailRecursive(const QSize &size);
	
	QPointSet tileKeys() const { return surface().keys(); }
	QPointSet tileKeysRecursive() const;
	
	virtual Malachite::Surface surface() const { return Malachite::Surface(); }
	virtual Layer *clone() const = 0;
	virtual Type type() const = 0;
	virtual bool canHaveChildren() const { return false; }
	
protected:
	
private:
	Layer *_parent;	// 0 : root item
	LayerList _childrenList;
	
	QString _name;
	bool _isLocked, _isVisible;
	double _opacity;
	int _blendMode;
	QPixmap _thumbnail;
	
	bool _isThumbnailDirty;
};


class RasterLayer : public Layer
{
public:
	RasterLayer(const QString &name = QString()) : Layer(name) {}
	RasterLayer(const RasterLayer &other) : Layer(other), _surface(other._surface) {}
	
	bool setProperty(const QVariant &data, int role);
	
	Layer *clone() const { return new RasterLayer(*this); }
	Type type() const { return Layer::TypeRaster; }
	
	Malachite::Surface surface() const { return _surface; }
	void setSurface(const Malachite::Surface &surface) { _surface = surface; }
	
	Malachite::Surface *psurface() { return &_surface; }
	
private:
	Malachite::Surface _surface;
};


class GroupLayer : public Layer
{
public:
	GroupLayer(const QString &name = QString()) : Layer(name) {}
	GroupLayer(const GroupLayer &other) : Layer(other) {}
	
	Layer *clone() const { return new GroupLayer(*this); }
	Type type() const { return Layer::TypeGroup; }
	bool canHaveChildren() const { return true; }
	
	void updateThumbnail(const QSize &size);
};



}

#endif // FSLAYER_H
