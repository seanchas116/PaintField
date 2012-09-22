#ifndef FSDOCUMENTIO_H
#define FSDOCUMENTIO_H

#include <QObject>

#include "Minizip/unzip.h"
#include "Minizip/zip.h"

#include "document.h"

namespace PaintField {


bool zip64SaveByteArray(zipFile file, const QByteArray &data, const QString &path);
bool zip64

class DefaultDocumentSaver : public QObject
{
	Q_OBJECT
public:
	
	DefaultDocumentSaver(const QString &path, const QString &lastSavedPath, QObject *parent = 0);
	
	bool save(Document *document);
	
private:
	
	bool saveLayerRecursive(const Layer *parent, ZipArchive *archive, int *fileCount, QVariantList *result);
	
	bool saveSurface(const QString &saveName, const Malachite::Surface &surface, const QPointSet &unmodified);
	
	bool saveInZip(const QByteArray &data, const QString &path);
	
	unzFile _lastSavedFile;
	zipFile _file;
};

class DefaultDocumentIO : public QObject
{
	Q_OBJECT
public:
	explicit DefaultDocumentIO(QObject *parent = 0);
	
	static Document *open(const QString &filePath, QObject *parent = 0);
	static bool saveAs(Document *document, const QString &filePath);
	
signals:
	
public slots:
	
private:
	
	static bool saveLayerRecursive(const Layer *parent, ZipArchive *archive, int *fileCount, QVariantList *result);
	static bool loadLayerRecursive(Layer *parent, ZipArchive *archive, const QVariantList &layerDataList);
	
	bool saveSurface(const QString &saveName, const Malachite::Surface &surface);
};

}

#endif // FSDOCUMENTIO_H
