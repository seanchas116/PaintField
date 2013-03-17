#include <QtCore>
#include <qjson/parser.h>
#include <qjson/serializer.h>
#include <Malachite/Painter>
#include <stdexcept>
#include <Minizip/unzip.h>
#include <Minizip/zip.h>

#include "grouplayer.h"
#include "layerfactorymanager.h"
#include "randomstring.h"

#include "documentio.h"

using namespace Malachite;
using namespace std;

namespace PaintField
{

static void saveIntoZip(zipFile zip, const QString &path, const QByteArray &data, int level)
{
	if (zipOpenNewFileInZip64(zip, path.toLocal8Bit(), 0, 0, 0, 0, 0, 0, Z_DEFLATED, level, 1) != ZIP_OK)
		throw runtime_error("cannot create file in archive");
	
	zipWriteInFileInZip(zip, data.data(), data.length());
	zipCloseFileInZip(zip);
}

static QByteArray loadFromUnzip(unzFile unzip, const QString &path)
{
	if (unzLocateFile(unzip, path.toLocal8Bit(), 1) != UNZ_OK)
		throw runtime_error("cannot find file in archive");
	
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

struct DocumentSaver::Data
{
	zipFile zip = 0;
	Document *document;
	
	QVariantList saveLayerChildrenRecursive(const Layer *parent, int &sourceFileCount)
	{
		Q_ASSERT(parent);
		
		QVariantList maps;
		
		for (const Layer *layer : parent->children())
		{
			auto map = layer->saveProperies();
			
			map["type"] = layerFactoryManager()->nameForTypeInfo(typeid(*layer));
			
			if (layer->hasDataToSave())
			{
				QByteArray data;
				QDataStream stream(&data, QIODevice::WriteOnly);
				layer->saveDataFile(stream);
				
				QString path = "data/" + QString::number(sourceFileCount) + "." + layer->dataSuffix();
				sourceFileCount++;
				
				saveIntoZip(zip, path, data, Z_DEFAULT_COMPRESSION);
				
				map["source"] = path;
			}
			
			if (layer->count())
			{
				map["children"] = saveLayerChildrenRecursive(layer, sourceFileCount);
			}
			
			maps << map;
		}
		
		return maps;
	}
};

DocumentSaver::DocumentSaver(Document *document, QObject *parent) :
	QObject(parent),
	d(new Data)
{
	d->document = document;
}

DocumentSaver::~DocumentSaver()
{
	delete d;
}

bool DocumentSaver::save(const QString &filePath)
{
	try
	{
		QString tempFilePath = createTemporaryFilePath();
		
		if (QFile::exists(tempFilePath))
			QFile::remove(tempFilePath);
		
		d->zip = zipOpen64(tempFilePath.toLocal8Bit(), APPEND_STATUS_CREATE);
		if (!d->zip)
			throw runtime_error("failed to create temp file");
		
		{
			QVariantMap headerMap;
			headerMap["width"] = d->document->width();
			headerMap["height"] = d->document->height();
			headerMap["version"] = "1.0";
			
			{
				int count = 0;
				headerMap["stack"] = d->saveLayerChildrenRecursive(d->document->layerModel()->rootLayer(), count);
			}
			
			{
				QJson::Serializer serializer;
				QByteArray headerJson = serializer.serialize(headerMap);
				saveIntoZip(d->zip, "header.json", headerJson, Z_DEFAULT_COMPRESSION);
			}
		}
		
		zipClose(d->zip, 0);
		d->zip = 0;
		
		if (QFile::exists(filePath))
			QFile::remove(filePath);
		
		if (!QFile::rename(tempFilePath, filePath))
			throw runtime_error("unable to copy file to the specified place");
		
		d->document->setFilePath(filePath);
		d->document->setModified(false);
		return true;
	}
	catch (const runtime_error &error)
	{
		PAINTFIELD_WARNING << error.what();
		
		if (d->zip)
		{
			zipClose(d->zip, 0);
			d->zip = 0;
		}
		
		return false;
	}
}

struct DocumentLoader::Data
{
	unzFile unzip = 0;
	
	void loadLayerChildrenRecursive(Layer *parent, const QVariantList &propertyMaps)
	{
		for (const QVariant &item : propertyMaps)
		{
			QVariantMap map = item.toMap();
			PAINTFIELD_DEBUG << map;
			
			Layer *layer = layerFactoryManager()->createLayer(map["type"].toString());
			if (!layer)
				continue;
			
			layer->loadProperties(map);
			
			if (layer->hasDataToSave() && map.contains("source"))
			{
				QString source = map["source"].toString();
				auto data = loadFromUnzip(unzip, source);
				
				QDataStream stream(&data, QIODevice::ReadOnly);
				layer->loadDataFile(stream);
			}
			
			if (layer->canHaveChildren())
				loadLayerChildrenRecursive(layer, map["children"].toList());
			
			parent->append(layer);
		}
	}
};

DocumentLoader::DocumentLoader(QObject *parent) :
	QObject(parent),
	d(new Data)
{}

DocumentLoader::~DocumentLoader()
{
	delete d;
}

Document *DocumentLoader::load(const QString &filePath, QObject *parent)
{
	try
	{
		d->unzip = unzOpen64(filePath.toLocal8Bit());
		
		QVariantMap headerMap;
		
		{
			QJson::Parser parser;
			headerMap = parser.parse(loadFromUnzip(d->unzip, "header.json")).toMap();
		}
		
		if (headerMap["version"].toString() != "1.0")
			throw runtime_error("incompatible file version");
		
		QSize size;
		size.rwidth() = headerMap["width"].toInt();
		size.rheight() = headerMap["height"].toInt();
		
		if (size.isEmpty())
			throw runtime_error("invalid size");
		
		QVariantList stack = headerMap["stack"].toList();
		
		GroupLayer group;
		d->loadLayerChildrenRecursive(&group, stack);
		
		unzClose(d->unzip);
		d->unzip = 0;
		
		auto document = new Document(QString(), size, group.takeAll(), parent);
		document->setFilePath(filePath);
		return document;
	}
	catch (const runtime_error &error)
	{
		PAINTFIELD_WARNING << error.what();
		
		if (d->unzip)
		{
			unzClose(d->unzip);
			d->unzip = 0;
		}
		
		return 0;
	}
}

}
