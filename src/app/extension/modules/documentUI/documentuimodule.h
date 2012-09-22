#ifndef FSDOCUMENTUIMODULE_H
#define FSDOCUMENTUIMODULE_H

#include <QObject>
#include "../document/documentmodulebase.h"

namespace PaintField {

class DocumentUIModule : public DocumentApplicationModuleBase
{
	Q_OBJECT
public:
	explicit DocumentUIModule(QObject *parent = 0);
	
	void initialize();
	
	DocumentModuleBase *createDocumentModule();
	
signals:
	
public slots:
	
};

}

#endif // FSDOCUMENTUIMODULE_H
