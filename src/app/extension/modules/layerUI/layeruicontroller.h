#ifndef FSLAYERUICONTROLLER_H
#define FSLAYERUICONTROLLER_H

#include <QObject>
#include "../document/document.h"

namespace PaintField
{

class LayerUIController : public QObject
{
	Q_OBJECT
public:
	
	LayerUIController(LayerModel *model, QObject *parent = 0);
	
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
	
	LayerModel *_model;
	
	QList<QAction *> _actionsForLayers;
	QAction *_mergeLayersAction;
};

}

#endif // FSLAYERUICONTROLLER_H
