#ifndef DOCUMENTIO_H
#define DOCUMENTIO_H

#include <QObject>
#include <QByteArray>

#include <Malachite/Surface>

#include "document.h"

namespace PaintField
{

class DocumentDatabase;

class DocumentWriter : public QObject
{
	Q_OBJECT
	
public:
	
	explicit DocumentWriter(Document *document, QObject *parent = 0);
	~DocumentWriter();
	
	bool save(const QString &filePath);
	
private:
	
	struct Data;
	Data *d;
};

class DocumentReader : public QObject
{
	Q_OBJECT
	
public:
	
	explicit DocumentReader(QObject *parent = 0);
	~DocumentReader();
	
	Document *load(const QString &filePath, QObject *parent);
	
private:
	
	struct Data;
	Data *d;
};

}



#endif // DOCUMENTIO_H
