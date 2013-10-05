#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTreeView>

#include "paintfield/core/util.h"
#include "paintfield/core/widgets/simplebutton.h"

#include "brushlibrarymodel.h"

#include "brushlibraryview.h"

namespace PaintField {

BrushLibraryView::BrushLibraryView(BrushLibraryModel *model, QWidget *parent) :
    QWidget(parent)
{
	auto layout = new QVBoxLayout;
	layout->setContentsMargins(0,0,0,0);
	layout->setSpacing(0);
	
	{
		auto treeView = new QTreeView;
		treeView->setHeaderHidden(true);
		treeView->setModel(model);
		treeView->setSelectionModel(model->selectionModel());
		treeView->setSelectionMode(QAbstractItemView::SingleSelection);
		
		Util::setExpandTreeViewRecursive(treeView, QModelIndex(), true);
		
		layout->addWidget(treeView);
	}
	
	{
		auto buttonLayout = new QHBoxLayout;
		buttonLayout->setContentsMargins(6,6,6,6);
		
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

	connect(this, SIGNAL(reloadRequested()), model, SLOT(reload()));
}

} // namespace PaintField
