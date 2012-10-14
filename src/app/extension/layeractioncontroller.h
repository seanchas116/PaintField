#ifndef FSLAYERUICONTROLLER_H
#define FSLAYERUICONTROLLER_H

#include <QObject>
#include "core/workspacecontroller.h"

namespace PaintField
{

class LayerActionController : public QObject
{
	Q_OBJECT
public:
	
	LayerActionController(CanvasController *parent);
	
	QAction *importAction() { return _importAction; }
	QAction *newRasterAction() { return _newRasterAction; }
	QAction *newGroupAction() { return _newGroupAction; }
	QAction *removeAction() { return _removeAction; }
	QAction *mergeAction() { return _mergeAction; }
	
signals:
	
public slots:
	
	void importLayer();
	void newRasterLayer() { newLayer(Layer::TypeRaster); }
	void newGroupLayer() { newLayer(Layer::TypeGroup); }
	void removeLayers();
	void mergeLayers();
	
private slots:
	
	void onCanvasChanged(CanvasController *controller);
	void onSelectionChanged(const QItemSelection &selection);
	
private:
	
	void newLayer(Layer::Type type);
	void setActionsEnabled(const QList<QAction *> &actions, bool enabled);
	
	LayerModel *_model = nullptr;
	
	QAction *_importAction = nullptr, *_newRasterAction = nullptr, *_newGroupAction = nullptr, *_removeAction = nullptr, *_mergeAction = nullptr;
	
	QList<QAction *> _actionsForLayers;
};

}

#endif // FSLAYERUICONTROLLER_H
