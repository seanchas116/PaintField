#ifndef FSLAYERTREEVIEW_H
#define FSLAYERTREEVIEW_H

#include <QTreeView>

namespace PaintField
{

class LayerTreeView : public QTreeView
{
	Q_OBJECT
public:
	explicit LayerTreeView(QWidget *parent = 0) : QTreeView(parent) {}
	
signals:
	
	void windowFocused();
	
public slots:
	
protected:
	
	void focusInEvent(QFocusEvent *event);
};

}

#endif // FSLAYERTREEVIEW_H
