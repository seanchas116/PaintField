#ifndef FSACTIONMODULE_H
#define FSACTIONMODULE_H

#include <QObject>
#include "core/application.h"
#include "actionmanager.h"
#include "core/applicationmodulebase.h"

namespace PaintField {

class ActionModule : public ApplicationModuleBase
{
	Q_OBJECT
public:
	explicit ActionModule(QObject *parent = 0);
	
	void firstInitialize();
	
	static ActionManager *actionManager() { return app()->findChild<ActionManager *>(); }
	
signals:
	
public slots:
};

}

#endif // FSACTIONMODULE_H
