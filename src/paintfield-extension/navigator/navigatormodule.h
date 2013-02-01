#ifndef NAVIGATORMODULE_H
#define NAVIGATORMODULE_H

#include <QObject>
#include "paintfield-core/module.h"

namespace PaintField
{

class NavigatorController;
class NavigatorView;

class NavigatorModule : public CanvasModule
{
	Q_OBJECT
public:
	NavigatorModule(Canvas *canvas, QObject *parent);
	
signals:
	
public slots:
	
};

class NavigatorModuleFactory : public ModuleFactory
{
	Q_OBJECT
public:
	NavigatorModuleFactory(QObject *parent = 0) : ModuleFactory(parent) {}
	
	void initialize(AppController *app) override;
	
	CanvasModuleList createCanvasModules(Canvas *canvas, QObject *parent);
};

}

#endif // NAVIGATORMODULE_H
