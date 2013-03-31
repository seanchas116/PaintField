#include <QAction>
#include <QFileDialog>
#include <QDir>
#include <Malachite/ImageIO>
#include <QItemSelectionModel>
#include <QMimeData>
#include <QApplication>
#include <QClipboard>

#include "paintfield/core/layeritemmodel.h"
#include "paintfield/core/layerscene.h"
#include "paintfield/core/rasterlayer.h"
#include "paintfield/core/grouplayer.h"
#include "paintfield/core/util.h"
#include "paintfield/core/appcontroller.h"
#include "paintfield/core/settingsmanager.h"

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
	
	auto settingsManager = appController()->settingsManager();
	
	{
		auto a = Util::createAction("paintfield.layer.import", this, SLOT(importLayer()));
		a->setText(tr("Import..."));
		d->actions[ActionImport] = a;
	}
	
	{
		auto a = Util::createAction("paintfield.layer.newRaster", this, SLOT(newRasterLayer()));
		a->setText(tr("New Layer"));
		a->setShortcut(settingsManager->value({".key-bindings", "paintfield.layer.newRaster"}).toString());
		d->actions[ActionNewRaster] = a;
	}
	
	{
		auto a = Util::createAction("paintfield.layer.newGroup", this, SLOT(newGroupLayer()));
		a->setText(tr("New Group"));
		a->setShortcut(settingsManager->value({".key-bindings", "paintfield.layer.newGroup"}).toString());
		d->actions[ActionNewGroup] = a;
	}
	
	{
		auto a = Util::createAction("paintfield.layer.remove", this, SLOT(removeLayers()));
		a->setText(tr("Delete"));
		a->setShortcut(settingsManager->value({".key-bindings", "paintfield.edit.delete"}).toString());
		d->actions[ActionRemove] = a;
		d->actionsForLayers << a;
	}
	
	{
		auto a = Util::createAction("paintfield.layer.merge", this, SLOT(mergeLayers()));
		a->setText(tr("Merge"));
		a->setShortcut(settingsManager->value({".key-bindings", "paintfield.layer.merge"}).toString());
		d->actions[ActionMerge] = a;
	}
	
	{
		auto a = Util::createAction("paintfield.layer.copy", this, SLOT(copyLayers()));
		a->setText(tr("Copy"));
		a->setShortcut(settingsManager->value({".key-bindings", "paintfield.edit.copy"}).toString());
		d->actions[ActionCopy] = a;
		d->actionsForLayers << a;
	}
	
	{
		auto a = Util::createAction("paintfield.layer.cut", this, SLOT(cutLayers()));
		a->setText(tr("Cut"));
		a->setShortcut(settingsManager->value({".key-bindings", "paintfield.edit.cut"}).toString());
		d->actions[ActionCut] = a;
		d->actionsForLayers << a;
	}
	
	{
		auto a = Util::createAction("paintfield.layer.paste", this, SLOT(pasteLayers()));
		a->setText(tr("Paste"));
		a->setShortcut(settingsManager->value({".key-bindings", "paintfield.edit.paste"}).toString());
		d->actions[ActionPaste] = a;
	}
	
	connect(document->layerScene()->itemSelectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(onSelectionChanged(QItemSelection)));
	onSelectionChanged(document->layerScene()->itemSelectionModel()->selection());
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
		addLayers( { layer} , tr("Add From Image File") );
}

void LayerUIController::newRasterLayer()
{
	addLayers( { new RasterLayer(tr("New Layer") ) }, tr("Add Layer") );
}

void LayerUIController::newGroupLayer()
{
	addLayers( { new GroupLayer(tr("New Group") ) }, tr("Add Group") );
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

static const QString layersMimeType = "application/x-paintfield-layers";

void LayerUIController::copyLayers()
{
	copyOrCutLayers(false);
}

void LayerUIController::cutLayers()
{
	copyOrCutLayers(true);
}

void LayerUIController::copyOrCutLayers(bool cut)
{
	auto layers = d->document->layerScene()->selection();
	if (layers.size() == 0)
		return;
	
	auto mime = new QMimeData();
	
	{
		QByteArray data;
		QDataStream stream(&data, QIODevice::WriteOnly);
		
		stream << layers.size();
		
		for (auto layer : layers)
			layer->encodeRecursive(stream);
		
		mime->setData(layersMimeType, data);
	}
	
	qApp->clipboard()->setMimeData(mime);
	
	if (cut)
		d->document->layerScene()->removeLayers(layers, tr("Cut Layers"));
}

void LayerUIController::pasteLayers()
{
	auto mime = qApp->clipboard()->mimeData();
	if (mime->hasFormat(layersMimeType))
	{
		QByteArray data = mime->data(layersMimeType);
		QDataStream stream(&data, QIODevice::ReadOnly);
		
		int count;
		stream >> count;
		
		if (count == 0)
			return;
		
		LayerList layers;
		layers.reserve(count);
		
		for (int i = 0; i < count; ++i)
			layers << Layer::decodeRecursive(stream);
		
		addLayers(layers, tr("Paste Layers"));
	}
}

void LayerUIController::addLayers(const LayerList &layers, const QString &description)
{
	auto scene = d->document->layerScene();
	
	auto current = scene->current();
	int row = current ? current.index() : scene->rootLayer().count();
	auto parent = current ? current.parent() : scene->rootLayer();
	scene->addLayers(layers, parent, row, description);
}

void LayerUIController::onSelectionChanged(const QItemSelection &selection)
{
	d->actions[ActionMerge]->setEnabled(selection.size() == 1);
	
	for (auto action : d->actionsForLayers)
		action->setEnabled(selection.size());
}

}
