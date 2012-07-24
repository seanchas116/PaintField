#include <QtGui>
#include "fssimplebutton.h"
#include "fscanvasview.h"
#include "fsguimain.h"

#include "fsmainpanel.h"

FSMainPanel::FSMainPanel(QWidget *parent) :
	QWidget(parent),
	_currentView(0)
{
	_instance = this;
	
	_menu = new QMenu(this);
	
	_menu->addAction(fsActionManager()->action("newFile"));
	_menu->addAction(fsActionManager()->action("openFile"));
	_menu->addAction(fsActionManager()->action("saveFile"));
	_menu->addSeparator();
	_menu->addAction(fsActionManager()->action("saveAsFile"));
	
	// temporary ui
	
	QVBoxLayout *buttonLayout = new QVBoxLayout();
	
	FSSimpleButton *newButton = new FSSimpleButton(":/icons/16x16/add.svg");
	
	FSSimpleButton *menuButton = new FSSimpleButton(":/icons/16x16/menuRight.svg");
	menuButton->setMenu(_menu);
	
	buttonLayout->addWidget(newButton);
	buttonLayout->addWidget(menuButton);
	buttonLayout->addStretch(1);
	
	QGridLayout *gridLayout = new QGridLayout();
	gridLayout->addWidget(new FSSimpleButton(":/icons/24x24/move.svg"), 0, 0);
	gridLayout->addWidget(new FSSimpleButton(":/icons/24x24/selectRect.svg"), 0, 1);
	gridLayout->addWidget(new FSSimpleButton(":/icons/24x24/selectEllipse.svg"), 0, 2);
	gridLayout->setRowStretch(1, 1);
	gridLayout->setColumnStretch(3, 1);
	
	
	QHBoxLayout *mainLayout = new QHBoxLayout();
	mainLayout->addLayout(gridLayout);
	mainLayout->addLayout(buttonLayout);
	
	setLayout(mainLayout);
	
	setWindowTitle("Azurite");
}

void FSMainPanel::closeEvent(QCloseEvent *)
{
	fsGuiMain()->quit();
}

FSMainPanel *FSMainPanel::_instance = 0;
