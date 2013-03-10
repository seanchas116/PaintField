
#include <QHash>

#include "canvas.h"
#include "document.h"
#include "documentcontroller.h"

#include "documentreferencemanager.h"

namespace PaintField {

struct DocumentReferenceManager::Data
{
	QHash<Document *, QSet<Canvas *> > documentsToCanvasLists;
	
	void removeDocument(Document *document)
	{
		document->deleteLater();
		documentsToCanvasLists.remove(document);
	}
};

DocumentReferenceManager::DocumentReferenceManager(QObject *parent) :
	QObject(parent),
	d(new Data)
{}

DocumentReferenceManager::~DocumentReferenceManager()
{
	delete d;
}

void DocumentReferenceManager::addCanvas(Canvas *canvas)
{
	auto document = canvas->document();
	document->setParent(this);
	d->documentsToCanvasLists[document] << canvas;
}

void DocumentReferenceManager::removeCanvas(Canvas *canvas)
{
	auto document = canvas->document();
	
	if (!d->documentsToCanvasLists.contains(document))
		return;
	
	Q_ASSERT(d->documentsToCanvasLists[document].size());
	
	d->documentsToCanvasLists[document].remove(canvas);
	
	if (d->documentsToCanvasLists[document].isEmpty())
		d->removeDocument(document);
}

bool DocumentReferenceManager::tryRemoveCanvas(Canvas *canvas)
{
	auto document = canvas->document();
	
	if (!d->documentsToCanvasLists.contains(document))
		return true;
	
	if (d->documentsToCanvasLists[document].size() == 1 && d->documentsToCanvasLists[document].contains(canvas))
	{
		if (!DocumentController::confirmClose(document))
			return false;
	}
	removeCanvas(canvas);
	return true;
}

} // namespace PaintField
