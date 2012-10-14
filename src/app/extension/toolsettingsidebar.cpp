#include "core/toolmanager.h"

#include "toolsettingsidebar.h"

namespace PaintField
{

ToolSettingSidebar::ToolSettingSidebar(ToolManager *toolManager, QWidget *parent) :
    QWidget(parent),
    _layout(new QVBoxLayout)
{
	setWindowTitle(tr("Tool Settings"));
	
	_layout->setContentsMargins(0, 0, 0, 0);
	setLayout(_layout);
	
	connect(toolManager, SIGNAL(currentToolFactoryChanged(ToolFactory*)), this, SLOT(onCurrentToolFactoryChanged(ToolFactory*)));
	onCurrentToolFactoryChanged(toolManager->currentToolFactory());
}


void ToolSettingSidebar::onCurrentToolFactoryChanged(ToolFactory *factory)
{
	if (_widget)
		_layout->removeWidget(_widget.data());
	
	_widget.reset(factory->createSettingWidget());
	
	if (_widget)
		_layout->addWidget(_widget.data());
}

}
