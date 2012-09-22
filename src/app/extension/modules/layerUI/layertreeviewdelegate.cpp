#include <QtGui>
#include "modules/document/layermodel.h"
#include "layertreewidget.h"
#include "layertreeviewdelegate.h"

namespace PaintField
{

LayerTreeViewDelegate::LayerTreeViewDelegate(LayerTreeWidget *panel, QObject *parent) :
	QStyledItemDelegate(parent),
	_panel(panel)
{
}

bool LayerTreeViewDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
	Q_UNUSED(option);
	
	if (event->type() == QEvent::MouseButtonPress && static_cast<QMouseEvent *>(event)->button() == Qt::RightButton)
	{
		LayerModel *layerModel = qobject_cast<LayerModel *>(model);
		Q_ASSERT(layerModel);
		
		layerModel->selectionModel()->select(index, QItemSelectionModel::Select);
		
		QMenu menu;
		
		menu.addAction(tr("Remove"), _panel, SLOT(removeLayer()));
	}
	
	return true;
}

}
