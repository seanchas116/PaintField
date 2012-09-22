#ifndef MAINWINDOWMODULE_H
#define MAINWINDOWMODULE_H

#include <QObject>
#include "mainwindow.h"
#include "core/applicationmodulebase.h"

namespace PaintField
{

class MainWindowModule : public ApplicationModuleBase
{
	Q_OBJECT
public:
	explicit MainWindowModule(QObject *parent = 0);
	
	void initialize();
	
	static MainWindow *mainWindow() { return _mainWindow; }
	
signals:
	
public slots:
	
	void onAppModulesLoaded();
	
private:
	
	static MainWindow *_mainWindow;
};

}

#endif // MAINWINDOWMODULE_H
