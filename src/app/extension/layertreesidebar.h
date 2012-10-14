#ifndef FSLAYERTREEPANEL_H
#define FSLAYERTREEPANEL_H

#include "core/layer.h"
#include "core/widgets/loosespinbox.h"

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
class WorkspaceController;

class LayerModelView;
class DoubleSlider;
class SimpleButton;

class LayerTreeSidebar : public QWidget
{
	Q_OBJECT
	
public:
	
	explicit LayerTreeSidebar(LayerModel *model, QWidget *parent = 0);
	
	SimpleButton *addButton() { return _addButton; }
	SimpleButton *removeButton() { return _removeButton; }
	SimpleButton *miscButton() { return _miscButton; }
	
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
	
	LayerModel *_layerModel = 0;
	
	// forms
	
	LayerModelView *_treeView = 0;
	
	DoubleSlider *_opacitySlider = 0;
	LooseSpinBox *_opacitySpinBox = 0;
	
	QComboBox *_blendModeComboBox = 0;
	
	QWidget *_formWidget = 0;
	
	SimpleButton *_addButton = 0, *_removeButton = 0, *_miscButton = 0;
};

}

#endif // FSLAYERTREEPANEL_H
