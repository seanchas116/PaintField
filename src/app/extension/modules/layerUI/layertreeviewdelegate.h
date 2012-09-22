#ifndef FSLAYERTREEVIEWDELEGATE_H
#define FSLAYERTREEVIEWDELEGATE_H

#include <QStyledItemDelegate>

namespace PaintField
{

class LayerTreeWidget;

class LayerTreeViewDelegate : public QStyledItemDelegate
{
	Q_OBJECT
public:
	explicit LayerTreeViewDelegate(LayerTreeWidget *panel, QObject *parent = 0);
	
	bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);
	
signals:
	
public slots:
	
private:
	LayerTreeWidget *_panel;
};

}

#endif // FSLAYERTREEVIEWDELEGATE_H
