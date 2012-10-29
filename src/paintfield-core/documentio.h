#ifndef DOCUMENTIO_H
#define DOCUMENTIO_H

#include <QObject>
#include <QByteArray>
#include <Minizip/unzip.h>
#include <Minizip/zip.h>
#include <Malachite/Surface>

#include "document.h"

namespace PaintField
{

bool saveIntoZip(zipFile zip, const QString &path, const QByteArray &data, int level = Z_DEFAULT_COMPRESSION);
QByteArray loadFromUnzip(unzFile unzip, const QString &path);

class DocumentDatabase;

class DocumentIO : public QObject
{
	Q_OBJECT
	
public:
	
	DocumentIO(const QString &path, QObject *parent = 0);
	~DocumentIO();
	
	QString path() const { return _path; }
	
	bool openUnzip();
	unzFile unzipFile() { return _unzip; }
	void closeUnzip();
	
	bool saveAs(Document *document, const QString &newPath);
	bool save(Document *document) { return saveAs(document, _path); }
	Document *load(QObject *parent);
	
private:
	
	bool saveLayerRecursive(const Layer *parent, DocumentDatabase *database, QVariantList *result);
	bool loadLayerRecursive(Layer *parent, DocumentDatabase *database, const QVariantList &layerDataList);
	
	unzFile _unzip;
	QString _path;
};

class DocumentDatabase
{
public:
	DocumentDatabase(DocumentIO *documentIO);
	
	/**
	 * Loads a surface from the DocumentIO.
	 * The DocumentIO's unzip must be opened.
	 * @param path The path to load from
	 * @return The loaded surface
	 */
	Malachite::Surface loadSurface(const QString &path);
	
	/**
	 * Adds a surface into the list which contains surfaces which will be saved when "save()" is called.
	 * 
	 * @param surface The surface to save
	 * @return The in-archive path to which the surface will be saved
	 */
	QString addSurface(const Malachite::Surface &surface);
	
	/**
	 * Actually saves the surfaces.
	 * @param zip A zip archive to save into
	 * @return true if suceeded
	 */
	bool save(zipFile zip);
	
private:
	
	Malachite::Image loadTile(const QString &path, const QSize &size);
	Malachite::Image loadTileData(const QVariant &tileData, const QSize &size, QPoint *key);
	
	/**
	 * Add a tile into the list which contains tiles which will be saved when "save()" is called.
	 * Does nothing if the same tile has already been added to the list, to avoid duplication.
	 * 
	 * @param tile The tile to save
	 * @return The in-archive path to which the tile will be saved
	 */
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
		SurfaceSaveInfo(const QVariant &data, const QString &path) : data(data), path(path) {}
		
		QVariant data;
		QString path;
	};
	
	int _surfaceCount, _tileCount;
	QList<SurfaceSaveInfo> _surfacesToSave;
	QList<TileSaveInfo> _tilesToSave;
};

}



#endif // DOCUMENTIO_H
