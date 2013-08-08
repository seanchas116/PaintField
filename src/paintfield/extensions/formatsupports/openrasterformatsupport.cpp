#include <stdexcept>
#include <QDomDocument>
#include <QBuffer>
#include <Malachite/ImageIO>
#include "paintfield/core/rasterlayer.h"
#include "paintfield/core/zip.h"

#include "openrasterformatsupport.h"

namespace PaintField {

OpenRasterFormatSupport::OpenRasterFormatSupport(QObject *parent) :
	FormatSupport(parent)
{
	setShortDescription(tr("OpenRaster"));
}

static QHash<QString, Malachite::BlendMode> createBlendModeHash()
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

static auto blendModeHash = createBlendModeHash();

static void readLayers(UnzipArchive *archive, QList<LayerRef> &layers, const QDomElement &stackElement)
{
	for (auto layerElement = stackElement.firstChildElement(); !layerElement.isNull(); layerElement = layerElement.nextSiblingElement())
	{
		auto tagName = layerElement.tagName();
		
		if (tagName != "layer" && tagName != "stack")
			continue;
		
		auto layer = std::make_shared<RasterLayer>();
		
		// set layer properties
		
		layer->setName(layerElement.attribute("name"));
		
		if (layerElement.hasAttribute("composite-op"))
		{
			auto blendModeString = layerElement.attribute("composite-op");
			layer->setBlendMode(blendModeHash.value(blendModeString, Malachite::BlendMode::Normal));
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
		
		UnzipFile srcFile(archive, src);
		
		if (srcFile.open())
		{
			auto data = srcFile.readAll();
			QBuffer buffer(&data);
			buffer.open(QIODevice::ReadOnly);
			
			Malachite::ImageImporter importer;
			importer.load(&buffer);
			auto surface = importer.toSurface(QPoint(x, y));
			layer->setSurface(surface);
		}
		
		if (tagName == "stack")
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

bool OpenRasterFormatSupport::write(QIODevice *device, const QList<LayerConstRef> &layers, const QSize &size, const QVariant &option)
{
	return false;
}

}
