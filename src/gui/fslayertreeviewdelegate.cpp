#include <QtGui>
#include "fsdocumentmodel.h"
#include "fslayertreepanel.h"
#include "fslayertreeviewdelegate.h"

FSLayerTreeViewDelegate::FSLayerTreeViewDelegate(FSLayerTreePanel *panel, QObject *parent) :
	QStyledItemDelegate(parent),
	_panel(panel)
{
}

bool FSLayerTreeViewDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
	Q_UNUSED(option);
	
	if (event->type() == QEvent::MouseButtonPress && static_cast<QMouseEvent *>(event)->button() == Qt::RightButton)
	{
		FSDocumentModel *document = qobject_cast<FSDocumentModel *>(model);
		Q_ASSERT(document);
		
		document->selectionModel()->select(index, QItemSelectionModel::Select);
		
		QMenu menu;
		
		menu.addAction(tr("Remove"), _panel, SLOT(removeLayer()));
	}
	
	return true;
}
