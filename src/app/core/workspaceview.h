#ifndef WORKSPACEVIEW_H
#define WORKSPACEVIEW_H

#include <QMainWindow>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QAction>
#include <QPointer>

#include "sidebarfactory.h"
#include "canvasview.h"

namespace PaintField
{

class WorkspaceMenuAction : public QAction
{
	Q_OBJECT
public:
	
	WorkspaceMenuAction(QObject *parent) : QAction(parent) {}
	
	void setBackendAction(QAction *action);
	QAction *backendAction() { return _backendAction; }
	
private slots:
	
	void onBackendActionChanged();
	
private:
	
	QPointer<QAction> _backendAction = 0;
};

class WorkspaceView : public QMainWindow
{
	Q_OBJECT
public:
	explicit WorkspaceView(QWidget *parent = 0);
	
	void addSidebarFrame(const QString &id, const QString &text, Qt::DockWidgetArea area);
	void setSidebar(const QString &id, QWidget *sidebar);
	
signals:
	
public slots:
	
private:
	
	QList<QDockWidget *> _dockWidgets;
};

}

#endif // WORKSPACEVIEW_H
