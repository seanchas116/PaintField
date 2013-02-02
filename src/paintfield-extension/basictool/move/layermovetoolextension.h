#pragma once

#include "paintfield-core/extension.h"

namespace PaintField
{

class LayerMoveToolExtension : public AppExtension
{
	Q_OBJECT
public:
	LayerMoveToolExtension(AppController *app, QObject *parent) : AppExtension(app, parent) {}
	
	Tool *createTool(const QString &name, Canvas *canvas) override;
	
signals:
	
public slots:
	
};

class LayerMoveToolExtensionFactory : public ExtensionFactory
{
	Q_OBJECT
	
public:
	
	LayerMoveToolExtensionFactory(QObject *parent = 0) : ExtensionFactory(parent) {}
	
	void initialize(AppController *app) override;
	
	AppExtensionList createAppExtensions(AppController *app, QObject *parent) override
	{
		return { new LayerMoveToolExtension(app, parent) };
	}
};

}

