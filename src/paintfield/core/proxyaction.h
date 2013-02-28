#pragma once

#include <QAction>
#include <QPointer>

namespace PaintField {

class ProxyAction : public QAction
{
	Q_OBJECT
public:
	explicit ProxyAction(QObject *parent = 0);
	
	void setBackendAction(QAction *action);
	QAction *backendAction() { return _backendAction; }
	
private slots:
	
	void onBackendActionChanged();
	
private:
	
	QPointer<QAction> _backendAction = 0;
};

} // namespace PaintField

