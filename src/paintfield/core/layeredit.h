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
	
	virtual void redo(const LayerPtr &layer);
	virtual void undo(const LayerPtr &layer);
	
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
	
	LayerPropertyEdit(const LayerPtr &layer, const QVariant &property, int role);
	void redo(const LayerPtr &layer);
	void undo(const LayerPtr &layer);
	
private:
	void change(const LayerPtr &layer);
	
	QVariant _newProperty;
	int _role;
};

class LayerSurfaceEdit : public LayerEdit
{
public:
	LayerSurfaceEdit(const Malachite::Surface &surface, const QPointSet &tileKeys);
	void redo(const LayerPtr &layer);
	void undo(const LayerPtr &layer);
	
private:
	Malachite::Surface _surface;
};

}

#endif // FSLAYEREDIT_H
