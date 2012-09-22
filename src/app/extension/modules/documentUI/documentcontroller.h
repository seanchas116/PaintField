#ifndef FSDOCUMENTCONTROLLER_H
#define FSDOCUMENTCONTROLLER_H

#include <QObject>
#include "documentmanagercontroller.h"

namespace PaintField {

class DocumentController : public QObject
{
	Q_OBJECT
public:
	DocumentController(Document *document, QObject *parent = 0);
	
signals:
	
public slots:
	
	void closeDocument() { DocumentManagerController::closeDocument(_document); }
	void saveAsDocument() { DocumentManagerController::saveAsDocument(_document); }
	void saveDocument() { DocumentManagerController::saveDocument(_document); }
	
private:
	
	Document *_document;
};

}

#endif // FSDOCUMENTCONTROLLER_H
