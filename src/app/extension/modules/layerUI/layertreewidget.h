#ifndef FSLAYERTREEPANEL_H
#define FSLAYERTREEPANEL_H

#include "../document/layer.h"
#include "widgets/loosespinbox.h"

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
class LayerModel;

class LayerTreeView;
class DoubleSlider;
class SimpleButton;

class LayerTreeWidget : public QWidget
{
	Q_OBJECT
	
public:
	
	explicit LayerTreeWidget(LayerModel *layerModel, QWidget *parent = 0);
	~LayerTreeWidget();
	
public slots:
	
signals:
	
protected:
	
private slots:
	
	void updatePropertyView();
	void setOpacityPercentage(double value);
	void viewFocused();
	
private:
	
	void setItemSelected(bool selected);
	
	void createForms();
	
	void newLayerItem(Layer::Type type);
	
	LayerModel *_layerModel;
	
	
	// forms
	
	LayerTreeView *_treeView;
	
	DoubleSlider *_opacitySlider;
	LooseSpinBox *_opacitySpinBox;
	
	QHBoxLayout *_opacityLayout;
	
	QComboBox *_blendModeComboBox;
	
	QFormLayout *_formLayout;
	QWidget *_formWidget;
	
	SimpleButton *_addButton, *_removeButton, *_menuButton;
	QHBoxLayout *_buttonLayout;
	
	QVBoxLayout *_propertyLayout;
	
	QVBoxLayout *_mainLayout;
	
	QMenu *_layerAddMenu, *_miscMenu;
};

}

#endif // FSLAYERTREEPANEL_H
