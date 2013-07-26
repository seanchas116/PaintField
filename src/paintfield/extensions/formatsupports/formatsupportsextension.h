#pragma once

#include "paintfield/core/extension.h"

namespace PaintField {

class FormatSupportsExtension : public AppExtension
{
	Q_OBJECT
public:
	explicit FormatSupportsExtension(AppController *appC, QObject *parent = 0);
	
signals:
	
public slots:
	
};


class FormatSupportsExtensionFactory : public ExtensionFactory
{
	Q_OBJECT
public:
	
	explicit FormatSupportsExtensionFactory(QObject *parent = 0);
	
	void initialize(AppController *app) override;
	
	AppExtensionList createAppExtensions(AppController *app, QObject *parent);
	
signals:
	
public slots:
	
};

} // namespace PaintField
