#ifndef FSLAYER_H
#define FSLAYER_H

#include <QList>
#include <QPixmap>
#include <QVariant>
#include "mlsurface.h"
#include "mlmisc.h"
#include "fsglobal.h"
#include "mlblendmode.h"

class FSDocumentModel;

class FSLayer;
typedef QList<FSLayer *> FSLayerList;
typedef QList<const FSLayer *> FSLayerConstList;

class FSLayer
{
public:
	enum Type {
		TypeGroup,
		TypeRaster
	};
	
	FSLayer(const QString &name = QString());
	FSLayer(const FSLayer &other);
	virtual ~FSLayer();
	
	FSLayerList children() { return _childrenList; }
	FSLayerConstList children() const { return mlConstList(_childrenList); }
	
	const FSLayer *child(int row) const;
	const FSLayer *child(const QString &name) const;
	FSLayer *child(int row) { return const_cast<FSLayer *>(static_cast<const FSLayer *>(this)->child(row)); }
	FSLayer *child(const QString &name) { return const_cast<FSLayer *>(static_cast<const FSLayer *>(this)->child(name)); }
	
	const FSLayer *sibling(int row) const { return _parent->child(row); }
	const FSLayer *sibling(const QString &name) const { return _parent->child(name); }
	FSLayer *sibling(int row) { return _parent->child(row); }
	FSLayer *sibling(const QString &name) { return _parent->child(name); }
	
	const FSLayer *parent() const { return _parent; }
	FSLayer *parent() { return _parent; }
	
	/**
	  Returns the root layer of "this".
	*/
	const FSLayer *root() const;
	
	/**
	  Returns the root layer of "this".
	*/
	FSLayer *root() { return const_cast<FSLayer *>(static_cast<const FSLayer *>(this)->root()); }
	
	/**
	  Returns whether "this" is an ancestor of "layer".
	*/
	bool isAncestorOf(const FSLayer *layer) const;
	
	/**
	  Returns whether "this" is an ancestor of "layer".
	  This function is safer than isAncestorOf() because it even works if "layer" is already deleted.
	*/
	bool isAncestorOfSafe(const FSLayer *layer) const;
	
	/**
	  Returns whether this contains a child layer with row index = "row".
	*/
	bool contains(int row) const { return (0 <= row && row < _childrenList.size()) ? true : false; }
	
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
	int rowForChild(const FSLayer *child) const { return _childrenList.indexOf(const_cast<FSLayer *>(child)); }
	
	/**
	  Returns the row index of "this" in its parent.
	*/
	int row() const { return _parent ? _parent->rowForChild(this) : 0; }
	
	/**
	  Inserts "child" at "row" as a child layer and returns whether it succeeded.
	*/
	bool insertChild(int row, FSLayer *child);
	
	/**
	  Inserts "children" at "row" as child layers and returns whether succeeded.
	*/
	bool insertChildren(int row, const FSLayerList &children);
	
	/**
	  Prepends "child" as a child layer.
	*/
	void prependChild(FSLayer *child) { insertChild(0, child); }
	
	/**
	  Appends "child" as a child layer.
	*/
	void appendChild(FSLayer *child) { insertChild(childCount(), child); }
	
	/**
	  Take the child layer at "row" and returns it.
	  Its parent will be 0.
	*/
	FSLayer *takeChild(int row);
	
	/**
	  Take all children and returns them.
	  All of their parents will be 0;
	*/
	FSLayerList takeChildren();
	
	/**
	  Replace the child layer at "row" with "child".
	  Returns the replaced layer if succeeded, otherwise 0.
	  This function will do insertChild() and takeChild().
	*/
	FSLayer *replaceChild(int row, FSLayer *child);
	
	/**
	  Remove the child at "row" and returns whether succeeded.
	*/
	bool removeChild(int row);
	
	/**
	  Clones "this" and the descendants.
	*/
	FSLayer *cloneRecursive();
	
	QString unduplicatedChildName(const QString &name) const;
	
	virtual bool setProperty(const QVariant &data, int role);
	virtual QVariant property(int role) const;
	
	void setName(const QString &name) { _name = name; }
	QString name() const { return _name; }
	void setVisible(bool visible) { _isVisible = visible; }
	bool isVisible() const { return _isVisible; }
	void setLocked(bool locked) { _isLocked = locked; }
	bool isLocked() const { return property(FSGlobal::RoleLocked).toBool(); }
	void setThumbnail(const QPixmap &thumbnail) { _thumbnail = thumbnail; }
	QPixmap thumbnail() const { return _thumbnail; }
	void setOpacity(double opacity) { _opacity = opacity; }
	double opacity() const { return _opacity; }
	void setBlendMode(MLBlendMode mode) { _blendMode = mode; }
	MLBlendMode blendMode() const { return _blendMode; }
	
	void setThumbnailDirty(bool x) { _isThumbnailDirty = x; }
	bool isThumbnailDirty() const { return _isThumbnailDirty; }
	
	virtual void updateThumbnail(const QSize &size);
	void updateThumbnailRecursive(const QSize &size);
	void updateDirtyThumbnailRecursive(const QSize &size);
	
	QPointSet tileKeys() const { return surface().keys(); }
	QPointSet tileKeysRecursive() const;
	
	virtual MLSurface surface() const { return MLSurface(); }
	virtual FSLayer *clone() const = 0;
	virtual Type type() const = 0;
	virtual bool canHaveChildren() const { return false; }
	
protected:
	
private:
	FSLayer *_parent;	// 0 : root item
	FSLayerList _childrenList;
	
	QString _name;
	bool _isLocked, _isVisible;
	double _opacity;
	MLBlendMode _blendMode;
	QPixmap _thumbnail;
	
	bool _isThumbnailDirty;
};

#endif // FSLAYER_H
