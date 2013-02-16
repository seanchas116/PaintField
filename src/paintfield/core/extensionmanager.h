#pragma once

#include "extension.h"

namespace PaintField
{

class ExtensionManager : public QObject
{
	Q_OBJECT
public:
	explicit ExtensionManager(QObject *parent = 0);
	
	void addExtensionFactory(ExtensionFactory *factory);
	
	void initialize(AppController *app);
	
	QList<AppExtension *> createAppExtensions(AppController *app, QObject *parent);
	QList<WorkspaceExtension *> createWorkspaceExtensions(Workspace *workspace, QObject *parent);
	QList<CanvasExtension *> createCanvasExtensions(Canvas *canvas, QObject *parent);
	
signals:
	
public slots:
	
private:
	
	QList<ExtensionFactory *> _factories;
};

}
