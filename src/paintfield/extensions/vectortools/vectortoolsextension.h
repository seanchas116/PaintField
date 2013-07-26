#pragma once

#include "paintfield/core/extension.h"

namespace PaintField {

class VectorToolsExtension : public CanvasExtension
{
	Q_OBJECT
public:
	explicit VectorToolsExtension(Canvas *canvas, QObject *parent = 0);
	
	Tool *createTool(const QString &name, Canvas *canvas) override;
	
signals:
	
public slots:
	
};

class VectorToolsExtensionFactory : public ExtensionFactory
{
	Q_OBJECT
	
public:
	
	explicit VectorToolsExtensionFactory(QObject *parent = 0) : ExtensionFactory(parent) {}
	
	void initialize(AppController *app) override;
	
	CanvasExtensionList createCanvasExtensions(Canvas *canvas, QObject *parent) override
	{
		return { new VectorToolsExtension(canvas, parent) };
	}
};

}// namespace PaintField

