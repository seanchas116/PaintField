#ifndef WORKSPACEVIEW_H
#define WORKSPACEVIEW_H

#include <QMainWindow>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QAction>
#include <QPointer>
#include <QVBoxLayout>

#include "global.h"
#include "canvasview.h"
#include "settingsmanager.h"
#include "workspacetabwidget.h"

#include "widgets/docktabwidget.h"
#include "widgets/docktabmotherwidget.h"

namespace PaintField
{

class SidebarTabWidget : public WorkspaceTabWidget
{
	Q_OBJECT
	Q_INTERFACES(PaintField::ReproductiveInterface)
	
public:
	
	SidebarTabWidget(WorkspaceController *workspace, QWidget *parent);
	
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
	
	explicit WorkspaceMotherWidget(WorkspaceController *workspace, QWidget *parent = 0) : DockTabMotherWidget(parent), _workspace(workspace) {}
	
	bool tabIsInsertable(DockTabWidget *src, int srcIndex) override;
	
private:
	
	WorkspaceController *_workspace;
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

class WorkspaceController;

class WorkspaceView : public QMainWindow
{
	Q_OBJECT
public:
	explicit WorkspaceView(WorkspaceController *controller, QWidget *parent = 0);
	
	void createSideBarFrames(const SideBarDeclarationHash &sidebarDeclarations, const QVariant &order);
	void createToolBars(const ToolBarDeclarationHash &toolBarDeclarations, const QVariant &order);
	void createMenuBar(const ActionDeclarationHash &actionDeclarations, const MenuDeclarationHash &menuDeclarations,  const QVariant &order);
	
	void setCentralWidget(QWidget *widget);
	
	void setSidebar(const QString &id, QWidget *sidebar);
	QToolBar *toolBar(const QString &id);
	void associateMenuBarWithActions(const QActionList &actions);
	
	WorkspaceController *controller();
	
signals:
	
	void focusChanged(bool x);
	void closeRequested();
	
public slots:
	
	void setCurrentCanvas(CanvasController *canvas);
	
protected:
	
	void closeEvent(QCloseEvent *event);
	
	void focusInEvent(QFocusEvent *);
	void focusOutEvent(QFocusEvent *);
	
private slots:
	
	void onCurrentCanvasPropertyChanged();
	
private:
	
	void createSideBarFramesInArea(DockTabMotherWidget::Direction splitterDir, const SideBarDeclarationHash &sidebarDeclarations, const QVariant &areaOrder);
	void createSideBarFramesInSplitter(DockTabMotherWidget::Direction splitterDir, int splitterIndex, const SideBarDeclarationHash &sidebarDeclarations, const QVariant &splitterOrder);
	void createToolBarsInArea(Qt::ToolBarArea area, const ToolBarDeclarationHash &toolBarDeclarations, const QVariant &areaOrder);
	
	class Data;
	Data *d;
};

}

#endif // WORKSPACEVIEW_H
