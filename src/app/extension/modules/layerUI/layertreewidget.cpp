#include <QtGui>

#include "layertreeview.h"
#include "widgets/doubleslider.h"
#include "widgets/simplebutton.h"
#include "core/application.h"
#include "modules/document/document.h"
#include "modules/document/rasterlayer.h"
#include "modules/action/actionmodule.h"
#include "mlpainter.h"
#include "mlimageio.h"

#include "layertreewidget.h"

namespace PaintField
{

LayerTreeWidget::LayerTreeWidget(LayerModel *layerModel, QWidget *parent) :
    QWidget(parent),
    _layerModel(layerModel)
{
	createForms();
	
	ActionManager *actionManager = ActionModule::actionManager();
	
	actionManager->connectTriggered("newLayer", this, SLOT(newLayer()));
	actionManager->connectTriggered("newGroup", this, SLOT(newGroup()));
	actionManager->connectTriggered("addLayerFromFile", this, SLOT(addImage()));
	
	// creating menus
	
	_layerAddMenu = new QMenu(this);
	
	_layerAddMenu->addAction(actionManager->action("newLayer"));
	_layerAddMenu->addAction(actionManager->action("newGroup"));
	_layerAddMenu->addAction(actionManager->action("addLayerFromFile"));
	
	_addButton->setMenu(_layerAddMenu);
	
	_miscMenu = new QMenu(this);
	_miscMenu->addAction(actionManager->action("mergeLayer"));
	
	_menuButton->setMenu(_miscMenu);
	
	connect(_removeButton, SIGNAL(pressed()), this, SLOT(removeLayer()));
	
	_treeView->setModel(_layerModel);
	if (_layerModel)
	{
		connect(_layerModel, SIGNAL(modified()), this, SLOT(updatePropertyView()));
		connect(_layerModel, SIGNAL(currentIndexChanged(QModelIndex,QModelIndex)), this, SLOT(updatePropertyView()));
		updatePropertyView();
		_treeView->setSelectionModel(_layerModel->selectionModel());
	}
	else
	{
		setEnabled(false);
	}
	
	updatePropertyView();
	
	setWindowTitle(tr("Layer"));
}

LayerTreeWidget::~LayerTreeWidget()
{
}

void LayerTreeWidget::updatePropertyView()
{
	if (_layerModel->currentIndex().isValid())
	{
		_formWidget->setEnabled(true);
		_opacitySpinBox->setValue(_layerModel->currentIndex().data(PaintField::RoleOpacity).toDouble() * 100.0);
	}
	else
	{
		_formWidget->setEnabled(false);
	}
}

void LayerTreeWidget::setOpacityPercentage(double value)
{
	_layerModel->setData(_layerModel->currentIndex(), value / 100.0, PaintField::RoleOpacity);
}

void LayerTreeWidget::viewFocused()
{
	_layerModel->updateDirtyThumbnails();
}

void LayerTreeWidget::createForms()
{
	_treeView = new LayerTreeView();
	_treeView->setHeaderHidden(true);
	_treeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
	_treeView->setDragDropMode(QAbstractItemView::DragDrop);
	_treeView->setDefaultDropAction(Qt::MoveAction);
	_treeView->setDropIndicatorShown(true);
	connect(_treeView, SIGNAL(windowFocused()), this, SLOT(viewFocused()));
	
	_opacitySlider = new DoubleSlider();
	_opacitySlider->setOrientation(Qt::Horizontal);
	_opacitySlider->setMinimum(0);
	_opacitySlider->setMaximum(1000);
	_opacitySlider->setDoubleMinimum(0.0);
	_opacitySlider->setDoubleMaximum(100.0);
	
	_opacitySpinBox = new LooseSpinBox();
	_opacitySpinBox->setDecimals(1);
	_opacitySpinBox->setMinimum(0);
	_opacitySpinBox->setMaximum(100);
	_opacitySpinBox->setSingleStep(1.0);
	
	connect(_opacitySlider, SIGNAL(doubleValueChanged(double)), _opacitySpinBox, SLOT(setValue(double)));
	connect(_opacitySpinBox, SIGNAL(valueChanged(double)), _opacitySlider, SLOT(setDoubleValue(double)));
	connect(_opacitySlider, SIGNAL(doubleValueChanged(double)), this, SLOT(setOpacityPercentage(double)));
	
	_opacityLayout = new QHBoxLayout();
	_opacityLayout->addWidget(_opacitySlider);
	_opacityLayout->addWidget(_opacitySpinBox);
	_opacityLayout->addWidget(new QLabel("%"));
	
	_blendModeComboBox = new QComboBox();
	
	_formLayout = new QFormLayout();
	_formLayout->addRow(tr("Opacity"), _opacityLayout);
	_formLayout->addRow(tr("Blend"), _blendModeComboBox);
	_formLayout->setHorizontalSpacing(5);
	_formLayout->setVerticalSpacing(5);
	_formLayout->setContentsMargins(5, 5, 5, 5);
	
	_formWidget = new QWidget();
	_formWidget->setLayout(_formLayout);
	
	_addButton = new SimpleButton(":/icons/16x16/add.svg");
	_addButton->setMargins(4, 0, 4, 0);
	_removeButton = new SimpleButton(":/icons/16x16/subtract.svg");
	_removeButton->setMargins(4, 0, 4, 0);
	_menuButton = new SimpleButton(":/icons/16x16/menuDown.svg");
	_menuButton->setMargins(4, 0, 4, 0);
	
	_buttonLayout = new QHBoxLayout();
	_buttonLayout->addWidget(_addButton);
	_buttonLayout->addWidget(_removeButton);
	_buttonLayout->addWidget(_menuButton);
	_buttonLayout->addStretch(1);
	_buttonLayout->setContentsMargins(5, 5, 5, 5);
	_buttonLayout->setSpacing(0);
	
	_propertyLayout = new QVBoxLayout();
	_propertyLayout->addWidget(_formWidget);
	_propertyLayout->addLayout(_buttonLayout);
	_propertyLayout->setContentsMargins(5, 5, 5, 5);
	
	_mainLayout = new QVBoxLayout();
	_mainLayout->addWidget(_treeView);
	_mainLayout->addLayout(_propertyLayout);
	_mainLayout->setContentsMargins(0, 0, 0, 0);
	_mainLayout->setSpacing(0);
	
	setLayout(_mainLayout);
}

}
