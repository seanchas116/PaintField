#include <QtGui>
#include <Malachite/ImageIO>

#include "paintfield-core/util.h"

#include "layeractioncontroller.h"

namespace PaintField
{

LayerActionController::LayerActionController(CanvasController *parent) :
    QObject(parent),
    _canvas(parent)
{
	_importAction = createAction("paintfield.layer.import", this, SLOT(importLayer()));
	_importAction->setText(tr("Import..."));
	_newRasterAction = createAction("paintfield.layer.newRaster", this, SLOT(newRasterLayer()));
	_newRasterAction->setText(tr("New Layer"));
	_newGroupAction = createAction("paintfield.layer.newGroup", this, SLOT(newGroupLayer()));
	_newGroupAction->setText(tr("New Group"));
	//_actionsForLayers << actionManager->addAction("paintfield.layer.remove", this, SLOT(removeLayers()));
	_mergeAction = createAction("paintfield.layer.merge", this, SLOT(mergeLayers()));
	_mergeAction->setText(tr("Merge"));
}

void LayerActionController::importLayer()
{
	QString filePath = QFileDialog::getOpenFileName(0,
													QObject::tr("Add Layer From Image File"),
													QDir::homePath(),
													QObject::tr("Image Files (*.bmp *.png *.jpg *.jpeg)"));
	if (filePath.isEmpty())
		return;
	
	auto layer = Layer::createFromImageFile(filePath);
	if (!layer)
		return;
	
	QModelIndex index = _canvas->selectionModel()->currentIndex();
	int row = index.isValid() ? index.row() + 1 : _canvas->layerModel()->rowCount(QModelIndex());
	_canvas->layerModel()->addLayer(layer, index.parent(), row, tr("Add Image"));
}

void LayerActionController::newLayer(Layer::Type type)
{
	QModelIndex index = _canvas->selectionModel()->currentIndex();
	int row = index.isValid() ? index.row() + 1 : _canvas->layerModel()->rowCount(QModelIndex());
	_canvas->layerModel()->newLayer(type, index.parent(), row);
}

void LayerActionController::removeLayers()
{
	_canvas->layerModel()->removeLayers(_canvas->selectionModel()->selectedIndexes());
}

void LayerActionController::mergeLayers()
{
	QItemSelection selection = _canvas->selectionModel()->selection();
	
	if (selection.size() == 1)
	{
		QItemSelectionRange range = selection.at(0);
		_canvas->layerModel()->mergeLayers(range.parent(), range.top(), range.bottom());
	}
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
