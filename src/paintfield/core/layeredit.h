#ifndef FSLAYEREDIT_H
#define FSLAYEREDIT_H

#include <QVariant>
#include "layer.h"

namespace PaintField {

class LayerEdit
{
public:
	LayerEdit() {}
	virtual ~LayerEdit() {}
	
	virtual void redo(const LayerRef &layer);
	virtual void undo(const LayerRef &layer);
	
	void setName(const QString &name) { _name = name; }
	QString name() const { return _name; }
	
	void setModifiedKeys(const QPointSet &keys) { _modifiedKeys = keys; }
	
	/**
	  Returns the tile keys edited by this edit.
	  The document and the canvas try to update tiles specified by this when this edit is commited.
	  If this is empty, they will try to update all visible tiles.
	*/
	QPointSet modifiedKeys() const { return _modifiedKeys; }
	
private:
	QString _name;
	QPointSet _modifiedKeys;
};

class LayerPropertyEdit : public LayerEdit
{
public:
	
	LayerPropertyEdit(const LayerRef &layer, const QVariant &property, int role);
	void redo(const LayerRef &layer);
	void undo(const LayerRef &layer);
	
private:
	void change(const LayerRef &layer);
	
	QVariant _newProperty;
	int _role;
};

class LayerSurfaceEdit : public LayerEdit
{
public:
	LayerSurfaceEdit(const Malachite::Surface &surface, const QPointSet &tileKeys);
	void redo(const LayerRef &layer);
	void undo(const LayerRef &layer);
	
private:
	Malachite::Surface _surface;
};

class LayerMoveEdit : public LayerEdit
{
public:
	explicit LayerMoveEdit(const QPoint &offset)
	    : LayerEdit(),
		  _offset(offset)
	{}
	
	void redo(const LayerRef &layer);
	void undo(const LayerRef &layer);
	
private:
	QPoint _offset;
};

}

#endif // FSLAYEREDIT_H
