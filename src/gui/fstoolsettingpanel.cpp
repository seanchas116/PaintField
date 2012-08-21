#include "fstoolsettingpanel.h"
#include "fscore.h"

FSToolSettingPanel::FSToolSettingPanel(QWidget *parent) :
    QWidget(parent),
    _layout(new QVBoxLayout)
{
	setWindowTitle(tr("Tool Settings"));
	_layout->setContentsMargins(0, 0, 0, 0);
	setLayout(_layout);
	connect(fsToolManager(), SIGNAL(currentToolFactoryChanged(FSToolFactory*)), this, SLOT(onCurrentToolFactoryChanged(FSToolFactory*)));
	onCurrentToolFactoryChanged(fsToolManager()->currentToolFactory());
}

void FSToolSettingPanel::onCurrentToolFactoryChanged(FSToolFactory *factory)
{
	if (_widget)
		_layout->removeWidget(_widget.data());
	
	_widget.reset(factory->createSettingWidget());
	
	if (_widget)
		_layout->addWidget(_widget.data());
}
