#ifndef FSDOCUMENTTABBAR_H
#define FSDOCUMENTTABBAR_H

#include <QTabBar>
#include "modules/document/documentmanager.h"

namespace PaintField {

class DocumentTabBar : public QTabBar
{
	Q_OBJECT
public:
	explicit DocumentTabBar(QWidget *parent = 0);
	
signals:
	
	void documentVisibleChanged(Document *document, bool visible);
	void currentDocumentChanged(Document *document);
	
public slots:
	
	void addDocument(Document *document);
	void removeDocument(Document *document);
	void changeCurrentDocument(Document *document);
	
private slots:
	
	void onCurrentChanged(int index);
	void onTabCloseRequested(int index);
	
private:
	
	QList<Document *> _documentList;
};

}

#endif // FSDOCUMENTTABBAR_H
