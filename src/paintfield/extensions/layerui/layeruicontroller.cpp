#include <QAction>
#include <QFileDialog>
#include <QDir>
#include <Malachite/ImageIO>

#include "paintfield/core/util.h"

#include "layeruicontroller.h"

namespace PaintField
{

LayerUIController::LayerUIController(Canvas *parent) :
    QObject(parent),
    _canvas(parent)
{
	_importAction = Util::createAction("paintfield.layer.import", this, SLOT(importLayer()));
	_importAction->setText(tr("Import..."));
	
	_newRasterAction = Util::createAction("paintfield.layer.newRaster", this, SLOT(newRasterLayer()));
	_newRasterAction->setText(tr("New Layer"));
	
	_newGroupAction = Util::createAction("paintfield.layer.newGroup", this, SLOT(newGroupLayer()));
	_newGroupAction->setText(tr("New Group"));
	
	//_actionsForLayers << actionManager->addAction("paintfield.layer.remove", this, SLOT(removeLayers()));
	
	_mergeAction = Util::createAction("paintfield.layer.merge", this, SLOT(mergeLayers()));
	_mergeAction->setText(tr("Merge"));
}

void LayerUIController::importLayer()
{
	QString filePath = QFileDialog::getOpenFileName(0,
													QObject::tr("Add Layer From Image File"),
													QDir::homePath(),
													QObject::tr("Image Files (*.bmp *.png *.jpg *.jpeg)"));
	if (filePath.isEmpty())
		return;
	
	auto layer = Layer::createFromImageFile(filePath);
	
	if (layer)
		addLayer(layer, tr("Add From Image File"));
}

void LayerUIController::newRasterLayer()
{
	addLayer(new RasterLayer(tr("New Layer")), tr("Add Layer"));
}

void LayerUIController::newGroupLayer()
{
	addLayer(new GroupLayer(tr("New Group")), tr("Add Group"));
}

void LayerUIController::removeLayers()
{
	_canvas->layerModel()->removeLayers(_canvas->selectionModel()->selectedIndexes());
}

void LayerUIController::mergeLayers()
{
	QItemSelection selection = _canvas->selectionModel()->selection();
	
	if (selection.size() == 1)
	{
		QItemSelectionRange range = selection.at(0);
		_canvas->layerModel()->mergeLayers(range.parent(), range.top(), range.bottom());
	}
}

void LayerUIController::addLayer(Layer *layer, const QString &description)
{
	auto index = _canvas->selectionModel()->currentIndex();
	int row = index.isValid() ? index.row() : _canvas->layerModel()->rowCount(QModelIndex());
	auto parent = index.isValid() ? index.parent() : QModelIndex();
	_canvas->layerModel()->addLayers({layer}, parent, row, description);
}

void LayerUIController::onSelectionChanged(const QItemSelection &selection)
{
	_mergeAction->setEnabled(selection.size() == 1);
	setActionsEnabled(_actionsForLayers, selection.size());
}

void LayerUIController::setActionsEnabled(const QList<QAction *> &actions, bool enabled)
{
	for (QAction *action : actions)
		action->setEnabled(enabled);
}

}
