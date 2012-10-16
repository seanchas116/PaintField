#ifndef EXTENSIONMODULE_H
#define EXTENSIONMODULE_H

#include "core/module.h"

namespace PaintField
{

class WorkspaceController;
class CanvasController;

class ExtensionModule : public Module
{
	Q_OBJECT
public:
	explicit ExtensionModule(QObject *parent = 0);
	
	void initialize();
	
signals:
	
public slots:
	
private slots:
	
	void onWorkspaceAdded(WorkspaceController *workspace);
	void onCanvasAdded(CanvasController *canvas);
};

}

#endif // EXTENSIONMODULE_H
