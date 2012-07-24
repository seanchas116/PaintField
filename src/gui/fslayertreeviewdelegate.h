#ifndef FSLAYERTREEVIEWDELEGATE_H
#define FSLAYERTREEVIEWDELEGATE_H

#include <QStyledItemDelegate>

class FSLayerTreePanel;

class FSLayerTreeViewDelegate : public QStyledItemDelegate
{
	Q_OBJECT
public:
	explicit FSLayerTreeViewDelegate(FSLayerTreePanel *panel, QObject *parent = 0);
	
	bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);
	
signals:
	
public slots:
	
private:
	FSLayerTreePanel *_panel;
};

#endif // FSLAYERTREEVIEWDELEGATE_H
