#ifndef LAYERMODELVIEW_H
#define LAYERMODELVIEW_H

#include <QTreeView>

namespace PaintField
{

class LayerModelView : public QTreeView
{
	Q_OBJECT
public:
	explicit LayerModelView(QWidget *parent = 0) : QTreeView(parent) {}
	
signals:
	
	void windowFocused();
	
public slots:
	
protected:
	
	void focusInEvent(QFocusEvent *event);
};

}

#endif // LAYERMODELVIEW_H
