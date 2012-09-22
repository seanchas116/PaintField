#ifndef FSDOCUMENTMODULEBASE_H
#define FSDOCUMENTMODULEBASE_H

#include <QObject>
#include "document.h"
#include "core/applicationmodulebase.h"

namespace PaintField {

class DocumentModuleBase : public QObject
{
	Q_OBJECT
public:
	explicit DocumentModuleBase(QObject *parent = 0);
	
	virtual void firstInitialize(Document *document) { Q_UNUSED(document) }
	virtual void secondInitialize(Document *document) { Q_UNUSED(document) }
	
signals:
	
public slots:
	
};

class DocumentApplicationModuleBase : public ApplicationModuleBase
{
	Q_OBJECT
	
public:
	
	DocumentApplicationModuleBase(QObject *parent = 0) : ApplicationModuleBase(parent) {}
	
	virtual DocumentModuleBase *createDocumentModule();
};

}

#endif // FSDOCUMENTMODULEBASE_H
