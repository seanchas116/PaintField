#include <QtGui>

#include "util.h"
#include "layermodelview.h"
#include "core/widgets/doubleslider.h"
#include "core/widgets/simplebutton.h"
#include "core/application.h"
#include "core/workspacecontroller.h"
#include "core/document.h"
#include "Malachite/mlpainter.h"
#include "Malachite/mlimageio.h"

#include "layertreesidebar.h"

namespace PaintField
{

LayerTreeSidebar::LayerTreeSidebar(LayerModel *model, QWidget *parent) :
    QWidget(parent),
    _layerModel(model)
{
	createForms();
	
	_treeView->setModel(_layerModel);
	
	if (_layerModel)
	{
		connect(_layerModel, SIGNAL(modified()), this, SLOT(updatePropertyView()));
		connect(_layerModel, SIGNAL(currentIndexChanged(QModelIndex,QModelIndex)), this, SLOT(updatePropertyView()));
		updatePropertyView();
		_treeView->setSelectionModel(_layerModel->selectionModel());
	}
	else
		setEnabled(false);
	
	updatePropertyView();
	
	setWindowTitle(tr("Layer"));
}

void LayerTreeSidebar::updatePropertyView()
{
	if (_layerModel->currentIndex().isValid())
	{
		_formWidget->setEnabled(true);
		_opacitySpinBox->setValue(_layerModel->currentIndex().data(PaintField::RoleOpacity).toDouble() * 100.0);
	}
	else
		_formWidget->setEnabled(false);
}

void LayerTreeSidebar::setOpacityPercentage(double value)
{
	_layerModel->setData(_layerModel->currentIndex(), value / 100.0, PaintField::RoleOpacity);
}

void LayerTreeSidebar::viewFocused()
{
	_layerModel->updateDirtyThumbnails();
}

void LayerTreeSidebar::createForms()
{
	_treeView = new LayerModelView();
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
	
	QHBoxLayout *opacityLayout = new QHBoxLayout();
	opacityLayout->addWidget(_opacitySlider);
	opacityLayout->addWidget(_opacitySpinBox);
	opacityLayout->addWidget(new QLabel("%"));
	
	_blendModeComboBox = new QComboBox();
	
	QFormLayout *formLayout = new QFormLayout();
	formLayout->addRow(tr("Opacity"), opacityLayout);
	formLayout->addRow(tr("Blend"), _blendModeComboBox);
	formLayout->setHorizontalSpacing(5);
	formLayout->setVerticalSpacing(5);
	formLayout->setContentsMargins(5, 5, 5, 5);
	
	_formWidget = new QWidget();
	_formWidget->setLayout(formLayout);
	
	_addButton = new SimpleButton(":/icons/16x16/add.svg");
	_addButton->setMargins(4, 0, 4, 0);
	_removeButton = new SimpleButton(":/icons/16x16/subtract.svg");
	_removeButton->setMargins(4, 0, 4, 0);
	_miscButton = new SimpleButton(":/icons/16x16/menuDown.svg");
	_miscButton->setMargins(4, 0, 4, 0);
	
	QHBoxLayout *buttonLayout = new QHBoxLayout();
	buttonLayout->addWidget(_addButton);
	buttonLayout->addWidget(_removeButton);
	buttonLayout->addWidget(_miscButton);
	buttonLayout->addStretch(1);
	buttonLayout->setContentsMargins(5, 5, 5, 5);
	buttonLayout->setSpacing(0);
	
	QVBoxLayout *propertyLayout = new QVBoxLayout();
	propertyLayout->addWidget(_formWidget);
	propertyLayout->addLayout(buttonLayout);
	propertyLayout->setContentsMargins(5, 5, 5, 5);
	
	QVBoxLayout *mainLayout = new QVBoxLayout();
	mainLayout->addWidget(_treeView);
	mainLayout->addLayout(propertyLayout);
	mainLayout->setContentsMargins(0, 0, 0, 0);
	mainLayout->setSpacing(0);
	
	setLayout(mainLayout);
}

}
