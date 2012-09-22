#ifndef DOCUMENTIO_H
#define DOCUMENTIO_H

#include <QObject>
#include <QByteArray>
#include <Minizip/unzip.h>
#include <Minizip/zip.h>
#include <Malachite/mlsurface.h>

#include "document.h"

namespace PaintField
{

bool saveIntoZip(zipFile zip, const QString &path, const QByteArray &data, int level);
QByteArray loadFromUnzip(unzFile unzip, const QString &path);

class DocumentIO : public QObject
{
public:
	
	DocumentIO(const QString &path, QObject *parent = 0);
	~DocumentIO();
	
	QString path() const { return _path; }
	
	unzFile unzipFile() { return _unzip; }
	
	void saveAs(Document *document, const QString &newPath = path());
	
private:
	
	bool saveLayerRecursive(const Layer *parent, AbstractDocumentDatabaseWriter *database, QVariantList *result);
	
	unzFile _unzip;
	QString _path;
};

class DocumentDatabase
{
public:
	DocumentDatabase(DocumentIO *documentIO);
	
	QString addSurface(const Malachite::Surface &surface);
	
	void save(zipFile *zip);
	
private:
	
	QString addTile(const Malachite::Image &tile);
	
	DocumentIO *_documentIO;
	
	struct TileSaveInfo
	{
		TileSaveInfo(const Malachite::Image &tile, const QString &path) : tile(tile), path(path) {}
		
		Malachite::Image tile;
		QString path;
	};
	
	struct SurfaceSaveInfo
	{
		SurfaceSaveInfo(const QVariant &data, const QString &path) : surface(surface), path(path) {}
		
		Malachite::Surface data;
		QString path;
	};
	
	int _surfaceCount, _tileCount;
	QList<SurfaceSaveInfo> _surfacesToSave;
	QList<TileSaveInfo> _tilesToSave;
};

}



#endif // DOCUMENTIO_H
