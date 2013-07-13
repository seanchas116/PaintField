#ifndef LAYERPROPERTYEDITOR_H
#define LAYERPROPERTYEDITOR_H

#include <QWidget>
#include "paintfield/core/layer.h"

namespace PaintField
{

class LayerScene;

class LayerPropertyEditor : public QWidget
{
	Q_OBJECT
public:
	explicit LayerPropertyEditor(LayerScene *scene, QWidget *parent = 0);
	~LayerPropertyEditor();
	
signals:
	
public slots:
	
	void setCurrentLayer(const LayerConstRef &current);
	void updateEditor();
	
private slots:
	
	void setOpacityPercent(double value);
	
private:
	
	struct Data;
	Data *d;
};

}

#endif // LAYERPROPERTYEDITOR_H
