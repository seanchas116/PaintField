#include "psdbinarystream.h"
#include "psdutils.h"

#include "psdlayerandmaskinformationsection.h"

namespace PaintField {

void PsdChannelData::load(PsdBinaryStream &stream, const QRect &rect, int depth, int dataSize)
{
	stream.pushPos();

	int byteDepth = depth / 8;

	int width = rect.width();
	int height = rect.height();
	int byteCount = width * height * byteDepth;

	uint16_t compression;
	stream >> compression;

	PAINTFIELD_DEBUG << "channel data size" << dataSize;

	switch (compression)
	{
		case 0: // no compression
		{
			PAINTFIELD_DEBUG << "no compression";
			rawData = stream.read(byteCount);
			PAINTFIELD_DEBUG << "actual data size" << rawData.size();
			break;
		}
		case 1: // PackBits
		{
			PAINTFIELD_DEBUG << "PackBits";

			rawData.reserve(byteCount);

			QVector<uint16_t> srcScanlineLengths;
			srcScanlineLengths.reserve(height);

			int dstScanlineLength = width * byteDepth;

			for (int y = 0; y < height; ++y)
			{
				uint16_t length;
				stream >> length;
				srcScanlineLengths << length;
			}

			for (int y = 0; y < height; ++y)
			{
				auto srcData = stream.read(srcScanlineLengths[y]);
				rawData += PsdUtils::decodePackBits(srcData, dstScanlineLength);
			}

			PAINTFIELD_DEBUG << "expected data size" << byteCount;
			PAINTFIELD_DEBUG << "actual data size" << rawData.size();

			break;
		}
		default:
			throw std::runtime_error("unsupported compression");
	}

	stream.popPos();
	stream.move(dataSize);
}

void PsdChannelData::save(PsdBinaryStream &stream) const
{
	// no compression
	stream << uint16_t(0);
	stream.write(rawData);
}

struct PsdLayerMaskData
{
	void load(PsdBinaryStream &stream)
	{
		uint32_t length;
		stream >> length;
		stream.move(length);
	}

	void save(PsdBinaryStream &stream) const
	{
		uint32_t length = 0;
		stream << length;
	}
};

struct PsdLayerBlendingRangesData
{
	void load(PsdBinaryStream &stream)
	{
		uint32_t length;
		stream >> length;
		stream.move(length);
	}

	void save(PsdBinaryStream &stream) const
	{
		uint32_t length = 0;
		stream << length;
	}
};

void PsdChannelInfo::load(PsdBinaryStream &stream)
{
	stream >> id;
	PAINTFIELD_DEBUG << "id" << id;
	stream >> length;
	PAINTFIELD_DEBUG << "length" << length;

	if (id < -3 || 3 < id)
		throw std::runtime_error("invalid channel id");
}

void PsdChannelInfo::save(PsdBinaryStream &stream) const
{
	stream << id;
	stream << length;
}

void PsdLayerRecord::load(PsdBinaryStream &stream)
{
	PAINTFIELD_DEBUG;
	PAINTFIELD_DEBUG << "READING LAYER RECORD...";

	stream >> rectTop >> rectLeft >> rectBottom >> rectRight;
	PAINTFIELD_DEBUG << "rect" << rectTop << rectLeft << rectBottom << rectRight;

	uint16_t channelCount;
	stream >> channelCount;
	PAINTFIELD_DEBUG << "channel count" << channelCount;

	channelInfos.reserve(channelCount);
	for (int i = 0; i < channelCount; ++i)
	{
		PsdChannelInfo info;
		info.load(stream);
		channelInfos << info;
	}

	auto blendModeSignature = stream.read(4);
	PAINTFIELD_DEBUG << "blend mode sig" << blendModeSignature;
	if (blendModeSignature != "8BIM")
		throw std::runtime_error("wrong blend mode signature");

	blendModeKey = stream.read(4);
	PAINTFIELD_DEBUG << "blend mode" << blendModeKey;
	stream >> opacity;
	PAINTFIELD_DEBUG << "opacity" << opacity;
	stream >> clipping;
	PAINTFIELD_DEBUG << "clipping" << clipping;
	stream >> flags;
	PAINTFIELD_DEBUG << "flags" << flags;
	stream.move(1);

	uint32_t extraDataFieldLength;
	stream >> extraDataFieldLength;
	PAINTFIELD_DEBUG << "extra data field length" << extraDataFieldLength;

	stream.pushPos();

	if (extraDataFieldLength)
	{
		PsdLayerMaskData maskData;
		maskData.load(stream);

		PsdLayerBlendingRangesData blendingRangesData;
		blendingRangesData.load(stream);

		PsdUtils::readPascalString(stream, 4);

		while (true)
		{
			auto signature = stream.peek(4);
			if (signature != "8BIM" && signature != "8B64")
				break;

			stream.move(4);
			auto key = stream.read(4);

			PAINTFIELD_DEBUG << "key" << key;

			uint32_t length;
			stream >> length;

			stream.pushPos();

			if (key == "lsct")
			{
				PAINTFIELD_DEBUG << "lsct length" << length;
				stream >> sectionType;
				PAINTFIELD_DEBUG << "section type" << sectionType;

				if (length >= 12)
				{
					stream.move(4);
					sectionBlendModeKey = stream.read(4);
					PAINTFIELD_DEBUG << "section blend mode" << sectionBlendModeKey;
				}
			}
			else if (key == "luni")
			{
				layerName = PsdUtils::readUnicodePascalString(stream);
				PAINTFIELD_DEBUG << "name" << layerName;
			}

			stream.popPos();
			stream.move(length);
		}
	}

	stream.popPos();
	stream.move(extraDataFieldLength);
}

void PsdLayerRecord::save(PsdBinaryStream &stream) const
{
	stream << rectTop << rectLeft << rectBottom << rectRight;
	stream << uint16_t(channelInfos.size());

	for (const auto &channelInfo : channelInfos)
		channelInfo.save(stream);

	stream.write("8BIM");

	stream.write(blendModeKey);
	stream << opacity;
	stream << clipping;
	stream << flags;
	stream << uint8_t(0);

	stream.markOffset<uint32_t>();

	PsdLayerMaskData maskData;
	maskData.save(stream);
	PsdLayerBlendingRangesData blendingRangesData;
	blendingRangesData.save(stream);

	PsdUtils::writePascalString(stream, 4, layerName);

	if (sectionType != (int)SectionType::Other)
	{
		stream.write("8BIM");
		stream.write("lsct");
		stream << uint32_t(4);
		stream << sectionType;
	}

	stream.write("8BIM");
	stream.write("luni");

	stream.markOffset<uint32_t>();
	PsdUtils::writeUnicodePascalString(stream, layerName);
	stream.writeOffset<uint32_t>();

	stream.writeOffset<uint32_t>();
}

void PsdLayerInfo::load(PsdBinaryStream &stream, int depth)
{
	uint32_t length;
	stream >> length;
	PAINTFIELD_DEBUG << length;

	int16_t signedLayerCount;
	stream >> signedLayerCount;
	PAINTFIELD_DEBUG << signedLayerCount;

	int layerCount = abs(signedLayerCount);

	layerRecords.reserve(layerCount);

	// read layer records
	for (int i = 0; i < layerCount; ++i)
	{
		auto record = makeSP<PsdLayerRecord>();
		record->load(stream);
		layerRecords << record;
	}

	// read channel data
	for (const auto &layerRecord : layerRecords)
	{
		auto rect = layerRecord->getRect();

		for (const auto &channelInfo : layerRecord->channelInfos)
		{
			auto channelData = makeSP<PsdChannelData>();
			channelData->load(stream, rect, depth, channelInfo.length);

			layerRecord->channelDatas << channelData;
		}
	}
}

void PsdLayerInfo::save(PsdBinaryStream &stream) const
{
	stream.markOffset<uint32_t>();
	stream << int16_t(-layerRecords.size());

	for (const auto &layerRecord : layerRecords)
	{
		layerRecord->save(stream);
	}

	for (const auto &layerRecord : layerRecords)
	{
		Q_ASSERT(layerRecord->channelDatas.size() == layerRecord->channelInfos.size());
		for (const auto &channelData : layerRecord->channelDatas)
		{
			channelData->save(stream);
		}
	}

	stream.writeOffset<uint32_t>();
}

struct PsdGlobalLayerMaskInfo
{
	void load(PsdBinaryStream &stream)
	{
		uint32_t length;
		stream >> length;
		PAINTFIELD_DEBUG << "length" << length;
	}

	void save(PsdBinaryStream &stream)
	{
		stream << uint32_t(0);
	}
};


void PsdLayerAndMaskInformationSection::load(PsdBinaryStream &stream, int depth)
{
	uint32_t length;
	stream >> length;
	PAINTFIELD_DEBUG << "length" << length;

	layerInfo.load(stream, depth);

	PsdGlobalLayerMaskInfo globalLayerMaskInfo;
	globalLayerMaskInfo.load(stream);
}

void PsdLayerAndMaskInformationSection::save(PsdBinaryStream &stream) const
{
	stream.markOffset<uint32_t>();
	layerInfo.save(stream);

	PsdGlobalLayerMaskInfo globalLayerMaskInfo;
	globalLayerMaskInfo.save(stream);
	stream.writeOffset<uint32_t>();
}

} // namespace PaintField
