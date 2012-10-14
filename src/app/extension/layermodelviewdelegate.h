#ifndef FSLAYERTREEVIEWDELEGATE_H
#define FSLAYERTREEVIEWDELEGATE_H

#include <QStyledItemDelegate>

namespace PaintField
{

class LayerTreePanel;

class LayerModelViewDelegate : public QStyledItemDelegate
{
	Q_OBJECT
public:
	explicit LayerModelViewDelegate(LayerTreePanel *panel, QObject *parent = 0);
	
	bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);
	
signals:
	
public slots:
	
private:
	LayerTreePanel *_panel;
};

}

#endif // FSLAYERTREEVIEWDELEGATE_H
