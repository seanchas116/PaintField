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
	
	LayerActionController(WorkspaceController *workspace, QObject *parent = 0);
	
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
	
	LayerModel *_model = 0;
	
	QAction *_importAction = 0, _newRasterAction = 0, _newGroupAction = 0, _removeAction = 0, _mergeAction = 0;
	
	QList<QAction *> _actionsForLayers;
};

}

#endif // FSLAYERUICONTROLLER_H
