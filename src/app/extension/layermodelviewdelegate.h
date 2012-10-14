#ifndef FSLAYERTREEVIEWDELEGATE_H
#define FSLAYERTREEVIEWDELEGATE_H

#include <QStyledItemDelegate>

namespace PaintField
{

class LayerActionController;

class LayerModelViewDelegate : public QStyledItemDelegate
{
	Q_OBJECT
public:
	explicit LayerModelViewDelegate(LayerActionController *actionController, QObject *parent = 0);
	
	bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);
	
signals:
	
public slots:
	
private:
	LayerActionController *_actionController;
};

}

#endif // FSLAYERTREEVIEWDELEGATE_H
