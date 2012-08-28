#include <QtGui>

#include "fscore.h"

#include "fslayertreepanel.h"
#include "fscolorpanel.h"
#include "fstoolpanel.h"
#include "fstoolsettingpanel.h"
#include "fsaction.h"
#include "fsundoredoaction.h"
#include "src/tool/fsbrushsettingwidget.h"

#include "fsguimain.h"


FSGuiMain::FSGuiMain(QObject *parent) :
	QObject(parent),
	_currentCanvas(0)
{
	_instance = this;
	
	// creating actions
	
	_actionManager = new FSActionManager(this);
	
	_actionManager->addAction(new QAction(_actionManager), "newFile", tr("New File"), QKeySequence("Ctrl+Shift+N"));
	_actionManager->addAction(new QAction(_actionManager), "openFile", tr("Open File..."), QKeySequence("Ctrl+O"));
	_actionManager->addAction(new FSCanvasAction(_actionManager), "saveFile", tr("Save"), QKeySequence("Ctrl+S"));
	_actionManager->addAction(new FSCanvasAction(_actionManager), "saveAsFile", tr("Save As..."), QKeySequence("Ctrl+Shift+S"));
	_actionManager->addAction(new FSCanvasAction(_actionManager), "closeFile", tr("Close File"), QKeySequence("Ctrl+W"));
	_actionManager->addAction(new FSCanvasAction(_actionManager), "exportFile", tr("Export..."));
	_actionManager->addAction(new QAction(_actionManager), "quit", "Quit", QKeySequence("Ctrl+Q"));
	
	_actionManager->addAction(new FSUndoAction(_actionManager), "undo", tr("Undo"), QKeySequence("Ctrl+Z"));
	_actionManager->addAction(new FSRedoAction(_actionManager), "redo", tr("Redo"), QKeySequence("Ctrl+Shift+Z"));
	
	_actionManager->addAction(new FSEditAction(_actionManager), "cut", tr("Cut"), QKeySequence("Ctrl+X"));
	_actionManager->addAction(new FSEditAction(_actionManager), "copy", tr("Copy"), QKeySequence("Ctrl+C"));
	_actionManager->addAction(new FSEditAction(_actionManager), "paste", tr("Paste"), QKeySequence("Ctrl+C"));
	_actionManager->addAction(new FSEditAction(_actionManager), "delete", tr("Delete"));
	_actionManager->addAction(new FSEditAction(_actionManager), "selectAll", tr("Select All"), QKeySequence("Ctrl+A"));
	
	_actionManager->addAction(new FSCanvasAction(_actionManager), "newLayer", tr("New Layer"), QKeySequence("Ctrl+N"));
	_actionManager->addAction(new FSCanvasAction(_actionManager), "newGroup", tr("New Group"), QKeySequence("Ctrl+G"));
	_actionManager->addAction(new FSCanvasAction(_actionManager), "addLayerFromFile", tr("Add Layer From Image File..."));
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
	_actionManager->connectTriggered("exportFile", this, SLOT(exportFile()));
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
	fileMenu->addAction(action("closeFile"));
	fileMenu->addAction(action("saveFile"));
	fileMenu->addAction(action("saveAsFile"));
	fileMenu->addSeparator();
	fileMenu->addAction(action("exportFile"));
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
	
	//_mainPanel = new FSMainPanel;
	//_mainPanel->show();
	
	addPanel(new FSLayerTreePanel());
	addPanel(new FSColorPanel());
	addPanel(new FSToolPanel());
	//addPanel(new FSToolSettingPanel());
	addPanel(new FSBrushSettingWidget());
}

void FSGuiMain::addPanel(FSPanelWidget *widget)
{
	FSPanel *panel = new FSPanel;
	panel->setWidget(widget);
	panel->show();
	_panels << panel;
}

void FSGuiMain::addCanvas(FSCanvas *canvas)
{
	_canvases << canvas;
	connect(canvas, SIGNAL(windowFocusIn()), this, SLOT(onCanvasActivated()));
	connect(canvas, SIGNAL(windowClosed()), this, SLOT(onCanvasClosed()));
	canvas->show();
}

void FSGuiMain::setCurrentCanvas(FSCanvas *canvas)
{
	if (_currentCanvas != canvas)
	{
		_currentCanvas = canvas;
		emit currentCanvasChanged(canvas);
		emit currentDocumentChanged(canvas ? canvas->document() : 0);
	}
}

bool FSGuiMain::quit()
{
	foreach (FSCanvas *canvas, _canvases)
	{
		if (!canvas->closeFile())
			return false;
	}
	qApp->quit();
	return true;
}

void FSGuiMain::newFile()
{
	FSCanvas *canvas = FSCanvas::newFile();
	if (canvas)
		addCanvas(canvas);
}

void FSGuiMain::openFile()
{
	FSCanvas *canvas = FSCanvas::openFile();
	if (canvas)
		addCanvas(canvas);
}

void FSGuiMain::saveFile()
{
	FSCanvas *canvas = currentCanvas();
	if (canvas)
		canvas->saveFile();
}

void FSGuiMain::saveAsFile()
{
	FSCanvas *canvas = currentCanvas();
	if (canvas)
		canvas->saveAsFile();
}

void FSGuiMain::closeFile()
{
	FSCanvas *canvas = currentCanvas();
	if (canvas)
		canvas->closeFile();
	
	_canvases.removeOne(canvas);
}

void FSGuiMain::exportFile()
{
	FSCanvas *canvas = currentCanvas();
	if (canvas)
		canvas->exportFile();
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

void FSGuiMain::onCanvasActivated()
{
	FSCanvas *canvas = qobject_cast<FSCanvas *>(sender());
	if (!canvas && !_canvases.contains(canvas)) return;
	
	setCurrentCanvas(canvas);
}

void FSGuiMain::onCanvasClosed()
{
	FSCanvas *canvas = qobject_cast<FSCanvas *>(sender());
	Q_ASSERT(canvas);
	Q_ASSERT(_canvases.contains(canvas));
	
	_canvases.removeOne(canvas);
	
	if (_currentCanvas == canvas)
	{
		setCurrentCanvas(0);
	}
}



FSGuiMain *FSGuiMain::_instance;

