#ifndef WORKSPACEVIEW_H
#define WORKSPACEVIEW_H

#include <QMainWindow>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QAction>
#include <QVBoxLayout>
#include <QPointer>

#include "global.h"
#include "settingsmanager.h"
#include "workspacetabwidget.h"

#include "widgets/docktabwidget.h"
#include "widgets/docktabmotherwidget.h"

namespace PaintField
{

class Canvas;

class SidebarTabWidget : public WorkspaceTabWidget
{
	Q_OBJECT
	Q_INTERFACES(PaintField::ReproductiveInterface)
	
public:
	
	SidebarTabWidget(Workspace *workspace, QWidget *parent);
	
	bool tabIsInsertable(DockTabWidget *other, int index) override;
	QObject *createNew() override;
	
private:
	
	void commonInit();
};

class WorkspaceMotherWidget : public DockTabMotherWidget
{
	Q_OBJECT
	Q_INTERFACES(PaintField::DockTabDroppableInterface)
	
public:
	
	explicit WorkspaceMotherWidget(Workspace *workspace, QWidget *parent = 0) : DockTabMotherWidget(parent), _workspace(workspace) {}
	
	bool tabIsInsertable(DockTabWidget *src, int srcIndex) override;
	
private:
	
	Workspace *_workspace;
};

class WorkspaceMenuAction : public QAction
{
	Q_OBJECT
public:
	
	WorkspaceMenuAction(QObject *parent);
	
	void setBackendAction(QAction *action);
	QAction *backendAction() { return _backendAction; }
	
private slots:
	
	void onBackendActionChanged();
	
private:
	
	QPointer<QAction> _backendAction = 0;
};

class DockTabMotherWidget;

class SideBarFrame : public QWidget
{
	Q_OBJECT
	
public:
	
	SideBarFrame(QWidget *parent = 0);
	
	void setSideBar(QWidget *sideBar);
	
private:
	
	QVBoxLayout *_layout = 0;
	QWidget *_sideBar = 0;
};

class Workspace;

class WorkspaceView : public QMainWindow
{
	Q_OBJECT
public:
	explicit WorkspaceView(Workspace *workspace, QWidget *parent = 0);
	
	void createSideBarFrames(const QHash<QString, SideBarInfo> &sideBarInfos, const QVariant &order);
	void createToolBars(const QHash<QString, ToolBarInfo> &toolBarInfos, const QVariant &order);
	void createMenuBar(const QHash<QString, ActionInfo> &actionInfos, const QHash<QString, MenuInfo> &menuInfos,  const QVariant &order);
	
	void setSidebar(const QString &id, QWidget *sidebar);
	QToolBar *toolBar(const QString &id);
	void associateMenuBarWithActions(const QList<QAction *> &actions);
	
	Workspace *workspace();
	
signals:
	
	void closeRequested();
	
public slots:
	
	void setCurrentCanvas(Canvas *canvas);
	
protected:
	
	void closeEvent(QCloseEvent *event);
	
private slots:
	
	void onCurrentCanvasDocumentPropertyChanged();
	
private:
	
	void createSideBarFramesInArea(DockTabMotherWidget::Direction splitterDir, const QHash<QString, SideBarInfo> &sidebarDeclarations, const QVariant &areaOrder);
	void createSideBarFramesInSplitter(DockTabMotherWidget::Direction splitterDir, int splitterIndex, const QHash<QString, SideBarInfo> &sidebarDeclarations, const QVariant &splitterOrder);
	void createToolBarsInArea(Qt::ToolBarArea area, const QHash<QString, ToolBarInfo> &toolBarDeclarations, const QVariant &areaOrder);
	
	void updateWorkspaceItems();
	void updateWorkspaceItemsForCanvas(Canvas *canvas);
	void updateMenuBar();
	
	class Data;
	Data *d;
};

}

#endif // WORKSPACEVIEW_H
