#include <QtGui>
#include "core/layermodel.h"
#include "layeractioncontroller.h"
#include "layermodelviewdelegate.h"

namespace PaintField
{

LayerModelViewDelegate::LayerModelViewDelegate(LayerActionController *actionController, QObject *parent) :
	QStyledItemDelegate(parent),
	_actionController(actionController)
{
}

bool LayerModelViewDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
	Q_UNUSED(option);
	
	if (event->type() == QEvent::MouseButtonPress && static_cast<QMouseEvent *>(event)->button() == Qt::RightButton)
	{
		LayerModel *layerModel = qobject_cast<LayerModel *>(model);
		Q_ASSERT(layerModel);
		
		layerModel->selectionModel()->select(index, QItemSelectionModel::Select);
		
		QMenu menu;
		
		menu.addAction(tr("Remove"), _actionController, SLOT(removeLayer()));
	}
	
	return true;
}

}
