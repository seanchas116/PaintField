#pragma once

#include <QAction>
#include <QPointer>
#include "global.h"

namespace PaintField {

class ProxyAction : public QAction
{
	Q_OBJECT
public:
	explicit ProxyAction(QObject *parent = 0);
	
	void setBackendAction(QAction *action);
	QAction *backendAction() { return _backendAction; }
	
	void setCanBeEnabled(bool enabled);
	bool canBeEnabled() const { return _canBeEnabled; }
	
private slots:
	
	void onBackendActionChanged();
	
private:
	
	QPointer<QAction> _backendAction = 0;
	bool _canBeEnabled = true;
};

} // namespace PaintField

