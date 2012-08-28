#ifndef FSLAYERMOVETOOL_H
#define FSLAYERMOVETOOL_H

#include <QObject>
#include "fstool.h"
#include "fslayeredit.h"

class FSRasterLayer;

class FSLayerMoveEdit : public FSLayerEdit
{
public:
	explicit FSLayerMoveEdit(const QPoint &offset)
	    : FSLayerEdit(),
		  _offset(offset)
	{}
	
	void redo(FSLayer *layer);
	void undo(FSLayer *layer);
	
private:
	QPoint _offset;
};

class FSLayerMoveRenderDelegate;

class FSLayerMoveTool : public FSTool
{
	Q_OBJECT
public:
	explicit FSLayerMoveTool(FSCanvas *parent = 0);
	~FSLayerMoveTool();
	
	void render(MLPainter *painter, const FSLayer *layer, const QPoint &tileKey);
	
	FSLayerRenderDelegate *renderDelegate();
	
signals:
	
public slots:
	
protected:
	
	void cursorPressEvent(FSTabletEvent *event);
	void cursorMoveEvent(FSTabletEvent *event);
	void cursorReleaseEvent(FSTabletEvent *event);
	
	void beginMoveLayer();
	void moveLayer(FSTabletEvent *event);
	void endMoveLayer();
	
private:
	
	QScopedPointer<FSLayerMoveRenderDelegate> _delegate;
	const FSLayer *_layer;
	QPoint _dragStartPoint;
	QPointSet _lastKeys;
	bool _layerIsDragged;
};

class FSLayerMoveToolFactory : public FSToolFactory
{
	Q_OBJECT
public:
	explicit FSLayerMoveToolFactory(QObject *parent = 0);
	
	FSTool *createTool(FSCanvas *view) { return new FSLayerMoveTool(view); }
	bool isTypeSupported(FSLayer::Type type) const;
};

#endif // FSLAYERMOVETOOL_H
