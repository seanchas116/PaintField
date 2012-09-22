#ifndef FSDOCUMENTMODULE_H
#define FSDOCUMENTMODULE_H

#include <QObject>

#include "documentmanager.h"
#include "core/application.h"
#include "core/applicationmodulebase.h"

namespace PaintField {

class DocumentModule : public ApplicationModuleBase
{
	Q_OBJECT
public:
	explicit DocumentModule(QObject *parent = 0);
	
	void initialize();
	
	static DocumentManager *documentManager() { return app()->findChild<DocumentManager *>(); }
	
signals:
	
public slots:
	
};


}

#endif // FSDOCUMENTMODULE_H
