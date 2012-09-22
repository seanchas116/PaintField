#include <QtCore>

#include "QJson/Parser"
#include "QJson/Serializer"

#include "randomstring.h"

#include "defaultdocumentio.h"

namespace PaintField
{



DefaultDocumentSaver::DefaultDocumentSaver(const QString &path, const QString &lastSavedPath, QObject *parent)
{
	
}

bool DefaultDocumentSaver::save(Document *document)
{
	
}

DefaultDocumentIO::DefaultDocumentIO(QObject *parent) :
    QObject(parent)
{
}

Document *DefaultDocumentIO::open(const QString &filePath, QObject *parent)
{
	if (filePath.isEmpty())
	{
		qWarning() << Q_FUNC_INFO << ": file path is invalid";
		return 0;
	}
	
	ZipArchive archive;
	if (!archive.open(filePath))
	{
		qWarning() << Q_FUNC_INFO << ": unable to open file";
		return 0;
	}
	
	QByteArray headerJson = archive.readFile("header.json");
	
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
	
	if (!loadLayerRecursive(&group, &archive, stack))
	{
		qWarning() << Q_FUNC_INFO << ": cannot compose layer stack";
		return 0;
	}
	
	Document *document = new Document(QString(), size, group.takeChildren(), parent);
	document->setFilePath(filePath);
	return document;
}

bool DefaultDocumentIO::saveAs(Document *document, const QString &filePath)
{
	if (filePath.isEmpty())
	{
		qWarning() << Q_FUNC_INFO << ": filePath is empty";
		return false;
	}
	
	QString tempFilePath = createTemporaryFilePath();
	ZipArchive archive;
	if (!archive.open(tempFilePath, ZIP_CREATE))
	{
		qWarning() << __PRETTY_FUNCTION__ << ": unable to create temporary file";
		return false;
	}
	
	int fileCount = 0;
	
	QVariantMap documentData;
	documentData["width"] = document->width();
	documentData["height"] = document->height();
	documentData["version"] = "1.0";
	
	QVariantList stack;
	
	if (!saveLayerRecursive(document->layerModel()->rootLayer(), &archive, &fileCount, &stack))
	{
		qWarning() << Q_FUNC_INFO << ": unable to save file";
		return false;
	}
	
	QJson::Serializer serializer;
	QByteArray headerJson = serializer.serialize(documentData);
	
	if (!archive.writeFile("header.json", headerJson))
	{
		qWarning() << Q_FUNC_INFO << ": unable to save file";
		return false;
	}
	archive.close();
	
	if (QFile::exists(filePath))
	{
		QFile::remove(filePath);
	}
	
	if (!QFile::copy(tempFilePath, filePath) ) {
		qWarning() << __PRETTY_FUNCTION__ << "unable to copy file to the specified place";
		return false;
	}
	
	// succeeded to save file
	document->setFilePath(filePath);
	document->setModified(false);
	return true;
}

bool DefaultDocumentIO::saveLayerRecursive(const Layer *parent, ZipArchive *archive, int *fileCount, QVariantList *result)
{
	Q_ASSERT(parent);
	Q_ASSERT(archive);
	Q_ASSERT(fileCount);
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
			QString filePath = "data/" + QString::number(*fileCount) + ".surface";
			
			QByteArray byteArray;
			QBuffer buffer(&byteArray);
			
			buffer.open(QIODevice::WriteOnly);
			layer->surface().save(&buffer);
			buffer.close();
			
			archive->writeFile(filePath, byteArray);
			
			*fileCount += 1;
			
			layerData["source"] = filePath;
		}
		
		layerDataList << layerData;
	}
	
	*result = layerDataList;
	return true;
}

bool DefaultDocumentIO::loadLayerRecursive(Layer *parent, ZipArchive *archive, const QVariantList &layerDataList)
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

}
