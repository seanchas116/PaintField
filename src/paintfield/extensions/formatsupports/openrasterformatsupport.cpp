#include <stdexcept>
#include <QDomDocument>
#include <QBuffer>
#include <Malachite/ImageIO>
#include <Malachite/SurfacePainter>
#include "paintfield/core/rasterlayer.h"
#include "paintfield/core/grouplayer.h"
#include "paintfield/core/zip.h"
#include "paintfield/core/layerrenderer.h"

#include "openrasterformatsupport.h"

namespace PaintField {

OpenRasterFormatSupport::OpenRasterFormatSupport(QObject *parent) :
	FormatSupport(parent)
{
	setShortDescription(tr("OpenRaster"));
}

static QHash<QString, Malachite::BlendMode> createBlendModeFromStringHash()
{
	using namespace Malachite;
	QHash<QString, BlendMode> hash;
	hash["svg:src-over"] = BlendMode::Normal;
	hash["svg:plus"] = BlendMode::Plus;
	hash["svg:multiply"] = BlendMode::Multiply;
	hash["svg:screen"] = BlendMode::Screen;
	hash["svg:overlay"] = BlendMode::Overlay;
	hash["svg:darken"] = BlendMode::Darken;
	hash["svg:lighten"] = BlendMode::Lighten;
	hash["svg:color-dodge"] = BlendMode::ColorDodge;
	hash["svg:color-burn"] = BlendMode::ColorBurn;
	hash["svg:hard-light"] = BlendMode::HardLight;
	hash["svg:soft-light"] = BlendMode::SoftLight;
	hash["svg:difference"] = BlendMode::Difference;
	hash["svg:color"] = BlendMode::Color;
	hash["svg:luminosity"] = BlendMode::Luminosity;
	hash["svg:hue"] = BlendMode::Hue;
	hash["svg:saturation"] = BlendMode::Saturation;
	return hash;
}

static QHash<Malachite::BlendMode, QString> createBlendModeToStringHash()
{
	auto fromString = createBlendModeFromStringHash();
	QHash<Malachite::BlendMode, QString> toString;
	
	for (auto i = fromString.begin(); i != fromString.end(); ++i)
		toString[i.value()] = i.key();
	
	return toString;
}

static auto blendModeFromStringHash = createBlendModeFromStringHash();
static auto blendModeToStringHash = createBlendModeToStringHash();

static void readLayers(UnzipArchive *archive, QList<LayerRef> &layers, const QDomElement &stackElement)
{
	for (auto layerElement = stackElement.firstChildElement(); !layerElement.isNull(); layerElement = layerElement.nextSiblingElement())
	{
		auto tagName = layerElement.tagName();
		
		bool isLayer = (tagName == "layer");
		bool isStack = (tagName == "stack");
		
		if (!isLayer && !isStack)
			continue;
		
		LayerRef layer;
		if (isLayer)
			layer = makeSP<RasterLayer>();
		else
			layer = makeSP<GroupLayer>();
		
		// set layer properties
		
		layer->setName(layerElement.attribute("name"));
		
		if (layerElement.hasAttribute("composite-op"))
		{
			auto blendModeString = layerElement.attribute("composite-op");
			layer->setBlendMode(blendModeFromStringHash.value(blendModeString, Malachite::BlendMode::Normal));
		}
		
		if (layerElement.hasAttribute("opacity"))
		{
			auto opacityString = layerElement.attribute("opacity");
			layer->setOpacity(qBound(0.0, opacityString.toDouble(), 1.0));
		}
		
		if (layerElement.hasAttribute("visibility"))
		{
			if (layerElement.attribute("visibility") == "hidden")
				layer->setVisible(false);
		}
		
		auto x = layerElement.attribute("x").toInt();
		auto y = layerElement.attribute("y").toInt();
		auto src = layerElement.attribute("src");
		
		if (isLayer)
		{
			auto rasterLayer = staticSPCast<RasterLayer>(layer);
			
			UnzipFile srcFile(archive, src);
			
			if (srcFile.open())
			{
				auto data = srcFile.readAll();
				QBuffer buffer(&data);
				buffer.open(QIODevice::ReadOnly);
				
				Malachite::ImageReader importer;
				importer.read(&buffer);
				auto surface = importer.toSurface(QPoint(x, y));
				rasterLayer->setSurface(surface);
			}
		}
		
		if (isStack)
		{
			QList<LayerRef> children;
			readLayers(archive, children, layerElement);
			layer->append(children);
		}
		
		layers << layer;
	}
}

bool OpenRasterFormatSupport::read(QIODevice *device, QList<LayerRef> *pLayers, QSize *pSize)
{
	try
	{
		UnzipArchive archive(device);
		
		// open archive
		if (!archive.open())
			throw std::runtime_error("cannot open zip");
		
		// get stack.xml
		
		QDomDocument document;
		
		{
			UnzipFile file(&archive, "mimetype");
			if (!file.open())
				throw std::runtime_error("cannot open mimetype");
			auto data = file.readAll();
			if (data != "image/openraster")
				throw std::runtime_error("wrong mimetype");
		}
		
		{
			UnzipFile file(&archive, "stack.xml");
			if (!file.open())
				throw std::runtime_error("cannot open stack.xml");
			
			document.setContent(&file);
		}
		
		auto imageElement = document.documentElement();
		if (imageElement.tagName() != "image")
			throw std::runtime_error("no image element");
		
		auto w = imageElement.attribute("w").toInt();
		auto h = imageElement.attribute("h").toInt();
		QSize size(w, h);
		
		if (size.isEmpty())
			throw std::runtime_error("invalid size");
		
		auto stackElement = imageElement.firstChildElement("stack");
		if (stackElement.isNull())
			throw std::runtime_error("no stack element");
		
		readLayers(&archive, *pLayers, stackElement);
		*pSize = size;
	}
	catch (const std::runtime_error &error)
	{
		PAINTFIELD_WARNING << error.what();
		return false;
	}
	return true;
}

static QList<QDomElement> saveLayers(ZipArchive *archive, const QList<LayerConstRef> &layers, int &sourceFileCount, QDomDocument &document)
{
	QList<QDomElement> layerElements;
	
	for (const auto &layer : layers)
	{
		bool isGroup = layer->isType<GroupLayer>();
		QString tagName = isGroup ? "stack" : "layer";
		
		auto layerElement = document.createElement(tagName);
		
		layerElement.setAttribute("name", layer->name());
		layerElement.setAttribute("opacity", layer->opacity());
		
		if (layer->blendMode() != Malachite::BlendMode::PassThrough)
			layerElement.setAttribute("composite-op", blendModeToStringHash[layer->blendMode()]);
		
		layerElement.setAttribute("visibility", layer->isVisible() ? "visible" : "hidden");
		
		if (isGroup)
		{
			// save children recursive
			
			layerElement.setAttribute("x", 0);
			layerElement.setAttribute("y", 0);
			
			auto childElements = saveLayers(archive, layer->children(), sourceFileCount, document);
			for (auto child : childElements)
				layerElement.appendChild(child);
		}
		else
		{
			// save surface into png file
			
			Malachite::Surface surface;
			
			if (layer->isType<RasterLayer>())
			{
				surface = staticSPCast<const RasterLayer>(layer)->surface();
			}
			else
			{
				Malachite::Painter painter(&surface);
				layer->render(&painter);
			}
			
			auto rect = surface.boundingRect();
			layerElement.setAttribute("x", rect.left());
			layerElement.setAttribute("y", rect.top());
			
			QByteArray data;
			
			{
				QBuffer buffer(&data);
				buffer.open(QIODevice::WriteOnly);
				
				Malachite::ImageWriter exporter("png");
				exporter.setSurface(surface, rect);
				exporter.write(&buffer);
			}
			
			{
				QString path = "data/" + QString::number(sourceFileCount) + ".png";
				
				ZipFile file(archive, path);
				if (!file.open())
					throw std::runtime_error("cannot open file in zip");
				
				file.write(data);
				
				layerElement.setAttribute("src", path);
				sourceFileCount++;
			}
		}
		
		layerElements << layerElement;
	}
	
	return layerElements;
}

bool OpenRasterFormatSupport::write(QIODevice *device, const QList<LayerConstRef> &layers, const QSize &size, const QVariant &option)
{
	Q_UNUSED(option);
	
	try
	{
		ZipArchive archive(device);
		
		if (!archive.open())
			throw std::runtime_error("cannot open archive");
		
		// mimetype
		{
			ZipFile file(&archive, "mimetype");
			file.setMethod(ZipFile::MethodStored);
			if (!file.open())
				throw std::runtime_error("cannot open mimetype");
			file.write("image/openraster");
		}
		
		// stack.xml and data files
		{
			QDomDocument stackDocument;
			auto header = stackDocument.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"utf-8\"");
			stackDocument.appendChild(header);
			
			auto imageElement = stackDocument.createElement("image");
			imageElement.setAttribute("w", size.width());
			imageElement.setAttribute("h", size.height());
			
			stackDocument.appendChild(imageElement);
			
			auto stackElement = stackDocument.createElement("stack");
			
			int sourceFileCount = 0;
			auto layerElements = saveLayers(&archive, layers, sourceFileCount, stackDocument);
			for (const auto &layerElement : layerElements)
				stackElement.appendChild(layerElement);
			
			imageElement.appendChild(stackElement);
			
			{
				ZipFile file(&archive, "stack.xml");
				if (!file.open())
					throw std::runtime_error("cannot open stack.xml");
				
				file.write(stackDocument.toByteArray());
			}
		}
		
		// thumbnail and merged image
		{
			LayerRenderer renderer;
			auto surface = renderer.renderToSurface(layers);

			auto image = surface.crop(QRect(QPoint(), size));

			// merged image
			{
				QByteArray data;
				Malachite::ImageWriter writer("png");
				writer.setImage(image);

				{
					QBuffer buffer(&data);
					buffer.open(QIODevice::WriteOnly);
					writer.write(&buffer);
				}

				ZipFile file(&archive, "mergedimage.png");
				if (!file.open())
					throw std::runtime_error("cannot open mergedimage.png");

				file.write(data);
			}

			// thumbnail
			{
				auto qimage = surface.crop(QRect(QPoint(), size)).toImageU8().wrapInQImage();
				auto scaledImage = qimage.scaled(256, 256, Qt::KeepAspectRatio, Qt::SmoothTransformation);

				QByteArray data;

				{
					QBuffer buffer(&data);
					buffer.open(QIODevice::WriteOnly);
					scaledImage.save(&buffer, "PNG");
				}

				ZipFile file(&archive, "Thumbnails/thumbnail.png");
				if (!file.open())
					throw std::runtime_error("cannot open thumbnail.png");

				file.write(data);
			}
		}
	}
	catch (const std::runtime_error &error)
	{
		PAINTFIELD_WARNING << error.what();
		return false;
	}
	
	return true;
}

}
