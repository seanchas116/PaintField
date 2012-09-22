#ifndef FSDOCUMENTMANAGERCONTROLLER_H
#define FSDOCUMENTMANAGERCONTROLLER_H

#include <QObject>
#include "../document/document.h"

namespace PaintField {

class DocumentManagerController : public QObject
{
	Q_OBJECT
public:
	explicit DocumentManagerController(QObject *parent = 0);
	
	static Document *newDocument();
	static Document *openDocument();
	static bool closeDocument(Document *document);
	static bool saveDocument(Document *document);
	static bool saveAsDocument(Document *document);
	static bool exportDocument(Document *document);
	
signals:
	
public slots:
	
	void newAndAddDocument();
	void openAndAddDocument();
	
private:
};

}

#endif // FSDOCUMENTMANAGERCONTROLLER_H
