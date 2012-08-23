
#include <QHBoxLayout>
#include "fscore.h"
#include "fssimplebutton.h"

#include "fstoolpanel.h"

FSToolPanel::FSToolPanel(QWidget *parent) :
    FSPanelWidget(parent)
{
	resize(QSize(1, 1));
	
	setWindowTitle(tr("Tools"));
	
	//setStyleSheet("QWidget { background-color: grey }");
	
	//setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
	
	//setMinimumWidth(48);
	//setMaximumWidth(48);
	
	QList<QAction *> actions = fsToolManager()->actionList();
	
	QVBoxLayout *vlayout = new QVBoxLayout;
	vlayout->setContentsMargins(0, 0, 0, 0);
	vlayout->setSpacing(0);
	
	foreach (QAction *action, actions)
	{
		QToolButton *button = new QToolButton;
		button->setStyleSheet("QToolButton { background-color: transparent; }");
		button->setIconSize(QSize(48, 48));
		button->setDefaultAction(action);
		vlayout->addWidget(button);
	}
	
	QHBoxLayout *hlayout = new QHBoxLayout;
	
	QToolButton *menuButton = new QToolButton;
	menuButton->setIcon(fsCreateSimpleIconSet(":/icons/16x16/menuRight.svg"));
	menuButton->setStyleSheet("QToolButton { background-color: transparent; }");
	menuButton->setIconSize(QSize(16, 16));
	
	//hlayout->addStretch(1);
	hlayout->setAlignment(Qt::AlignRight);
	hlayout->addWidget(menuButton);
	hlayout->setContentsMargins(0, 0, 0, 0);
	hlayout->setSpacing(0);
	
	vlayout->addStretch(1);
	vlayout->addLayout(hlayout);
	
	setLayout(vlayout);}
