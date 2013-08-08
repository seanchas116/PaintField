#include <QTreeView>
#include <QComboBox>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QMenu>
#include <QKeyEvent>

#include <Malachite/Painter>
#include <Malachite/ImageIO>

#include "paintfield/core/layeritemmodel.h"
#include "paintfield/core/layerscene.h"
#include "paintfield/core/util.h"
#include "paintfield/core/widgets/looseslider.h"
#include "paintfield/core/widgets/simplebutton.h"
#include "paintfield/core/appcontroller.h"
#include "paintfield/core/workspace.h"
#include "paintfield/core/document.h"

#include "layerpropertyeditor.h"
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
};

LayerTreeSidebar::LayerTreeSidebar(LayerUIController *layerUIController, QWidget *parent) :
    QWidget(parent),
	d(new Data)
{
	d->uiController = layerUIController;
	d->document = layerUIController ? layerUIController->document() : 0;
	
	createForms();
	
	if (d->uiController)
	{
		for (QAction *action : d->uiController->actions())
		{
			action->setShortcutContext(Qt::WidgetWithChildrenShortcut);
			addAction(action);
		}
	}
	
	setEnabled(d->document);
}

LayerTreeSidebar::~LayerTreeSidebar()
{
	delete d;
}

void LayerTreeSidebar::showViewContextMenu(const QPoint &pos)
{
	PAINTFIELD_DEBUG << "showing context menu at" << pos;
	
	QMenu menu;
	
	menu.addAction(d->uiController->action(LayerUIController::ActionCut));
	menu.addAction(d->uiController->action(LayerUIController::ActionCopy));
	menu.addAction(d->uiController->action(LayerUIController::ActionPaste));
	menu.addAction(d->uiController->action(LayerUIController::ActionRemove));
	menu.addSeparator();
	menu.addAction(d->uiController->action(LayerUIController::ActionNewRaster));
	menu.addAction(d->uiController->action(LayerUIController::ActionNewGroup));
	menu.addAction(d->uiController->action(LayerUIController::ActionImport));
	menu.addSeparator();
	menu.addAction(d->uiController->action(LayerUIController::ActionMerge));
	menu.addAction(d->uiController->action(LayerUIController::ActionRasterize));
	
	menu.exec(d->view->mapToGlobal(pos));
}

bool LayerTreeSidebar::eventFilter(QObject *object, QEvent *event)
{
	if (object == d->view)
	{
		switch (event->type())
		{
			case QEvent::KeyPress:
			{
				auto keyEvent = static_cast<QKeyEvent *>(event);
				if (keyEvent->key() == Qt::Key_Backspace)
				{
					d->uiController->action(LayerUIController::ActionRemove)->trigger();
				}
				break;
			}
			default:
				break;
		}
	}
	return false;
}

void LayerTreeSidebar::createForms()
{
	QVBoxLayout *mainLayout = new QVBoxLayout();
	mainLayout->setSpacing(0);
	mainLayout->setContentsMargins(0, 0, 0, 0);
	
	{
		auto view = new QTreeView();
		
		if (d->uiController)
			view->setItemDelegate(new LayerModelViewDelegate(d->uiController, this));
		
		view->setHeaderHidden(true);
		view->setSelectionMode(QAbstractItemView::ExtendedSelection);
		view->setDragDropMode(QAbstractItemView::DragDrop);
		view->setDefaultDropAction(Qt::MoveAction);
		view->setDropIndicatorShown(true);
		
		if (d->document)
		{
			view->setModel(d->document->layerScene()->itemModel());
			view->setSelectionModel(d->document->layerScene()->itemSelectionModel());
			connect(d->document->layerScene(), SIGNAL(thumbnailsUpdated()), view, SLOT(update()));
		}
		
		view->setContextMenuPolicy(Qt::CustomContextMenu);
		connect(view, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showViewContextMenu(QPoint)));
		view->installEventFilter(this);
		
		d->view = view;
		
		mainLayout->addWidget(view);
	}
	
	{
		auto lowerLayout = new QVBoxLayout();
		lowerLayout->setSpacing(6);
		lowerLayout->setContentsMargins(6,6,6,6);
		
		{
			auto editor = new LayerPropertyEditor(d->document ? d->document->layerScene() : 0);
			lowerLayout->addWidget(editor);
		}
		
		// buttons
		{
			auto layout = new QHBoxLayout();
			layout->setSpacing(0);
			
			auto addButton = new SimpleButton(":/icons/16x16/add.svg", QSize(16,16));
			addButton->setMargins(4, 0, 4, 0);
			auto removeButton = new SimpleButton(":/icons/16x16/subtract.svg", QSize(16,16));
			removeButton->setMargins(4, 0, 4, 0);
			auto miscButton = new SimpleButton(":/icons/16x16/menuDown.svg", QSize(16,16));
			miscButton->setMargins(4, 0, 4, 0);
			
			if (d->uiController)
			{
				auto addMenu = new QMenu(this);
				
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
			
			lowerLayout->addLayout(layout);
		}
		
		mainLayout->addLayout(lowerLayout);
	}
	
	setLayout(mainLayout);
}

}
