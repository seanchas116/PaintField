#ifndef FSDOCUMENTMANAGER_H
#define FSDOCUMENTMANAGER_H

#include <QObject>
#include "document.h"

namespace PaintField {

class DocumentManager : public QObject
{
	Q_OBJECT
public:
	explicit DocumentManager(QObject *parent = 0);
	~DocumentManager();
	
	Document *currentDocument() { return _currentDocument; }
	
	void addDocument(Document *document);
	void removeDocument(Document *document);
	
signals:
	
	void documentAdded(Document *document);
	void documentRemoved(Document *document);
	void documentListChanged(const QList<Document *> &list);
	void currentDocumentChanged(Document *document);
	void documentVisibleChanged(Document *document, bool visible);
	
public slots:
	
	void setDocumentVisible(Document *document, bool visible);
	void setCurrentDocument(Document *document);
	
private:
	
	QList<Document *> _documentList;
	Document *_currentDocument;
};

}

#endif // FSDOCUMENTMANAGER_H
