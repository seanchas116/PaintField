#ifndef FSLAYEREDIT_H
#define FSLAYEREDIT_H

#include <QVariant>
#include "fslayer.h"

class FSLayerEdit
{
public:
	FSLayerEdit() {}
	virtual ~FSLayerEdit() {}
	
	virtual void saveUndoState(const FSLayer *layer);
	virtual void redo(FSLayer *layer);
	virtual void undo(FSLayer *layer);
	
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

class FSLayerPropertyEdit : public FSLayerEdit
{
public:
	FSLayerPropertyEdit(const QVariant &property, int role);
	void saveUndoState(const FSLayer *layer);
	void redo(FSLayer *layer);
	void undo(FSLayer *layer);
	
private:
	QVariant _newProperty, _oldProperty;
	int _role;
};

class FSLayerSurfaceEdit : public FSLayerEdit
{
public:
	FSLayerSurfaceEdit(const MLSurface &surface, const QPointSet &tileKeys);
	void saveUndoState(const FSLayer *layer);
	void redo(FSLayer *layer);
	void undo(FSLayer *layer);
	
private:
	MLSurface _surface;
	MLSurface _oldSurface;
};

#endif // FSLAYEREDIT_H
