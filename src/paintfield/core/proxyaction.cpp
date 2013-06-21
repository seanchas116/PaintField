#include "proxyaction.h"

namespace PaintField {

ProxyAction::ProxyAction(QObject *parent) :
	QAction(parent)
{
	setEnabled(false);
}

void ProxyAction::setBackendAction(QAction *action)
{
	if (action == _backendAction)
		return;
	
	if (_backendAction)
	{
		disconnect(_backendAction, 0, this, 0);
		disconnect(this, 0, _backendAction, 0);
	}
	
	_backendAction = action;
	setEnabled(action);
	
	if (action)
	{
		connect(action, SIGNAL(changed()), this, SLOT(onBackendActionChanged()));
		
		if (action->isCheckable())
		{
			setCheckable(true);
			connect(action, SIGNAL(triggered(bool)), this, SLOT(setChecked(bool)));
			connect(this, SIGNAL(triggered(bool)), action, SLOT(setChecked(bool)));
		}
		else
		{
			connect(this, SIGNAL(triggered()), action, SLOT(trigger()));
		}
		onBackendActionChanged();
	}
}

void ProxyAction::onBackendActionChanged()
{
	Q_ASSERT(_backendAction);
	setEnabled(_backendAction->isEnabled());
	
	if (!_backendAction->text().isEmpty())
		setText(_backendAction->text());
}

} // namespace PaintField
