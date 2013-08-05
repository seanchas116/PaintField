#include <stdexcept>

#include "layerfactorymanager.h"
#include "zip.h"
#include "json.h"

#include "paintfieldformatsupport.h"

namespace PaintField {

PaintFieldFormatSupport::PaintFieldFormatSupport(QObject *parent) :
	FormatSupport(parent)
{
	setShortDescription(tr("PaintField Document"));
}

QString PaintFieldFormatSupport::name() const
{
	return "paintfield.format.paintfield";
}

QStringList PaintFieldFormatSupport::suffixes() const
{
	return {"pfield"};
}

bool PaintFieldFormatSupport::canRead() const
{
	return true;
}

bool PaintFieldFormatSupport::canWrite() const
{
	return true;
}

FormatSupport::Capabilities PaintFieldFormatSupport::capabilities() const
{
	return CapabilityAll;
}

static void readLayers(UnzipArchive *archive, QList<LayerRef> &layers, const QVariantList &propertyMaps)
{
	for (const auto &item : propertyMaps)
	{
		QVariantMap map = item.toMap();
		auto layer = layerFactoryManager()->createLayer(map["type"].toString());
		if (!layer)
		{
			PAINTFIELD_WARNING << "unsupported layer type";
			continue;
		}
		
		layer->loadProperties(map);
		
		if (layer->hasDataToSave() && map.contains("source"))
		{
			QString source = map["source"].toString();
			UnzipFile file(archive, source);
			if (!file.open())
				PAINTFIELD_WARNING << "cannot read data from source;";
			
			QDataStream stream(&file);
			layer->loadDataFile(stream);
		}
		
		if (layer->canHaveChildren())
		{
			QList<LayerRef> layers;
			readLayers(archive, layers, map["children"].toList());
			layer->append(layers);
		}
		
		layers << layer;
	}
}

bool PaintFieldFormatSupport::read(QIODevice *device, QList<LayerRef> *layers, QSize *psize)
{
	try
	{
		UnzipArchive archive(device);
		if (!archive.open())
			throw std::runtime_error("cannot open zip");
		
		QVariantMap headerMap;
		{
			UnzipFile file(&archive, "header.json");
			
			if (!file.open())
				throw std::runtime_error("cannot find header.json");
			
			headerMap = Json::read(file.readAll()).toMap();
		}
		
		if (headerMap["version"].toString() != "1.0")
			throw std::runtime_error("incompatible file version");
		
		QSize size;
		size.rwidth() = headerMap["width"].toInt();
		size.rheight() = headerMap["height"].toInt();
		
		if (size.isEmpty())
			throw std::runtime_error("invalid size");
		
		readLayers(&archive, *layers, headerMap["stack"].toList());
		
		*psize = size;
		
		return true;
	}
	catch (const std::runtime_error &error)
	{
		PAINTFIELD_WARNING << error.what();
		return false;
	}
}

static QVariantList saveLayers(ZipArchive *archive, const QList<LayerConstRef> &layers, int &sourceFileCount)
{
	QVariantList maps;
	
	for (const auto &layer : layers)
	{
		auto map = layer->saveProperties();
		map["type"] = layerFactoryManager()->nameForTypeInfo(typeid(*layer));
		
		if (layer->hasDataToSave())
		{
			QString path = "data/" + QString::number(sourceFileCount) + "." + layer->dataSuffix();
			sourceFileCount++;
			ZipFile file(archive, path);
			if (!file.open())
				throw std::runtime_error("cannot add source file");
			
			QDataStream stream(&file);
			
			layer->saveDataFile(stream);
			
			map["source"] = path;
		}
		
		if (layer->count())
		{
			map["children"] = saveLayers(archive, layer->children(), sourceFileCount);
		}
		
		maps << map;
	}
	
	return maps;
}

bool PaintFieldFormatSupport::write(QIODevice *device, const QList<LayerConstRef> &layers, const QSize &size, const QVariant &option)
{
	try
	{
		ZipArchive archive(device);
		
		if (!archive.open())
			throw std::runtime_error("cannot open archive");
		
		QVariantMap headerMap;
		headerMap["width"] = size.width();
		headerMap["height"] = size.height();
		headerMap["version"] = "1.0";
		
		{
			int count = 0;
			headerMap["stack"] = saveLayers(&archive, layers, count);
		}
		
		{
			auto headerData = Json::write(headerMap);
			ZipFile file(&archive, "header.json");
			
			if (!file.open())
				throw std::runtime_error("cannot add header file");
			
			file.write(headerData);
		}
		return true;
	}
	catch (const std::runtime_error &error)
	{
		PAINTFIELD_WARNING << error.what();
		return false;
	}
}

} // namespace PaintField
