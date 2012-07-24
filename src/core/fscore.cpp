#include "src/tool/fslayermovetool.h"
#include "src/tool/fsbrushtool.h"
#include "fscore.h"

FSCore::FSCore(QObject *parent) :
	QObject(parent)
{
	Q_ASSERT(_instance == 0);
	_instance = this;
	
	_toolManager = new FSToolManager(this);
	_paletteManager = new FSPaletteManager(this);
	
	_toolManager->addToolFactory(new FSLayerMoveToolFactory());
	_toolManager->addToolFactory(new FSBrushToolFactory());
	
	_toolManager->setCurrentToolFactory("layerMove");
}

FSCore *FSCore::_instance;
