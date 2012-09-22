#include "documentmanager.h"

namespace PaintField
{

DocumentManager::DocumentManager(QObject *parent) :
    QObject(parent),
    _currentDocument(0)
{
	
}

DocumentManager::~DocumentManager()
{
}

void DocumentManager::addDocument(Document *document)
{
	document->setParent(this);
	_documentList << document;
	
	emit documentAdded(document);
	
	if (_documentList.size() == 1)
		setCurrentDocument(document);
}

void DocumentManager::removeDocument(Document *document)
{
	if (!_documentList.contains(document))
		return;
	
	_documentList.removeOne(document);
	document->deleteLater();
	
	emit documentRemoved(document);
	
	if (_documentList.size() == 0)
		setCurrentDocument(0);
}

void DocumentManager::setDocumentVisible(Document *document, bool visible)
{
	if (_documentList.contains(document))
		emit documentVisibleChanged(document, visible);
	else
		qWarning() << Q_FUNC_INFO << ": invalid document";
}

void DocumentManager::setCurrentDocument(Document *document)
{
	if (_documentList.contains(document))
		emit currentDocumentChanged(document);
	else
		qWarning() << Q_FUNC_INFO << ": invalid document";
}

}
