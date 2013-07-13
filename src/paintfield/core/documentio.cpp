#include <QtCore>
#include <Malachite/Painter>
#include <stdexcept>
#include <minizip/unzip.h>
#include <minizip/zip.h>

#include "json.h"
#include "layerscene.h"
#include "grouplayer.h"
#include "layerfactorymanager.h"
#include "randomstring.h"

#include "documentio.h"

using namespace Malachite;
using namespace std;

namespace PaintField
{

namespace MinizipSupport
{

static voidpf openFile(voidpf opaque, const void *filename, int mode)
{
	Q_UNUSED(opaque);
	auto file = new QFile(QString::fromUtf8(reinterpret_cast<const char *>(filename)));
	QIODevice::OpenModeFlag flag;
	if ((mode & ZLIB_FILEFUNC_MODE_READWRITEFILTER) == ZLIB_FILEFUNC_MODE_READ)
		flag = QIODevice::ReadOnly;
	else if (mode & ZLIB_FILEFUNC_MODE_EXISTING)
		flag = QIODevice::ReadWrite;
	else if (mode & ZLIB_FILEFUNC_MODE_CREATE)
		flag = QIODevice::WriteOnly;
	else
		return nullptr;
	
	if (file->open(flag))
		return file;
	else
		return nullptr;
}

static uLong readFile(voidpf opaque, voidpf stream, void *buf, uLong size)
{
	Q_UNUSED(opaque);
	auto file = reinterpret_cast<QFile *>(stream);
	Q_ASSERT(file);
	return file->read(reinterpret_cast<char *>(buf), size);
}

static uLong writeFile(voidpf opaque, voidpf stream, const void *buf, uLong size)
{
	Q_UNUSED(opaque);
	auto file = reinterpret_cast<QFile *>(stream);
	Q_ASSERT(file);
	return file->write(reinterpret_cast<const char *>(buf), size);
}

static ZPOS64_T tellFile(voidpf opaque, voidpf stream)
{
	Q_UNUSED(opaque);
	auto file = reinterpret_cast<QFile *>(stream);
	Q_ASSERT(file);
	return file->pos();
}

static long seekFile(voidpf opaque, voidpf stream, ZPOS64_T offset, int origin)
{
	Q_UNUSED(opaque);
	auto file = reinterpret_cast<QFile *>(stream);
	Q_ASSERT(file);
	
	uint64_t pos;
	switch (origin)
	{
		case ZLIB_FILEFUNC_SEEK_CUR:
			pos = file->pos() + offset;
			break;
		case ZLIB_FILEFUNC_SEEK_END:
			pos = file->size() + offset;
			break;
		case ZLIB_FILEFUNC_SEEK_SET:
			pos = offset;
			break;
		default:
			return -1;
	}
	
	if (file->seek(pos))
		return 0;
	else
		return -1;
}

static int closeFile(voidpf opaque, voidpf stream)
{
	Q_UNUSED(opaque);
	auto file = reinterpret_cast<QFile *>(stream);
	Q_ASSERT(file);
	file->close();
	delete file;
	return 0;
}

static int errorFile(voidpf opaque, voidpf stream)
{
	Q_UNUSED(opaque);
	auto file = reinterpret_cast<QFile *>(stream);
	Q_ASSERT(file);
	return file->error();
}

static zlib_filefunc64_def fileFuncDefs = 
{
	.zopen64_file = openFile,
	.zread_file = readFile,
	.zwrite_file = writeFile,
	.ztell64_file = tellFile,
	.zseek64_file = seekFile,
	.zclose_file = closeFile,
	.zerror_file = errorFile
};

}

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
		throw runtime_error("cannot open current file");
	
	unzReadCurrentFile(unzip, data.data(), dataSize);
	unzCloseCurrentFile(unzip);
	
	return data;
}

struct DocumentWriter::Data
{
	zipFile zip = 0;
	Document *document;
	
	QVariantList saveLayerChildrenRecursive(const LayerConstRef &parent, int &sourceFileCount)
	{
		Q_ASSERT(parent);
		
		QVariantList maps;
		
		for (const auto &layer : parent->children())
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

DocumentWriter::DocumentWriter(Document *document, QObject *parent) :
	QObject(parent),
	d(new Data)
{
	d->document = document;
}

DocumentWriter::~DocumentWriter()
{
	delete d;
}

bool DocumentWriter::save(const QString &filePath)
{
	try
	{
		QString tempFilePath = createTemporaryFilePath();
		
		if (QFile::exists(tempFilePath))
			QFile::remove(tempFilePath);
		
		d->zip = zipOpen2_64(tempFilePath.toUtf8(), APPEND_STATUS_CREATE, nullptr, &MinizipSupport::fileFuncDefs);
		if (!d->zip)
			throw runtime_error("failed to create temp file");
		
		{
			QVariantMap headerMap;
			headerMap["width"] = d->document->width();
			headerMap["height"] = d->document->height();
			headerMap["version"] = "1.0";
			
			{
				int count = 0;
				headerMap["stack"] = d->saveLayerChildrenRecursive(d->document->layerScene()->rootLayer(), count);
			}
			
			{
				QByteArray headerJson = Json::write(headerMap);
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

struct DocumentReader::Data
{
	unzFile unzip = 0;
	
	void loadLayerChildrenRecursive(const LayerRef &parent, const QVariantList &propertyMaps)
	{
		for (const QVariant &item : propertyMaps)
		{
			QVariantMap map = item.toMap();
			PAINTFIELD_DEBUG << map;
			
			auto layer = layerFactoryManager()->createLayer(map["type"].toString());
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

DocumentReader::DocumentReader(QObject *parent) :
	QObject(parent),
	d(new Data)
{}

DocumentReader::~DocumentReader()
{
	delete d;
}

Document *DocumentReader::load(const QString &filePath, QObject *parent)
{
	try
	{
		d->unzip = unzOpen2_64(filePath.toLocal8Bit(), &MinizipSupport::fileFuncDefs);
		
		QVariantMap headerMap;
		
		{
			auto data = loadFromUnzip(d->unzip, "header.json");
			headerMap = Json::read(data).toMap();
		}
		
		if (headerMap["version"].toString() != "1.0")
			throw runtime_error("incompatible file version");
		
		QSize size;
		size.rwidth() = headerMap["width"].toInt();
		size.rheight() = headerMap["height"].toInt();
		
		if (size.isEmpty())
			throw runtime_error("invalid size");
		
		QVariantList stack = headerMap["stack"].toList();
		
		auto group = std::make_shared<GroupLayer>();
		d->loadLayerChildrenRecursive(group, stack);
		
		unzClose(d->unzip);
		d->unzip = 0;
		
		auto document = new Document(QString(), size, group->takeAll(), parent);
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
