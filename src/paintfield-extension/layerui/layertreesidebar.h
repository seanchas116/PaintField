#ifndef FSLAYERTREEPANEL_H
#define FSLAYERTREEPANEL_H

#include "paintfield-core/layer.h"
#include "paintfield-core/widgets/loosespinbox.h"

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
class Workspace;
class Canvas;
class LayerUIController;

class LayerModelView;
class DoubleSlider;
class SimpleButton;

class LayerTreeSidebar : public QWidget
{
	Q_OBJECT
	
public:
	
	explicit LayerTreeSidebar(LayerUIController *layerUIController, QWidget *parent = 0);
	
public slots:
	
signals:
	
protected:
	
private slots:
	
	void updatePropertyView();
	void setOpacityPercentage(double value);
	void viewFocused();
	
private:
	
	void createForms();
	
	LayerUIController *_layerUIController = 0;
	
	Canvas *_canvas = 0;
	
	// forms
	
	LayerModelView *_treeView = 0;
	
	DoubleSlider *_opacitySlider = 0;
	LooseSpinBox *_opacitySpinBox = 0;
	
	QComboBox *_blendModeComboBox = 0;
	
	QWidget *_formWidget = 0;
};

}

#endif // FSLAYERTREEPANEL_H
