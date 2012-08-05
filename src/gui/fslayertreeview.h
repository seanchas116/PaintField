#ifndef FSLAYERTREEVIEW_H
#define FSLAYERTREEVIEW_H

#include <QTreeView>

class FSLayerTreeView : public QTreeView
{
	Q_OBJECT
public:
	explicit FSLayerTreeView(QWidget *parent = 0) : QTreeView(parent) {}
	
signals:
	
	void windowFocused();
	
public slots:
	
protected:
	
	void focusInEvent(QFocusEvent *event);
};

#endif // FSLAYERTREEVIEW_H
