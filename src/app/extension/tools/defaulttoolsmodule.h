#ifndef DEFAULTTOOLSMODULE_H
#define DEFAULTTOOLSMODULE_H

#include <QObject>
#include "applicationmodulebase.h"

namespace PaintField
{

class DefaultToolsModule : public ApplicationModuleBase
{
	Q_OBJECT
public:
	explicit DefaultToolsModule(QObject *parent = 0);
	
	 void initialize();
	
signals:
	
public slots:
	
};

}

#endif // DEFAULTTOOLSMODULE_H
