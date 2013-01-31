#ifndef FSLAYERUICONTROLLER_H
#define FSLAYERUICONTROLLER_H

#include <QObject>
#include "paintfield-core/workspacecontroller.h"
#include "paintfield-core/canvascontroller.h"

namespace PaintField
{

class LayerUIController : public QObject
{
	Q_OBJECT
public:
	
	LayerUIController(CanvasController *parent);
	
	QAction *importAction() { return _importAction; }
	QAction *newRasterAction() { return _newRasterAction; }
	QAction *newGroupAction() { return _newGroupAction; }
	QAction *removeAction() { return _removeAction; }
	QAction *mergeAction() { return _mergeAction; }
	
	CanvasController *canvas() { return static_cast<CanvasController *>(parent()); }
	
signals:
	
public slots:
	
	void importLayer();
	void newRasterLayer() { newLayer(Layer::TypeRaster); }
	void newGroupLayer() { newLayer(Layer::TypeGroup); }
	void removeLayers();
	void mergeLayers();
	
private slots:
	
	void onSelectionChanged(const QItemSelection &selection);
	
private:
	
	void newLayer(Layer::Type type);
	void setActionsEnabled(const QList<QAction *> &actions, bool enabled);
	
	CanvasController *_canvas = nullptr;
	
	QAction *_importAction = nullptr, *_newRasterAction = nullptr, *_newGroupAction = nullptr, *_removeAction = nullptr, *_mergeAction = nullptr;
	
	QList<QAction *> _actionsForLayers;
};

}

#endif // FSLAYERUICONTROLLER_H
