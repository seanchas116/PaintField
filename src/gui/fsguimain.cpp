#include <QtGui>

#include "fscore.h"

#include "fscanvasview.h"
#include "fslayertreepanel.h"
#include "fscolorpanel.h"
#include "fsaction.h"
#include "fsundoredoaction.h"

#include "fsguimain.h"


FSGuiMain::FSGuiMain(QObject *parent) :
	QObject(parent),
	_mainPanel(0),
	_currentView(0),
	_currentEditActionHandler(0)
{
	_instance = this;
	
	// creating actions
	
	_actionManager = new FSActionManager(this);
	
	_actionManager->addAction(new QAction(_actionManager), "newFile", tr("New File"), QKeySequence("Ctrl+Shift+N"));
	_actionManager->addAction(new QAction(_actionManager), "openFile", tr("Open File..."), QKeySequence("Ctrl+O"));
	_actionManager->addAction(new FSCanvasViewAction(_actionManager), "saveFile", tr("Save"), QKeySequence("Ctrl+S"));
	_actionManager->addAction(new FSCanvasViewAction(_actionManager), "saveAsFile", tr("Save As..."), QKeySequence("Ctrl+Shift+S"));
	_actionManager->addAction(new FSCanvasViewAction(_actionManager), "closeFile", tr("Close File"), QKeySequence("Ctrl+W"));
	_actionManager->addAction(new QAction(_actionManager), "quit", "Quit", QKeySequence("Ctrl+Q"));
	
	_actionManager->addAction(new FSUndoAction(_actionManager), "undo", tr("Undo"), QKeySequence("Ctrl+Z"));
	_actionManager->addAction(new FSRedoAction(_actionManager), "redo", tr("Redo"), QKeySequence("Ctrl+Shift+Z"));
	
	_actionManager->addAction(new FSEditAction(_actionManager), "cut", tr("Cut"), QKeySequence("Ctrl+X"));
	_actionManager->addAction(new FSEditAction(_actionManager), "copy", tr("Copy"), QKeySequence("Ctrl+C"));
	_actionManager->addAction(new FSEditAction(_actionManager), "paste", tr("Paste"), QKeySequence("Ctrl+C"));
	_actionManager->addAction(new FSEditAction(_actionManager), "delete", tr("Delete"));
	_actionManager->addAction(new FSEditAction(_actionManager), "selectAll", tr("Select All"), QKeySequence("Ctrl+A"));
	
	_actionManager->addAction(new FSCanvasViewAction(_actionManager), "newLayer", tr("New Layer"), QKeySequence("Ctrl+N"));
	_actionManager->addAction(new FSCanvasViewAction(_actionManager), "newGroup", tr("New Group"), QKeySequence("Ctrl+G"));
	_actionManager->addAction(new FSCanvasViewAction(_actionManager), "addLayerFromFile", tr("Add Layer From Image File..."));
	_actionManager->addAction(new FSLayerAction(_actionManager), "mergeLayer", tr("Merge Layers"));
	
	foreach (QAction *action, fsCore()->toolManager()->actionList())
	{
		_actionManager->addAction(action);
	}
	
	_actionManager->addAction(new QAction(_actionManager), "minimizeWindow", tr("Minimize"));
	_actionManager->addAction(new QAction(_actionManager), "zoomWindow", tr("Zoom"));
	
	_actionManager->connectTriggered("newFile", this, SLOT(newFile()));
	_actionManager->connectTriggered("openFile", this, SLOT(openFile()));
	_actionManager->connectTriggered("saveFile", this, SLOT(saveFile()));
	_actionManager->connectTriggered("saveAsFile", this, SLOT(saveAsFile()));
	_actionManager->connectTriggered("closeFile", this, SLOT(closeFile()));
	_actionManager->connectTriggered("quit", this, SLOT(quit()));
	
	_actionManager->connectTriggered("minimizeWindow", this, SLOT(minimizeCurrentWindow()));
	_actionManager->connectTriggered("zoomWindow", this, SLOT(zoomCurrentWindow()));
	
	// creating menus
	
	_mainMenu = new QMenuBar();
	
	QMenu *fileMenu = _mainMenu->addMenu(tr("File"));
	QMenu *editMenu = _mainMenu->addMenu(tr("Edit"));
	QMenu *layerMenu = _mainMenu->addMenu(tr("Layer"));
	QMenu *toolMenu = _mainMenu->addMenu(tr("Tool"));
	QMenu *windowMenu = _mainMenu->addMenu(tr("Window"));
	QMenu *helpMenu = _mainMenu->addMenu(tr("Help"));
	
	fileMenu->addAction(action("newFile"));
	fileMenu->addAction(action("openFile"));
	fileMenu->addSeparator();
	fileMenu->addAction(action("saveFile"));
	fileMenu->addAction(action("saveAsFile"));
	fileMenu->addSeparator();
	fileMenu->addAction(action("quit"));
	
	editMenu->addAction(action("undo"));
	editMenu->addAction(action("redo"));
	editMenu->addSeparator();
	editMenu->addAction(action("cut"));
	editMenu->addAction(action("copy"));
	editMenu->addAction(action("paste"));
	editMenu->addAction(action("delete"));
	editMenu->addSeparator();
	editMenu->addAction(action("selectAll"));
	
	layerMenu->addAction(action("newLayer"));
	layerMenu->addAction(action("newGroup"));
	layerMenu->addAction(action("addLayerFromFile"));
	layerMenu->addSeparator();
	layerMenu->addAction(action("mergeLayer"));
	
	foreach (const QString &toolName, fsCore()->toolManager()->toolNameList())
	{
		QAction *action = _actionManager->action(toolName);
		if (action)
			toolMenu->addAction(action);
	}
	
	windowMenu->addAction(action("minimizeWindow"));
	windowMenu->addAction(action("zoomWindow"));
	
	_mainMenu->addMenu(fileMenu);
	_mainMenu->addMenu(editMenu);
	_mainMenu->addMenu(layerMenu);
	_mainMenu->addMenu(toolMenu);
	_mainMenu->addMenu(windowMenu);
	_mainMenu->addMenu(helpMenu);
	
	// adding panels
	
	_mainPanel = new FSMainPanel;
	_mainPanel->show();
	
	addPanel(new FSLayerTreePanel());
	addPanel(new FSColorPanel());
}

void FSGuiMain::addPanel(QWidget *panel)
{
	panel->show();
	_panels << panel;
}

void FSGuiMain::addCanvasView(FSCanvasView *view)
{
	_views << view;
	connect(view, SIGNAL(windowFocusIn()), this, SLOT(viewActivated()));
	connect(view, SIGNAL(windowClosed()), this, SLOT(viewClosed()));
	view->show();
}

void FSGuiMain::setCurrentView(FSCanvasView *view)
{
	if (_currentView != view)
	{
		_currentView = view;
		emit currentViewChanged(view);
		emit currentDocumentChanged(view ? view->documentModel() : 0);
	}
}

bool FSGuiMain::quit()
{
	foreach (FSCanvasView *view, _views)
	{
		if (!view->closeFile())
			return false;
	}
	qApp->quit();
	return true;
}

void FSGuiMain::newFile()
{
	FSCanvasView *view = FSCanvasView::newFile();
	if (view)
		addCanvasView(view);
}

void FSGuiMain::openFile()
{
	FSCanvasView *view = FSCanvasView::openFile();
	if (view)
		addCanvasView(view);
}

void FSGuiMain::saveFile()
{
	FSCanvasView *view = currentView();
	if (view)
		view->saveFile();
}

void FSGuiMain::saveAsFile()
{
	FSCanvasView *view = currentView();
	if (view)
		view->saveAsFile();
}

void FSGuiMain::closeFile()
{
	FSCanvasView *view = currentView();
	if (view)
		view->closeFile();
}

void FSGuiMain::minimizeCurrentWindow()
{
	QWidget *window = QApplication::activeWindow();
	if (window)
		window->showMinimized();
}

void FSGuiMain::zoomCurrentWindow()
{
	QWidget *window = QApplication::activeWindow();
	if (window)
		window->showMaximized();
}

void FSGuiMain::viewActivated()
{
	FSCanvasView *view = qobject_cast<FSCanvasView *>(sender());
	if (!view && !_views.contains(view)) return;
	
	setCurrentView(view);
}

void FSGuiMain::viewClosed()
{
	FSCanvasView *view = qobject_cast<FSCanvasView *>(sender());
	Q_ASSERT(view);
	Q_ASSERT(_views.contains(view));
	
	_views.removeOne(view);
	
	if (_currentView == view)
	{
		setCurrentView(0);
	}
}



FSGuiMain *FSGuiMain::_instance;

