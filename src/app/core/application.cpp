#include <QtGui>

#include "util.h"
#include "application.h"

namespace PaintField
{

Application::Application(int &argv, char **args) :
    TabletApplication(argv, args)
{
	QTextCodec::setCodecForCStrings(QTextCodec::codecForLocale());
	
	declareMenuItemTitle("paintfield.file", tr("File"));
	
	declareMenuItemTitle("paintfield.file.new", tr("New..."));
	declareMenuItemTitle("paintfield.file.open", tr("Open..."));
	declareMenuItemTitle("paintfield.file.close", tr("Close"));
	declareMenuItemTitle("paintfield.file.save", tr("Save"));
	declareMenuItemTitle("paintfield.file.saveAs", tr("Save As..."));
	declareMenuItemTitle("paintfield.file.export", tr("Export..."));
	declareMenuItemTitle("paintfield.file.quit", tr("Quit"));
	
	declareMenuItemTitle("paintfield.edit", tr("Edit"));
	
	declareMenuItemTitle("paintfield.edit.undo", tr("Undo"));
	declareMenuItemTitle("paintfield.edit.redo", tr("Redo"));
	declareMenuItemTitle("paintfield.edit.cut", tr("Cut"));
	declareMenuItemTitle("paintfield.edit.copy", tr("Copy"));
	declareMenuItemTitle("paintfield.edit.paste", tr("Paste"));
	declareMenuItemTitle("paintfield.edit.delete", tr("Delete"));
	declareMenuItemTitle("paintfield.edit.selectAll", tr("Select All"));
	
	declareMenuItemTitle("paintfield.layer", tr("Layer"));
	
	declareMenuItemTitle("paintfield.layer.newRaster", tr("New Layer"));
	declareMenuItemTitle("paintfield.layer.newGroup", tr("New Group"));
	declareMenuItemTitle("paintfield.layer.import", tr("Import..."));
	declareMenuItemTitle("paintfield.layer.merge", tr("Merge"));
	
	declareMenuItemTitle("paintfield.window", tr("Window"));
	
	declareMenuItemTitle("paintfield.window.minimize", tr("Minimize"));
	declareMenuItemTitle("paintfield.window.zoom", tr("Zoom"));
	
	declareMenuItemTitle("paintfield.help", tr("Help"));
	
	_workspaceManager = new WorkspaceManager(this);
	_actionManager = new ActionManager(this);
	
	_actionManager->addAction("paintfield.file.quit", this, SLOT(tryQuit()));
	_actionManager->addAction("paintfield.window.minimize", this, SLOT(minimizeCurrentWindow()));
	_actionManager->addAction("paintfield.window.zoom", this, SLOT(zoomCurrentWindow()));
}

void Application::addToolFactory(ToolFactory *factory)
{
	if (factory)
	{
		factory->setParent(this);
		_toolFactories << factory;
	}
}

void Application::addSidebarFactory(SidebarFactory *factory)
{
	if (factory)
	{
		factory->setParent(this);
		_sidebarFactories << factory;
	}
}

void Application::tryQuit()
{
	if (_workspaceManager->tryCloseAll())
		quit();
}

void Application::minimizeCurrentWindow()
{
	QWidget *widget = activeWindow();
	if (widget)
		widget->showMinimized();
}

void Application::zoomCurrentWindow()
{
	QWidget *widget = activeWindow();
	if (widget)
		widget->showMaximized();
}

void Application::loadKeyMapFromJson(const QString &path)
{
	QVariantMap map = loadJsonFromFile(path).toMap();
	
	for (auto iter = map.begin(); iter != map.end(); ++iter)
	{
		QString id = iter.key();
		QKeySequence key(iter.value().toString());
		if (!id.isEmpty() && !key.isEmpty())
		{
			declareActionKeyMap(id, key);
		}
	}
}

}
