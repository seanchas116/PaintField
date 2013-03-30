#include <QAction>
#include <QFileDialog>
#include <QDir>
#include <Malachite/ImageIO>
#include <QItemSelectionModel>

#include "paintfield/core/layeritemmodel.h"
#include "paintfield/core/layerscene.h"
#include "paintfield/core/rasterlayer.h"
#include "paintfield/core/grouplayer.h"
#include "paintfield/core/util.h"

#include "layeruicontroller.h"

namespace PaintField
{

struct LayerUIController::Data
{
	QHash<ActionType, QAction *> actions;
	Document *document = 0;
	QList<QAction *> actionsForLayers;
};

LayerUIController::LayerUIController(Document *document, QObject *parent) :
    QObject(parent),
	d(new Data)
{
	d->document = document;
	
	{
		auto a = Util::createAction("paintfield.layer.import", this, SLOT(importLayer()));
		a->setText(tr("Import..."));
		d->actions[ActionImport] = a;
	}
	
	{
		auto a = Util::createAction("paintfield.layer.newRaster", this, SLOT(newRasterLayer()));
		a->setText(tr("New Layer"));
		d->actions[ActionNewRaster] = a;
	}
	
	{
		auto a = Util::createAction("paintfield.layer.newGroup", this, SLOT(newGroupLayer()));
		a->setText(tr("New Group"));
		d->actions[ActionNewGroup] = a;
	}
	
	{
		auto a = Util::createAction("paintfield.layer.remove", this, SLOT(removeLayers()));
		a->setText(tr("Remove"));
		d->actions[ActionRemove] = a;
	}
	
	{
		auto a = Util::createAction("paintfield.layer.merge", this, SLOT(mergeLayers()));
		a->setText(tr("Merge"));
		d->actions[ActionMerge] = a;
	}
	
	connect(document->layerScene()->itemSelectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(onSelectionChanged(QItemSelection)));
}

LayerUIController::~LayerUIController()
{
	delete d;
}

QAction *LayerUIController::action(ActionType type)
{
	return d->actions.value(type, 0);
}

QList<QAction *> LayerUIController::actions()
{
	return d->actions.values();
}

Document *LayerUIController::document()
{
	return d->document;
}

void LayerUIController::importLayer()
{
	QString filePath = QFileDialog::getOpenFileName(0,
													QObject::tr("Add Layer From Image File"),
													QDir::homePath(),
													QObject::tr("Image Files (*.bmp *.png *.jpg *.jpeg)"));
	if (filePath.isEmpty())
		return;
	
	auto layer = RasterLayer::createFromImageFile(filePath);
	
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
	d->document->layerScene()->removeLayers(d->document->layerScene()->selection());
}

void LayerUIController::mergeLayers()
{
	auto selection = d->document->layerScene()->itemSelectionModel()->selection();
	
	if (selection.size() == 1)
	{
		QItemSelectionRange range = selection.at(0);
		d->document->layerScene()->mergeLayers(d->document->layerScene()->itemModel()->layerForIndex(range.parent()), range.top(), range.bottom() - range.top());
	}
}

void LayerUIController::addLayer(Layer *layer, const QString &description)
{
	auto scene = d->document->layerScene();
	
	auto current = scene->current();
	int row = current ? current.index() : scene->rootLayer().count();
	auto parent = current ? current.parent() : scene->rootLayer();
	scene->addLayers({layer}, parent, row, description);
}

void LayerUIController::onSelectionChanged(const QItemSelection &selection)
{
	d->actions[ActionMerge]->setEnabled(selection.size() == 1);
	
	for (auto action : d->actionsForLayers)
		action->setEnabled(selection.size());
}

}
