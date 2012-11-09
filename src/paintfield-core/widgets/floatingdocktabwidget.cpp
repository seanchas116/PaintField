#include <QEvent>
#include <QApplication>

#include "floatingdocktabwidget.h"

namespace PaintField
{

FloatingDockTabWidget::FloatingDockTabWidget(QWidget *baseWindow, QWidget *parent) :
	DockTabWidget(parent),
	_baseWindow(baseWindow)
{
	commonInit();
}

FloatingDockTabWidget::FloatingDockTabWidget(FloatingDockTabWidget *other, QWidget *parent) :
	DockTabWidget(other, parent),
	_baseWindow(other->_baseWindow)
{
	commonInit();
}

void FloatingDockTabWidget::commonInit()
{
	setAutoDeletionEnabled(true);
	
	if (parent() == 0)
	{
		setParent(_baseWindow);
		setWindowFlags(Qt::Tool);
		//connect(qApp, SIGNAL(focusChanged(QWidget*,QWidget*)), this, SLOT(onFocusChanged(QWidget*,QWidget*)));
		//onFocusChanged(0, qApp->focusWidget());
	}
}

void FloatingDockTabWidget::onFocusChanged(QWidget *old, QWidget *now)
{
	Q_UNUSED(old);
	
	if (parent() == _baseWindow && now != 0 && isVisible())
		setVisible(_baseWindow == now && this != now);
}

}
