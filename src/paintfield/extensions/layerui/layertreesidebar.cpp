#include <QTreeView>
#include <QComboBox>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QMenu>

#include <Malachite/Painter>
#include <Malachite/ImageIO>

#include "paintfield/core/layeritemmodel.h"
#include "paintfield/core/layerscene.h"
#include "paintfield/core/util.h"
#include "paintfield/core/widgets/doubleslider.h"
#include "paintfield/core/widgets/simplebutton.h"
#include "paintfield/core/appcontroller.h"
#include "paintfield/core/workspace.h"
#include "paintfield/core/document.h"

#include "layeruicontroller.h"
#include "layermodelviewdelegate.h"

#include "layertreesidebar.h"

namespace PaintField
{

struct LayerTreeSidebar::Data
{
	LayerUIController *uiController = 0;
	Document *document = 0;
	QTreeView *view = 0;
	
	DoubleSlider *opacitySlider = 0;
	LooseSpinBox *opacitySpinBox = 0;
	QComboBox *blendModeComboBox = 0;
	
	QWidget *formWidget = 0;
	
	LayerRef current;
};

LayerTreeSidebar::LayerTreeSidebar(LayerUIController *layerUIController, QWidget *parent) :
    QWidget(parent),
	d(new Data)
{
	d->uiController = layerUIController;
	d->document = layerUIController ? layerUIController->document() : 0;
	
	createForms();
	
	if (d->document)
	{
		auto document = d->document;
		
		d->view->setModel(document->layerScene()->itemModel());
		connect(document, SIGNAL(modified()), this, SLOT(updateView()));
		connect(document->layerScene(), SIGNAL(currentChanged(LayerRef,LayerRef)), this, SLOT(onCurrentChanged(LayerRef)));
		onCurrentChanged(document->layerScene()->current());
		d->view->setSelectionModel(document->layerScene()->itemSelectionModel());
		
		connect(document->layerScene(), SIGNAL(thumbnailsUpdated()), d->view, SLOT(update()));
	}
	else
	{
		setEnabled(false);
		onCurrentChanged(LayerRef());
	}
}

LayerTreeSidebar::~LayerTreeSidebar()
{
	delete d;
}

void LayerTreeSidebar::onCurrentChanged(const LayerRef &current)
{
	d->current = current;
	updateView();
}

void LayerTreeSidebar::updateView()
{
	d->opacitySpinBox->setEnabled(d->current);
	
	if (d->current)
	{
		d->opacitySpinBox->setValue(d->current.pointer()->property(RoleOpacity).toDouble() * 100.0);
	}
}

void LayerTreeSidebar::setOpacityPercentage(double value)
{
	d->document->layerScene()->setLayerProperty(d->current, value / 100.0, RoleOpacity, tr("Set Layer Opacity"));
}

void LayerTreeSidebar::createForms()
{
	QVBoxLayout *mainLayout = new QVBoxLayout();
	mainLayout->setContentsMargins(0, 0, 0, 0);
	mainLayout->setSpacing(0);
	
	{
		auto view = new QTreeView();
		
		if (d->uiController)
			view->setItemDelegate(new LayerModelViewDelegate(d->uiController, this));
		
		view->setHeaderHidden(true);
		view->setSelectionMode(QAbstractItemView::ExtendedSelection);
		view->setDragDropMode(QAbstractItemView::DragDrop);
		view->setDefaultDropAction(Qt::MoveAction);
		view->setDropIndicatorShown(true);
		d->view = view;
		
		mainLayout->addWidget(view);
	}
	
	{
		auto propertyLayout = new QVBoxLayout();
		propertyLayout->setContentsMargins(5, 5, 5, 5);
		
		// form
		{
			auto formLayout = new QFormLayout();
			formLayout->setHorizontalSpacing(5);
			formLayout->setVerticalSpacing(5);
			formLayout->setContentsMargins(5, 5, 5, 5);
			
			// opacity
			{
				auto layout = new QHBoxLayout();
				
				{
					auto slider = new DoubleSlider(Qt::Horizontal);
					slider->setMinimum(0);
					slider->setMaximum(1000);
					slider->setDoubleMinimum(0.0);
					slider->setDoubleMaximum(100.0);
					d->opacitySlider = slider;
					
					auto spin = new LooseSpinBox();
					spin->setDecimals(1);
					spin->setMinimum(0);
					spin->setMaximum(100);
					spin->setSingleStep(1.0);
					d->opacitySpinBox = spin;
					
					connect(slider, SIGNAL(doubleValueChanged(double)), spin, SLOT(setValue(double)));
					connect(spin, SIGNAL(valueChanged(double)), slider, SLOT(setDoubleValue(double)));
					connect(slider, SIGNAL(doubleValueChanged(double)), this, SLOT(setOpacityPercentage(double)));
					
					layout->addWidget(slider);
					layout->addWidget(spin);
					layout->addWidget(new QLabel("%"));
				}
				
				formLayout->addRow(tr("Opacity"), layout);
			}
			
			{
				auto combo = new QComboBox();
				combo->addItem(tr("Unimplemented"));
				d->blendModeComboBox = combo;
				
				formLayout->addRow(tr("Blend"), combo);
			}
			
			d->formWidget = new QWidget();
			d->formWidget->setLayout(formLayout);
			
			propertyLayout->addWidget(d->formWidget);
		}
		
		// buttons
		{
			auto layout = new QHBoxLayout();
			layout->setContentsMargins(5, 5, 5, 5);
			layout->setSpacing(0);
			
			auto addButton = new SimpleButton(":/icons/16x16/add.svg", QSize(16,16));
			addButton->setMargins(4, 0, 4, 0);
			auto removeButton = new SimpleButton(":/icons/16x16/subtract.svg", QSize(16,16));
			removeButton->setMargins(4, 0, 4, 0);
			auto miscButton = new SimpleButton(":/icons/16x16/menuDown.svg", QSize(16,16));
			miscButton->setMargins(4, 0, 4, 0);
			
			if (d->uiController)
			{
				QMenu *addMenu = new QMenu(this);
				
				addMenu->addAction(d->uiController->action(LayerUIController::ActionNewRaster));
				addMenu->addAction(d->uiController->action(LayerUIController::ActionNewGroup));
				addMenu->addAction(d->uiController->action(LayerUIController::ActionImport));
				
				addButton->setMenu(addMenu);
				
				connect(removeButton, SIGNAL(pressed()), d->uiController, SLOT(removeLayers()));
				
				QMenu *miscMenu = new QMenu(this);
				miscMenu->addAction(d->uiController->action(LayerUIController::ActionMerge));
				
				miscButton->setMenu(miscMenu);
			}
			
			layout->addWidget(addButton);
			layout->addWidget(removeButton);
			layout->addWidget(miscButton);
			layout->addStretch(1);
			
			propertyLayout->addLayout(layout);
		}
		
		mainLayout->addLayout(propertyLayout);
	}
	
	setLayout(mainLayout);
}

}
