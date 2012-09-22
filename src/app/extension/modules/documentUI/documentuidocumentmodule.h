#ifndef FSDOCUMENTUIDOCUMENTMODULE_H
#define FSDOCUMENTUIDOCUMENTMODULE_H

#include <QObject>
#include "../document/documentmodulebase.h"

namespace PaintField {

class DocumentUIDocumentModule : public DocumentModuleBase
{
	Q_OBJECT
public:
	explicit DocumentUIDocumentModule(QObject *parent = 0);
	
	void firstInitialize(Document *document);
	
signals:
	
public slots:
	
};

}

#endif // FSDOCUMENTUIDOCUMENTMODULE_H
