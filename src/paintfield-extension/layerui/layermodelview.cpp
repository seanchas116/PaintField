#include "layermodelviewdelegate.h"

#include "layermodelview.h"

namespace PaintField
{

LayerModelView::LayerModelView(QWidget *parent) :
    QTreeView(parent)
{
}

void LayerModelView::focusInEvent(QFocusEvent *event)
{
	Q_UNUSED(event);
	emit windowFocused();
}

}

