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
	
	return _unzip = unzOpen64(_path.toLocal8Bit());
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

bool DocumentIO::load(QObject *parent)
{
	if (openUnzip() == false)
	{
		qWarning() << Q_FUNC_INFO << ": unable to open file";
		return false;
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
	document->setFilePath(filePath);
	return document;
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

bool DocumentIO::loadLayerRecursive(Layer *parent, DocumentDatabase *database, const QVariantList &layerDataList)
{
	Q_ASSERT(parent);
	Q_ASSERT(archive);
	
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
			
			if (!loadLayerRecursive(group, archive, layerData["children"].toList()))
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
				QByteArray byteArray = archive->readFile(source);
				
				if (byteArray.isNull())
					return false;
				
				QBuffer buffer(&byteArray);
				buffer.open(QIODevice::ReadOnly);
				raster->setSurface(Malachite::Surface::loaded(&buffer));
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
