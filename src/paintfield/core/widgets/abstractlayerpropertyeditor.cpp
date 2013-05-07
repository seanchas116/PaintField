#include "../layerscene.h"

#include "abstractlayerpropertyeditor.h"

namespace PaintField {

struct AbstractLayerPropertyEditor::Data
{
	LayerScene *scene = 0;
	LayerConstPtr current;
};

AbstractLayerPropertyEditor::AbstractLayerPropertyEditor(LayerScene *layerScene, QWidget *parent) :
	QWidget(parent),
	d(new Data)
{
	d->scene = layerScene;
	
	if (layerScene)
	{
		connect(layerScene, SIGNAL(currentChanged(LayerConstPtr,LayerConstPtr)), this, SLOT(onCurrentChanged(LayerConstPtr)));
		connect(layerScene, SIGNAL(currentLayerChanged()), this, SLOT(onCurrentPropertyChanged()));
		d->current = layerScene->current();
	}
}

AbstractLayerPropertyEditor::~AbstractLayerPropertyEditor()
{
	delete d;
}

LayerScene *AbstractLayerPropertyEditor::layerScene()
{
	return d->scene;
}

LayerConstPtr AbstractLayerPropertyEditor::current()
{
	return d->current;
}

QVariant AbstractLayerPropertyEditor::currentProperty(int role) const
{
	if (d->current)
		return d->current->property(role);
	else
		return QVariant();
}

void AbstractLayerPropertyEditor::setCurrentProperty(const QVariant &data, int role, const QString &description)
{
	if (d->current)
		d->scene->setLayerProperty(d->current, data, role, description);
}

void AbstractLayerPropertyEditor::onCurrentChanged(const LayerConstPtr &current)
{
	d->current = current;
	updateForCurrentChange(current);
}

void AbstractLayerPropertyEditor::onCurrentPropertyChanged()
{
	updateForCurrentPropertyChange();
}

void AbstractLayerPropertyEditor::updateForCurrentChange(const LayerConstPtr &current)
{
	Q_UNUSED(current)
	updateForCurrentPropertyChange();
}

} // namespace PaintField
