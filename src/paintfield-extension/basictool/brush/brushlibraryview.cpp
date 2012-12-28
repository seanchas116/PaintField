#include <QtGui>
#include "paintfield-core/widgets/simplebutton.h"

#include "brushlibrarymodel.h"

#include "brushlibraryview.h"

namespace PaintField {

BrushLibraryView::BrushLibraryView(BrushLibraryModel *model, QItemSelectionModel *selectionModel, QWidget *parent) :
    QWidget(parent)
{
	auto layout = new QVBoxLayout;
	
	{
		auto modelView = new QTreeView;
		modelView->setHeaderHidden(true);
		modelView->setModel(model);
		modelView->setSelectionModel(selectionModel);
		modelView->setSelectionMode(QAbstractItemView::SingleSelection);
		
		auto builtinContentsIndex = model->findIndex(QModelIndex(), "Built-in");
		
		if (builtinContentsIndex.isValid())
		{
			modelView->expand(builtinContentsIndex);
			auto defaultPresetIndex = model->findIndex(builtinContentsIndex, "Pen");
			selectionModel->setCurrentIndex(defaultPresetIndex, QItemSelectionModel::SelectCurrent);
		}
		
		layout->addWidget(modelView);
	}
	
	{
		auto buttonLayout = new QHBoxLayout;
		
		/*
		// save button
		{
			auto button = new SimpleButton(":/icons/16x16/add.svg", QSize(16,16), this, SIGNAL(saveRequested()));
			buttonLayout->addWidget(button);
		}
		*/
		
		// reload button
		{
			auto button = new SimpleButton(":/icons/16x16/rotateRight.svg", QSize(16,16), this, SIGNAL(reloadRequested()));
			buttonLayout->addWidget(button);
		}
		
		buttonLayout->addStretch(1);
		
		layout->addLayout(buttonLayout);
	}
	
	setLayout(layout);
}

} // namespace PaintField
