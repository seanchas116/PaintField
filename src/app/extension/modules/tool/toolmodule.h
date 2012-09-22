#ifndef FSTOOLMODULE_H
#define FSTOOLMODULE_H

#include <QObject>
#include "core/application.h"
#include "core/applicationmodulebase.h"

#include "toolmanager.h"

namespace PaintField
{

class ToolModule : public ApplicationModuleBase
{
	Q_OBJECT
public:
	explicit ToolModule(QObject *parent = 0);
	
	void initialize();
	
	static ToolManager *toolManager() { return app()->findChild<ToolManager *>(); }
	
signals:
	
public slots:
	
};

}

#endif // FSTOOLMODULE_H
