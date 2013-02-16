#ifndef FSLAYERTREEVIEWDELEGATE_H
#define FSLAYERTREEVIEWDELEGATE_H

#include <QStyledItemDelegate>

namespace PaintField
{

class LayerUIController;

class LayerModelViewDelegate : public QStyledItemDelegate
{
	typedef QStyledItemDelegate super;
	
	Q_OBJECT
public:
	explicit LayerModelViewDelegate(LayerUIController *actionController, QObject *parent = 0);
	
	bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);
	
signals:
	
public slots:
	
private:
	LayerUIController *_actionController;
};

}

#endif // FSLAYERTREEVIEWDELEGATE_H
