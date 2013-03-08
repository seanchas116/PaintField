#ifndef DOCUMENTIO_H
#define DOCUMENTIO_H

#include <QObject>
#include <QByteArray>

#include <Malachite/Surface>

#include "document.h"

namespace PaintField
{

class DocumentDatabase;

class DocumentSaver : public QObject
{
	Q_OBJECT
	
public:
	
	explicit DocumentSaver(Document *document, QObject *parent = 0);
	~DocumentSaver();
	
	bool save(const QString &filePath);
	
private:
	
	struct Data;
	Data *d;
};

class DocumentLoader : public QObject
{
	Q_OBJECT
	
public:
	
	explicit DocumentLoader(QObject *parent = 0);
	~DocumentLoader();
	
	Document *load(const QString &filePath, QObject *parent);
	
private:
	
	struct Data;
	Data *d;
};

}



#endif // DOCUMENTIO_H
