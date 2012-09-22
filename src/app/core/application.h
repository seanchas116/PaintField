#ifndef FSAPPLICATION_H
#define FSAPPLICATION_H

#include <QMainWindow>
#include "tabletapplication.h"

#include "canvasmanager.h"
#include "toolmanager.h"
#include "palettemanager.h"
#include "actionManager.h"

namespace PaintField {

class Application : public TabletApplication
{
	Q_OBJECT
public:
	
	Application(int &argv, char **args);
	
	CanvasManager *canvasManager() { return _canvasManager; }
	ToolManager *toolManager() { return _toolManager; }
	PaletteManager *paletteManager() { return _paletteManager; }
	ActionManager *actionManager() { return _actionManager; }
	
signals:
	
public slots:
	
private:
	
	CanvasManager *_canvasManager;
	ToolManager *_toolManager;
	PaletteManager *_paletteManager;
	ActionManager *_actionManager;
};

inline Application *app() { return static_cast<Application *>(QCoreApplication::instance()); }

}

#endif // FSAPPLICATION_H
