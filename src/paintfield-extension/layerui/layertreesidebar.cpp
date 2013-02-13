#include <QtGui>
#include <Malachite/Painter>
#include <Malachite/ImageIO>

#include "paintfield-core/util.h"
#include "paintfield-core/widgets/doubleslider.h"
#include "paintfield-core/widgets/simplebutton.h"
#include "paintfield-core/appcontroller.h"
#include "paintfield-core/workspace.h"
#include "paintfield-core/document.h"

#include "layeruicontroller.h"
#include "layermodelviewdelegate.h"

#include "layertreesidebar.h"

namespace PaintField
{

LayerTreeSidebar::LayerTreeSidebar(LayerUIController *layerUIController, QWidget *parent) :
    QWidget(parent),
    _layerUIController(layerUIController),
    _canvas(layerUIController ? layerUIController->canvas() : 0)
{
	createForms();
	
	if (_canvas)
	{
		_treeView->setModel(_canvas->layerModel());
		connect(_canvas->document(), SIGNAL(modified()), this, SLOT(updatePropertyView()));
		connect(_canvas->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(updatePropertyView()));
		updatePropertyView();
		_treeView->setSelectionModel(_canvas->selectionModel());
		
		connect(_canvas->document()->layerModel(), SIGNAL(thumbnailsUpdated()), _treeView, SLOT(update()));
	}
	else
	{
		setEnabled(false);
	}
	updatePropertyView();
}

void LayerTreeSidebar::updatePropertyView()
{
	if (_canvas && _canvas->selectionModel()->currentIndex().isValid())
	{
		_formWidget->setEnabled(true);
		_opacitySpinBox->setValue(_canvas->selectionModel()->currentIndex().data(PaintField::RoleOpacity).toDouble() * 100.0);
	}
	else
		_formWidget->setEnabled(false);
}

void LayerTreeSidebar::setOpacityPercentage(double value)
{
	_canvas->layerModel()->setData(_canvas->selectionModel()->currentIndex(), value / 100.0, PaintField::RoleOpacity);
}

void LayerTreeSidebar::createForms()
{
	_treeView = new QTreeView();
	_treeView->setItemDelegate(new LayerModelViewDelegate(_layerUIController, this));
	_treeView->setHeaderHidden(true);
	_treeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
	_treeView->setDragDropMode(QAbstractItemView::DragDrop);
	_treeView->setDefaultDropAction(Qt::MoveAction);
	_treeView->setDropIndicatorShown(true);
	
	_opacitySlider = new DoubleSlider(Qt::Horizontal);
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
	_blendModeComboBox->addItem("Unimplemented");
	
	QFormLayout *formLayout = new QFormLayout();
	formLayout->addRow(tr("Opacity"), opacityLayout);
	formLayout->addRow(tr("Blend"), _blendModeComboBox);
	formLayout->setHorizontalSpacing(5);
	formLayout->setVerticalSpacing(5);
	formLayout->setContentsMargins(5, 5, 5, 5);
	
	_formWidget = new QWidget();
	_formWidget->setLayout(formLayout);
	
	auto addButton = new SimpleButton(":/icons/16x16/add.svg", QSize(16,16));
	addButton->setMargins(4, 0, 4, 0);
	auto removeButton = new SimpleButton(":/icons/16x16/subtract.svg", QSize(16,16));
	removeButton->setMargins(4, 0, 4, 0);
	auto miscButton = new SimpleButton(":/icons/16x16/menuDown.svg", QSize(16,16));
	miscButton->setMargins(4, 0, 4, 0);
	
	if (_layerUIController)
	{
		QMenu *addMenu = new QMenu(this);
		
		addMenu->addAction(_layerUIController->newRasterAction());
		addMenu->addAction(_layerUIController->newGroupAction());
		addMenu->addAction(_layerUIController->importAction());
		
		addButton->setMenu(addMenu);
		
		connect(removeButton, SIGNAL(pressed()), _layerUIController, SLOT(removeLayers()));
		
		QMenu *miscMenu = new QMenu(this);
		miscMenu->addAction(_layerUIController->mergeAction());
		
		miscButton->setMenu(miscMenu);
	}
	
	QHBoxLayout *buttonLayout = new QHBoxLayout();
	buttonLayout->addWidget(addButton);
	buttonLayout->addWidget(removeButton);
	buttonLayout->addWidget(miscButton);
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
