#include <QtCore>
#include <QJson/Parser>
#include <QJson/Serializer>
#include <Malachite/Painter>

#include "randomstring.h"

#include "documentio.h"

namespace PaintField
{

using namespace Malachite;

bool saveIntoZip(zipFile zip, const QString &path, const QByteArray &data, int level)
{
	if (zipOpenNewFileInZip64(zip, path.toLocal8Bit(), 0, 0, 0, 0, 0, 0, Z_DEFLATED, level, 1) != ZIP_OK)
		return false;
	
	zipWriteInFileInZip(zip, data.data(), data.length());
	
	zipCloseFileInZip(zip);
	
	return true;
}

QByteArray loadFromUnzip(unzFile unzip, const QString &path)
{
	if (unzLocateFile(unzip, path.toLocal8Bit(), 1) != UNZ_OK)
		return QByteArray();
	
	unz_file_info64 fileInfo;
	unzGetCurrentFileInfo64(unzip, &fileInfo, 0, 0, 0, 0, 0, 0);
	size_t dataSize = fileInfo.uncompressed_size;
	
	QByteArray data;
	data.resize(dataSize);
	
	if (unzOpenCurrentFile(unzip) != UNZ_OK)
		return QByteArray();
	
	unzReadCurrentFile(unzip, data.data(), dataSize);
	unzCloseCurrentFile(unzip);
	
	return data;
}

DocumentIO::DocumentIO(const QString &path, QObject *parent) :
    QObject(parent),
    _unzip(0),
    _path(path)
{
}

DocumentIO::~DocumentIO()
{
	if (_unzip)
		closeUnzip();
}

bool DocumentIO::openUnzip()
{
	if (_path.isEmpty())
		return false;
	
	_unzip = unzOpen64(_path.toLocal8Bit());
	return _unzip;
}

void DocumentIO::closeUnzip()
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
	
	zipClose(zip, 0);
	
	// move archive to specified place
	
	if (QFile::exists(newPath))
		QFile::remove(newPath);
	
	if (!QFile::copy(tempFilePath, newPath) )
	{
		qWarning() << __PRETTY_FUNCTION__ << "unable to copy file to the specified place";
		return false;
	}
	
	// succeeded to save file
	document->setFilePath(newPath);
	document->setModified(false);
	return true;
}

Document *DocumentIO::load(QObject *parent)
{
	if (openUnzip() == false)
	{
		qWarning() << Q_FUNC_INFO << ": unable to open file";
		return 0;
	}
	
	QByteArray headerJson = loadFromUnzip(unzipFile(), "header.json");
	
	if (headerJson.isNull())
	{
		qWarning() << Q_FUNC_INFO << ": failed to get header.json";
		return 0;
	}
	
	bool ok;
	QJson::Parser parser;
	
	QVariantMap documentData = parser.parse(headerJson, &ok).toMap();
	
	if (!ok)
	{
		qWarning() << Q_FUNC_INFO << ": parsing failed";
		return 0;
	}
	
	if (documentData.value("version") != "1.0")
	{
		qWarning() << Q_FUNC_INFO << ": incompatible file version";
		return 0;
	}
	
	GroupLayer group;
	QSize size;
	
	size.rwidth() = documentData.value("width").toInt();
	size.rheight() = documentData.value("height").toInt();
	
	if (size.isEmpty())
	{
		qWarning() << Q_FUNC_INFO << ": invalid size";
		return 0;
	}
	
	QVariantList stack = documentData.value("stack").toList();
	
	DocumentDatabase database(this);
	
	if (!loadLayerRecursive(&group, &database, stack))
	{
		qWarning() << Q_FUNC_INFO << ": cannot compose layer stack";
		return 0;
	}
	
	closeUnzip();
	
	Document *document = new Document(QString(), size, group.takeChildren(), parent);
	document->setFilePath(_path);
	return document;
}

bool DocumentIO::saveLayerRecursive(const Layer *parent, DocumentDatabase *database, QVariantList *result)
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
			if (saveLayerRecursive(layer, database, &childDataList))
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

bool DocumentIO::loadLayerRecursive(Layer *parent, DocumentDatabase *database, const QVariantList &layerDataList)
{
	Q_ASSERT(parent);
	Q_ASSERT(database);
	
	foreach (const QVariant listItem, layerDataList)
	{
		QVariantMap layerData = listItem.toMap();
		
		QString typeName = layerData["type"].toString();
		
		if (typeName == "group")
		{
			GroupLayer *group = new GroupLayer();
			
			group->setName(layerData["name"].toString());
			group->setVisible(layerData["visible"].toBool());
			group->setLocked(layerData["locked"].toBool());
			group->setOpacity(layerData["opacity"].toDouble());
			group->setBlendMode(Malachite::BlendModeUtil(layerData["blendMode"].toString()).index());
			
			parent->appendChild(group);
			
			if (!loadLayerRecursive(group, database, layerData["children"].toList()))
				return false;
		}
		else if (typeName == "raster")
		{
			RasterLayer *raster = new RasterLayer();
			
			raster->setName(layerData["name"].toString());
			raster->setVisible(layerData["visible"].toBool());
			raster->setLocked(layerData["locked"].toBool());
			raster->setOpacity(layerData["opacity"].toDouble());
			raster->setBlendMode(Malachite::BlendModeUtil(layerData["blendMode"].toString()).index());
			
			QString source = layerData["source"].toString();
			
			
			if (!source.isEmpty())
			{
				raster->setSurface(database->loadSurface(source));
			}
			
			parent->appendChild(raster);
		}
	}
	
	return true;
}

DocumentDatabase::DocumentDatabase(DocumentIO *documentIO) :
    _documentIO(documentIO),
    _surfaceCount(0),
    _tileCount(0)
{
	
}

Malachite::Surface DocumentDatabase::loadSurface(const QString &path)
{
	QByteArray data = loadFromUnzip(_documentIO->unzipFile(), path);
	
	if (data.isEmpty())
		return Surface();
	
	QJson::Parser parser;
	
	QVariant json = parser.parse(data);
	
	QVariantMap jsonMap = json.toMap();
	
	QSize tileSize;
	tileSize.setWidth(jsonMap["tileWidth"].toInt());
	tileSize.setHeight(jsonMap["tileHeight"].toInt());
	
	if (tileSize.width() <= 0 || tileSize.height() <= 0)
		return Surface();
	
	QVariant tiles = jsonMap["tiles"];
	
	QVariantList tileList = tiles.toList();
	
	Surface surface;
	
	if (tileSize.width() == Surface::TileSize && tileSize.height() == Surface::TileSize)
	{
		SurfaceEditor editor(&surface);
		
		for (const QVariant &tileData : tileList)
		{
			QPoint key;
			Image tile = loadTileData(tileData, tileSize, &key);
			
			if (!tile.isValid())
				continue;
			
			if (!surface.contains(key))
				editor.replaceTile(key, new Image(tile));
		}
	}
	else
	{
		Painter painter(&surface);
		painter.setBlendMode(Malachite::BlendModeSource);
		
		for (const QVariant &tileData : tileList)
		{
			QPoint key;
			Image tile = loadTileData(tileData, tileSize, &key);
			
			if (!tile.isValid())
				continue;
			
			painter.drawImage(key.x() * tileSize.width(), key.y() * tileSize.height(), tile);
		}
	}
	
	return surface;
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
	
	QString path = "data/surfaces/" + QString::number(_surfaceCount) + ".surface";
	
	_surfacesToSave << SurfaceSaveInfo(surfaceData, path);
	++_surfaceCount;
	
	return path;
}

bool DocumentDatabase::save(zipFile zip)
{
	// saving tiles
	
	foreach (auto tileInfo, _tilesToSave)
	{
		if (!saveIntoZip(zip, tileInfo.path, qCompress(tileInfo.tile.toByteArray()), Z_NO_COMPRESSION))
			return false;
	}
	
	// saving surfaces
	
	foreach (auto surfaceInfo, _surfacesToSave)
	{
		QJson::Serializer serializer;
		QByteArray json = serializer.serialize(surfaceInfo.data);
		if (!saveIntoZip(zip, surfaceInfo.path, json, Z_DEFAULT_COMPRESSION))
			return false;
	}
	
	return true;
}

Image DocumentDatabase::loadTile(const QString &path, const QSize &size)
{
	if (path.isEmpty())
		return Image();
	
	QByteArray compressed = loadFromUnzip(_documentIO->unzipFile(), path);
	if (compressed.isEmpty())
		return Image();
	
	QByteArray data = qUncompress(compressed);
	
	return Image::fromByteArray(data, size);
}

Image DocumentDatabase::loadTileData(const QVariant &tileData, const QSize &size, QPoint *key)
{
	QVariantMap tileMap = tileData.toMap();
	*key = QPoint(tileMap["x"].toInt(), tileMap["y"].toInt());
	
	return loadTile(tileMap["source"].toString(), size);
}

QString DocumentDatabase::addTile(const Malachite::Image &tile)
{
	for (auto iter = _tilesToSave.begin(); iter != _tilesToSave.end(); ++iter)
	{
		if (tile.referenceIsEqualTo(iter->tile))
			return iter->path;
	}
	
	QString path = "data/tiles/" + QString::number(_tileCount) + ".tile";
	
	_tilesToSave << TileSaveInfo(tile, path);
	++_tileCount;
	
	return path;
}

}
