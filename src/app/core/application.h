#ifndef FSAPPLICATION_H
#define FSAPPLICATION_H

#include <QMainWindow>
#include "tabletapplication.h"

#include "toolmanager.h"
#include "palettemanager.h"
#include "workspacemanager.h"

namespace PaintField {

class Application : public TabletApplication
{
	Q_OBJECT
public:
	
	Application(int &argv, char **args);
	
	/**
	 *
	 * @return The workspace manager
	 */
	WorkspaceManager *workspaceManager() { return _workspaceManager; }
	
	QList<ToolFactory *> toolFactories() { return _toolFactories; }
	
	void addToolFactory(ToolFactory *factory);
	
	void declareMenuTitle(const QString &id, const QString &title) { _menuTitles.insert(id, title); }
	void declareActionTitle(const QString &id, const QString &title) { _actionTitles.insert(id, title); }
	void declareActionShortcut(const QString &id, const QKeySequence &shortcut) { _actionShortcuts.insert(id, shortcut); }
	
	QHash<QString, QString> actionTitles() const { return _actionTitles; }
	QHash<QString, QString> menuTitles() const{ return _menuTitles; }
	QHash<QString, QKeySequence> actionShortcuts() const { return _actionShortcuts; }
	
signals:
	
public slots:
	
private:
	
	WorkspaceManager *_workspaceManager;
	
	QList<ToolFactory *> _toolFactories;
	
	QHash<QString, QString> _actionTitles, _menuTitles;
	QHash<QString, QKeySequence> _actionShortcuts;
};

inline Application *app() { return static_cast<Application *>(QCoreApplication::instance()); }

}

#endif // FSAPPLICATION_H
