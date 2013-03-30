#ifndef FSLAYERTREEPANEL_H
#define FSLAYERTREEPANEL_H

#include "paintfield/core/layer.h"
#include "paintfield/core/widgets/loosespinbox.h"

class QHBoxLayout;
class QVBoxLayout;
class QMenu;
class QItemSelection;
class QModelIndex;
class QComboBox;
class QFormLayout;

namespace PaintField
{

class Document;
class Workspace;
class Canvas;
class LayerUIController;

class DoubleSlider;
class SimpleButton;

class LayerTreeSidebar : public QWidget
{
	Q_OBJECT
	
public:
	
	explicit LayerTreeSidebar(LayerUIController *layerUIController, QWidget *parent = 0);
	~LayerTreeSidebar();
	
public slots:
	
signals:
	
protected:
	
private slots:
	
	void onCurrentChanged(const LayerRef &current);
	void updateView();
	void setOpacityPercentage(double value);
	
private:
	
	void createForms();
	
	struct Data;
	Data *d;
};

}

#endif // FSLAYERTREEPANEL_H
