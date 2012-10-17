#include <QtGui>
#include "Malachite/mlimageio.h"

#include "core/actionmanager.h"

#include "layeractioncontroller.h"

namespace PaintField
{

LayerActionController::LayerActionController(CanvasController *parent) :
    QObject(parent),
    _model(parent->document()->layerModel())
{
	ActionManager *actionManager = parent->workspace()->actionManager();
	
	_importAction = actionManager->addAction("paintfield.layer.import", this, SLOT(importLayer()));
	_newRasterAction = actionManager->addAction("paintfield.layer.newRaster", this, SLOT(newRasterLayer()));
	_newGroupAction = actionManager->addAction("paintfield.layer.newGroup", this, SLOT(newGroupLayer()));
	//_actionsForLayers << actionManager->addAction("paintfield.layer.remove", this, SLOT(removeLayers()));
	_mergeAction = actionManager->addAction("paintfield.layer.merge", this, SLOT(mergeLayers()));
}

void LayerActionController::importLayer()
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

void LayerActionController::newLayer(Layer::Type type)
{
	QModelIndex index = _model->currentIndex();
	int row = index.isValid() ? index.row() + 1 : _model->rowCount(QModelIndex());
	_model->newLayer(type, index.parent(), row);
}

void LayerActionController::removeLayers()
{
	_model->removeLayers(_model->selectionModel()->selectedIndexes());
}

void LayerActionController::mergeLayers()
{
	QItemSelection selection = _model->selectionModel()->selection();
	
	if (selection.size() == 1)
	{
		QItemSelectionRange range = selection.at(0);
		_model->mergeLayers(range.parent(), range.top(), range.bottom());
	}
}

void LayerActionController::onCanvasChanged(CanvasController *controller)
{
	if (_model)
		disconnect(_model->selectionModel(), 0, this, 0);
	
	_model = controller->document()->layerModel();
	
	connect(_model->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(onSelectionChanged(QItemSelection)));
	onSelectionChanged(_model->selectionModel()->selection());
}

void LayerActionController::onSelectionChanged(const QItemSelection &selection)
{
	_mergeAction->setEnabled(selection.size() == 1);
	setActionsEnabled(_actionsForLayers, selection.size());
}

void LayerActionController::setActionsEnabled(const QList<QAction *> &actions, bool enabled)
{
	for (QAction *action : actions)
		action->setEnabled(enabled);
}

}
