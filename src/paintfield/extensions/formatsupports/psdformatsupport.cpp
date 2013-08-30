#include <array>
#include <stdexcept>
#include <type_traits>
#include <memory>
#include <QStack>
#include <QHash>
#include <boost/range/adaptor/reversed.hpp>
#include "paintfield/core/rasterlayer.h"
#include "paintfield/core/grouplayer.h"
#include "paintfield/core/layerrenderer.h"

#include "psd/psdbinarystream.h"
#include "psd/psdfileheadersection.h"
#include "psd/psdcolormodedatasection.h"
#include "psd/psdimageresourcesection.h"
#include "psd/psdlayerandmaskinformationsection.h"
#include "psd/psdimagedatasection.h"
#include "psd/psdimageload.h"
#include "psd/psdimagesave.h"
#include "psd/psdutils.h"

#include "psdformatsupport.h"

namespace PaintField {

PsdFormatSupport::PsdFormatSupport(QObject *parent) :
	FormatSupport(parent)
{
	setShortDescription("Photoshop PSD");
}

bool PsdFormatSupport::read(QIODevice *device, QList<LayerRef> *layers, QSize *size)
{
	try
	{
		PsdBinaryStream stream(device);

		PsdFileHeaderSection header;
		header.load(stream);

		PsdColorModeDataSection colorData;
		colorData.load(stream);

		PsdImageResourceSection resource;
		resource.load(stream);

		PsdLayerAndMaskInformationSection layerSection;
		layerSection.load(stream, header.depth);

		QStack<LayerRef> parentStack;
		parentStack.push(std::make_shared<GroupLayer>());

		auto layerRecords = layerSection.layerInfo.layerRecords;

		auto makeLayer = [&](const Ref<PsdLayerRecord> &layerRecord, bool isGroup)
		{
			LayerRef layer;

			if (isGroup)
			{
				layer = std::make_shared<GroupLayer>();
			}
			else
			{
				layer = std::make_shared<RasterLayer>();
			}

			layer->setName(layerRecord->layerName);
			layer->setOpacity(layerRecord->opacity / 255.f);

			//layer->setOpacityProtected(layerRecord->flags & PsdLayerRecord::Flag::TransparencyProtected);
			layer->setVisible(!(layerRecord->flags & (int)PsdLayerRecord::Flag::Visible));

			layer->setBlendMode(PsdUtils::decodeBlendMode(layerRecord->blendModeKey));

			if (!isGroup)
			{
				auto rasterLayer = std::static_pointer_cast<RasterLayer>(layer);
				auto rect = layerRecord->getRect();
				auto image = PsdImageLoad::load(layerRecord->channelDatas, layerRecord->channelInfos , rect, header.depth);

				Malachite::Surface surface;
				surface.paste(image, rect.topLeft());

				rasterLayer->setSurface(surface);
			}

			return layer;
		};

		for (const auto &layerRecord : layerRecords | boost::adaptors::reversed)
		{
			switch (layerRecord->sectionType)
			{
				case (int)PsdLayerRecord::SectionType::Other:
				{
					auto layer = makeLayer(layerRecord, false);
					parentStack.top()->append(layer);
					break;
				}
				case (int)PsdLayerRecord::SectionType::OpenFolder:
				case (int)PsdLayerRecord::SectionType::ClosedFolder:
				{
					auto group = makeLayer(layerRecord, true);
					parentStack.push(group);
					break;
				}
				case (int)PsdLayerRecord::SectionType::BoundingSectionDivider:
				{
					if (parentStack.size() <= 1)
						throw std::runtime_error("too many end of folder");
					auto group = parentStack.pop();
					parentStack.top()->append(group);
					break;
				}
				default:
					break;
			}
		}

		*layers = parentStack.first()->takeAll();
		*size = QSize(header.width, header.height);
		return true;
	}
	catch (const std::runtime_error &error)
	{
		PAINTFIELD_WARNING << error.what();
		return false;
	}
}

static void writeLayers(QList<Ref<PsdLayerRecord>> &layerRecords, const QList<LayerConstRef> &layers, int bpp)
{
	// does not set section type
	auto writeLayer = [bpp](const LayerConstRef &layer)
	{
		auto record = std::make_shared<PsdLayerRecord>();

		if (layer->isType<RasterLayer>())
		{
			auto rasterLayer = std::dynamic_pointer_cast<const RasterLayer>(layer);
			auto surface = rasterLayer->surface();
			QRect rect = surface.boundingRect();

			PsdImageSave::save(surface.crop(rect), record->channelDatas, record->channelInfos, bpp);
			record->rectTop = rect.top();
			record->rectBottom = rect.top() + rect.height();
			record->rectLeft = rect.left();
			record->rectRight = rect.left() + rect.width();
		}
		else
		{
			PsdImageSave::saveEmpty(record->channelDatas, record->channelInfos);
		}

		record->blendModeKey = PsdUtils::encodeBlendMode(layer->blendMode());
		record->opacity = std::round(layer->opacity() * 0xFF);

		// TODO: transparency protection & clipping

		uint8_t flags = 0;
		if (!layer->isVisible())
			flags |= (int)PsdLayerRecord::Flag::Visible;
		record->flags = flags;

		record->layerName = layer->name();

		return record;
	};

	for (const auto &layer : layers)
	{
		auto record = writeLayer(layer);

		if (layer->isType<GroupLayer>())
		{
			record->sectionType = (int)PsdLayerRecord::SectionType::OpenFolder;
			layerRecords.prepend(record);

			writeLayers(layerRecords, layer->children(), bpp);

			auto endGroup = std::make_shared<PsdLayerRecord>();
			endGroup->sectionType = (int)PsdLayerRecord::SectionType::BoundingSectionDivider;
			PsdImageSave::saveEmpty(endGroup->channelDatas, endGroup->channelInfos);

			layerRecords.prepend(endGroup);
		}
		else
		{
			layerRecords.prepend(record);
		}
	}
}

bool PsdFormatSupport::write(QIODevice *device, const QList<LayerConstRef> &layers, const QSize &size, const QVariant &option)
{
	Q_UNUSED(option);

	try
	{
		PsdBinaryStream stream(device);
		
		PsdFileHeaderSection header;
		header.width = size.width();
		header.height = size.height();
		header.depth = 8;
		header.save(stream);
		
		PsdColorModeDataSection colorData;
		colorData.save(stream);
		
		PsdImageResourceSection resource;
		resource.save(stream);
		
		PsdLayerAndMaskInformationSection layerSection;
		writeLayers(layerSection.layerInfo.layerRecords, layers, header.depth);
		layerSection.save(stream);

		PsdImageDataSection imageDataSection;
		{
			LayerRenderer renderer;
			QRect rect(QPoint(), size);
			auto merged = renderer.renderToSurface(layers, Malachite::Surface::rectToKeys(rect));
			imageDataSection.data = PsdImageSave::saveAsImageData(merged.crop(rect), header.depth);
		}

		imageDataSection.save(stream);
		
		return true;
	}
	catch (const std::runtime_error &error)
	{
		PAINTFIELD_WARNING << error.what();
		return false;
	}
	return false;
}

} // namespace PaintField
