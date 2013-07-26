#pragma once

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
	~LayerModelViewDelegate();
	
	bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override;
	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
	QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
	QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
	void setEditorData(QWidget *editor, const QModelIndex &index) const override;
	
signals:
	
public slots:
	
private:
	
	struct Data;
	Data *d;
};

}

