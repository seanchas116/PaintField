#include <QtGui>

#include "application.h"

namespace PaintField
{

Application::Application(int &argv, char **args) :
    TabletApplication(argv, args)
{
	QTextCodec::setCodecForCStrings(QTextCodec::codecForLocale());
	
	_paletteManager = new PaletteManager(this);
	_toolManager = new ToolManager(this);
	_actionManager = new ActionManager(this);
	_canvasManager = new CanvasManager(this);
}


}
