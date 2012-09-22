#include "toolsettingpanel.h"
#include "core/application.h"
#include "modules/tool/toolmodule.h"

namespace PaintField
{

ToolSettingPanel::ToolSettingPanel(QWidget *parent) :
    QWidget(parent),
    _layout(new QVBoxLayout)
{
	setWindowTitle(tr("Tool Settings"));
	
	_layout->setContentsMargins(0, 0, 0, 0);
	setLayout(_layout);
	
	ToolManager *tmanager = ToolModule::toolManager();
	
	connect(tmanager, SIGNAL(currentToolFactoryChanged(ToolFactory*)), this, SLOT(onCurrentToolFactoryChanged(ToolFactory*)));
	onCurrentToolFactoryChanged(tmanager->currentToolFactory());
}


void ToolSettingPanel::onCurrentToolFactoryChanged(ToolFactory *factory)
{
	if (_widget)
		_layout->removeWidget(_widget.data());
	
	_widget.reset(factory->createSettingWidget());
	
	if (_widget)
		_layout->addWidget(_widget.data());
}

}
