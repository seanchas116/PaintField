#include "core/application.h"
#include "modules/documentUI/documentmanagercontroller.h"

#include "documenttabbar.h"

namespace PaintField
{

DocumentTabBar::DocumentTabBar(QWidget *parent) :
    QTabBar(parent)
{
}

void DocumentTabBar::addDocument(Document *document)
{
	_documentList << document;
	addTab(document->fileName());
}

void DocumentTabBar::removeDocument(Document *document)
{
	int index = _documentList.indexOf(document);
	_documentList.removeAt(index);
	removeTab(index);
}

void DocumentTabBar::changeCurrentDocument(Document *document)
{
	int index = _documentList.indexOf(document);
	setCurrentIndex(index);
}

void DocumentTabBar::onCurrentChanged(int index)
{
	emit currentDocumentChanged(_documentList.at(index));
}

void DocumentTabBar::onTabCloseRequested(int index)
{
	DocumentManagerController::closeDocument(_documentList.at(index));
}


}
