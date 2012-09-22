#include "mlimageio.h"

#include "../document/rasterlayer.h"
#include "../document/grouplayer.h"

#include "../action/actionmodule.h"

#include <QtGui>

#include "layeruicontroller.h"

namespace PaintField
{

LayerUIController::LayerUIController(LayerModel *model, QObject *parent) :
    QObject(parent),
    _model(model)
{
	ActionManager *actionManager = ActionModule::actionManager();
	
	actionManager->addAction(new QAction(this), "importLayer", this, SLOT(importLayer()));
	actionManager->addAction(new QAction(this), "newRasterLayer", this, SLOT(newRasterLayer()));
	actionManager->addAction(new QAction(this), "newGroupLayer", this, SLOT(newGroupLayer()));
	_actionsForLayers << actionManager->addAction(new QAction(this), "removeLayers", this, SLOT(removeLayers()));
	_mergeLayersAction = actionManager->addAction(new QAction(this), "mergeLayers", this, SLOT(mergeLayers()));
	
	connect(_model->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(onSelectionChanged(QItemSelection)));
	onSelectionChanged(_model->selectionModel()->selection());
}

void LayerUIController::importLayer()
{
	QString filePath = QFileDialog::getOpenFileName(0,
													QObject::tr("Add Layer From Image File"),
													QDir::homePath(),
													QObject::tr("Image Files (*.bmp *.png *.jpg *.jpeg)"));
	if (filePath.isEmpty())
		return;
	
	Malachite::ImageImporter importer(filePath);
	
	Malachite::Surface surface = importer.toSurface();
	if (surface.isNull())
		return;
	
	QFileInfo fileInfo(filePath);
	
	RasterLayer layer(fileInfo.fileName());
	layer.setSurface(surface);
	
	QModelIndex index = _model->currentIndex();
	int row = index.isValid() ? index.row() + 1 : _model->rowCount(QModelIndex());
	_model->addLayer(&layer, index.parent(), row, tr("Add Image"));
}

void LayerUIController::newLayer(Layer::Type type)
{
	QModelIndex index = _model->currentIndex();
	int row = index.isValid() ? index.row() + 1 : _model->rowCount(QModelIndex());
	_model->newLayer(type, index.parent(), row);
}

void LayerUIController::removeLayers()
{
	_model->removeLayers(_model->selectionModel()->selectedIndexes());
}

void LayerUIController::mergeLayers()
{
	QItemSelection selection = _model->selectionModel()->selection();
	
	if (selection.size() == 1)
	{
		QItemSelectionRange range = selection.at(0);
		_model->mergeLayers(range.parent(), range.top(), range.bottom());
	}
}

void LayerUIController::onSelectionChanged(const QItemSelection &selection)
{
	_mergeLayersAction->setEnabled(selection.size() == 1);
	setActionsEnabled(_actionsForLayers, selection.size());
}

void LayerUIController::setActionsEnabled(const QList<QAction *> &actions, bool enabled)
{
	foreach (QAction *action, actions)
		action->setEnabled(enabled);
}

}
