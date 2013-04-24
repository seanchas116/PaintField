#pragma once

#include "paintfield/core/extension.h"

namespace PaintField {

class RootExtensionFactory : public ExtensionFactory
{
	Q_OBJECT
public:
	explicit RootExtensionFactory(QObject *parent = 0);
	
	void initialize(AppController *app) override;
	
signals:
	
public slots:
	
};

} // namespace PaintField
