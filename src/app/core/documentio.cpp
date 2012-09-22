#include <QtCore>
#include <QJson/Parser>
#include <QJson/Serializer>

#include "documentio.h"

namespace PaintField
{

using namespace Malachite;

bool saveIntoZip(zipFile zip, const QString &path, const QByteArray &data, int level)
{
	zipOpenNewFileInZip64(zip, path.toLocal8Bit(), 0, 0, 0, 0, 0, 0, Z_DEFLATED, level, 1);
	
	zipWriteInFileInZip(zip, data.data(), data.length());
	
	zipCloseFileInZip(zip);
}

QByteArray loadFromUnzip(unzFile unzip, const QString &path)
{
	if (unzLocateFile(unzip, path.toLocal8Bit(), 1) != UNZ_OK)
	{
		return QByteArray();	// falied
	}
	else
	{
		unz_file_info64 fileInfo;
		unzGetCurrentFileInfo64(unzip, &fileInfo, 0, 0, 0, 0, 0, 0);
		size_t dataSize = fileInfo.uncompressed_size;
		
		QByteArray data(dataSize);
		
		unzOpenCurrentFile(unzip);
		unzReadCurrentFile(unzip, data.data(), dataSize);
		unzCloseCurrentFile(unzip);
		
		return data;
	}
}

DocumentIO::DocumentIO(const QString &path, QObject *parent) :
    QObject(parent),
    _unzip(0),
    _path(path)
{
	if (path.isEmpty() == false)
	{
		// open unzip file
		_unzip = unzOpen64(path.toLocal8Bit());
	}
}

DocumentIO::~DocumentIO()
{
	if (_unzip)
		unzClose(_unzip);
}

bool DocumentIO::saveAs(Document *document, const QString &newPath)
{
	if (newPath.isEmpty())
	{
		qWarning() << Q_FUNC_INFO << ": filePath is empty";
		return false;
	}
	
	QString tempFilePath = createTemporaryFilePath();
	
	zipFile zip = zipOpen64(tempFilePath.toLocal8Bit(), APPEND_STATUS_CREATE);
	if (zip == 0)
	{
		qWarning() << __PRETTY_FUNCTION__ << ": unable to create temporary file";
		return false;
	}
	
	DocumentDatabase database(this);
	
	QVariantMap documentData;
	documentData["width"] = document->width();
	documentData["height"] = document->height();
	documentData["version"] = "1.0";
	
	QVariantList stack;
	
	if (!saveLayerRecursive(document->layerModel()->rootLayer(), &database, &stack))
	{
		qWarning() << Q_FUNC_INFO << ": unable to save file";
		return false;
	}
	
	QJson::Serializer serializer;
	QByteArray headerJson = serializer.serialize(documentData);
	
	// write data into zip archive
	
	if (database.save(zip) == false)
	{
		qWarning() << Q_FUNC_INFO << ": unable to save file";
		return false;
	}
	
	if (saveIntoZip(zip, "header.json", headerJson) == false)
	{
		qWarning() << Q_FUNC_INFO << ": unable to save file";
		return false;
	}
	
	zipClose(_zip);
	
	// move archive to specified place
	
	if (QFile::exists(newPath))
	{
		QFile::remove(newPath);
	}
	
	if (!QFile::copy(tempFilePath, newPath) ) {
		qWarning() << __PRETTY_FUNCTION__ << "unable to copy file to the specified place";
		return false;
	}
	
	// succeeded to save file
	document->setFilePath(filePath);
	document->setModified(false);
	return true;
}

bool DocumentIO::saveLayerRecursive(const Layer *parent, AbstractDocumentDatabaseWriter *database, QVariantList *result)
{
	Q_ASSERT(parent);
	Q_ASSERT(database);
	Q_ASSERT(result);
	
	if (parent->type() != Layer::TypeGroup)
		return false;
	
	QVariantList layerDataList;
	
	foreach (const Layer *layer, parent->children() )
	{
		QVariantMap layerData;
		
		switch (layer->type())
		{
			case Layer::TypeGroup:
				layerData["type"] = "group";
				break;
			case Layer::TypeRaster:
				layerData["type"] = "raster";
				break;
			default:
				continue;
		}
		
		layerData["name"] = layer->name();
		layerData["visible"] = layer->isVisible();
		layerData["locked"] = layer->isLocked();
		layerData["opacity"] = layer->opacity();
		layerData["blendMode"] = Malachite::BlendModeUtil(layer->blendMode()).name();
		
		if (layer->type() == Layer::TypeGroup)
		{
			QVariantList childDataList;
			if (saveLayerRecursive(layer, archive, fileCount, &childDataList))
				layerData["children"] = childDataList;
			else
				return false;
		}
		else if (layer->type() == Layer::TypeRaster && !layer->surface().isNull())
		{
			layerData["source"] = database->addSurface(layer->surface());
		}
		
		layerDataList << layerData;
	}
	
	*result = layerDataList;
	return true;
}

DocumentDatabase::DocumentDatabase(DocumentIO *documentIO) :
    _documentIO(documentIO),
    _surfaceCount(0),
    _tileCount(0)
{
	
}

QString DocumentDatabase::addSurface(const Malachite::Surface &surface)
{
	QVariantList tiles;
	
	foreach (const QPoint &key, surface.keys())
	{
		QVariantMap tileData;
		tileData["x"] = key.x();
		tileData["y"] = key.y();
		tileData["source"] = addTile(surface.tileForKey(key));
		
		tiles << tileData;
	}
	
	QVariantMap surfaceData;
	surfaceData["tileWidth"] = Surface::TileSize;
	surfaceData["tileHeight"] = Surface::TileSize;
	surfaceData["tiles"] = tiles;
	
	_surfacesToSave << SurfaceSaveInfo(surfaceData, "data/surfaces/" + QString::number(_surfaceCount) + ".surface");
	++_surfaceCount;
}

void DocumentDatabase::save(zipFile *zip)
{
	// saving tiles
	
	foreach (auto tileInfo, _tilesToSave)
	{
		saveIntoZip(zip, tileInfo.path, qCompress(tileInfo.tile.toByteArray()), Z_NO_COMPRESSION);
	}
	
	// saving surfaces
	
	foreach (auto surfaceInfo, _surfacesToSave)
	{
		QJson::Serializer serializer;
		QByteArray json = serializer.serialize(surfaceInfo.data);
		saveIntoZip(zip, surfaceInfo.path, json, Z_DEFAULT_COMPRESSION);
	}
}

QString DocumentDatabase::addTile(const Malachite::Image &tile)
{
	for (auto iter = _tilesToSave.begin(); iter != _tilesToSave.end(); ++iter)
	{
		if (tile.referenceIsEqualTo(iter->tile))
			return iter->path;
	}
	
	_tilesToSave << TileSaveInfo(tile, "data/tiles/" + QString::number(_tileCount) + ".tile");
	++_tileCount;
}

}
