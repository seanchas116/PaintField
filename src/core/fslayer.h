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
	
	const FSLayer *root() const;
	FSLayer *root() { return const_cast<FSLayer *>(static_cast<const FSLayer *>(this)->root()); }
	
	bool isAncestorOf(const FSLayer *layer) const;
	
	bool contains(int row) const { return (0 <= row && row < _childrenList.size()) ? true : false; }
	bool insertable(int row) const { return (0 <= row && row <= _childrenList.size()) ? true : false; }
	
	int childCount() const { return _childrenList.size(); }
	int siblingCount() const { return _parent->childCount(); }
	int rowForChild(const FSLayer *child) const { return _childrenList.indexOf(const_cast<FSLayer *>(child)); }
	int row() const { return _parent ? _parent->rowForChild(this) : 0; }
	
	bool insertChild(int row, FSLayer *child);
	void prependChild(FSLayer *child) { insertChild(0, child); }
	void appendChild(FSLayer *child) { insertChild(childCount(), child); }
	FSLayer *takeChild(int row);
	FSLayer *replaceChild(int row, FSLayer *child);
	bool removeChild(int row);
	
	FSLayer *cloneRecursive();
	
	QString unduplicatedChildName(const QString &name) const;
	
	virtual bool setProperty(const QVariant &data, int role);
	virtual QVariant property(int role) const;
	
	void setName(const QString &name) { _name = name; }
	QString name() const { return _name; }
	void setVisible(bool visible) { _isVisible = visible; }
	bool visible() const { return _isVisible; }
	void setLocked(bool locked) { _isLocked = locked; }
	bool locked() const { return property(FSGlobal::RoleLocked).toBool(); }
	void setThumbnail(const QPixmap &thumbnail) { _thumbnail = thumbnail; }
	QPixmap thumbnail() const { return _thumbnail; }
	void setOpacity(double opacity) { _opacity = opacity; }
	double opacity() const { return _opacity; }
	void setBlendMode(MLBlendMode mode) { _blendMode = mode; }
	MLBlendMode blendMode() const { return _blendMode; }
	
	virtual void updateThumbnail(const QSize &size);
	void updateThumbnailRecursive(const QSize &size);
	
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
};

#endif // FSLAYER_H
