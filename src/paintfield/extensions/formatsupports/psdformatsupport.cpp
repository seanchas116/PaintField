#include <array>
#include <stdexcept>
#include <type_traits>
#include <memory>
#include <QStack>

#include "psdformatsupport.h"

namespace PaintField {

class BinaryStream
{
public:

	BinaryStream(QIODevice *device) :
		m_dataStream(device)
	{
	}

	QDataStream &dataStream() { return m_dataStream; }

	QByteArray read(qint64 size)
	{
		auto data = m_dataStream.device()->read(size);
		if (data.size() != size)
			throw std::runtime_error("cannot read data anymore");
		return data;
	}

	void write(const QByteArray &data)
	{
		auto size = m_dataStream.device()->write(data);
		if (size != data.size())
			throw std::runtime_error("cannot write data");
	}

	QByteArray peek(qint64 size)
	{
		auto data = read(size);
		move(-size);
		return data;
	}

	qint64 pos() const
	{
		return m_dataStream.device()->pos();
	}

	void seek(qint64 pos)
	{
		m_dataStream.device()->seek(pos);
	}

	void move(qint64 offset)
	{
		seek(pos() + offset);
	}

	void pushPos()
	{
		m_posStack.push(pos());
	}

	void popPos()
	{
		if (m_posStack.size() == 0)
			throw std::runtime_error("cannot pop position stack");
		seek(m_posStack.pop());
	}

private:

	QDataStream m_dataStream;
	QStack<qint64> m_posStack;
};

template <typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value>::type>
static BinaryStream &operator>>(BinaryStream &stream, T &value)
{
	stream.dataStream() >> value;
	return stream;
}

template <typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value>::type>
static BinaryStream &operator<<(BinaryStream &stream, T value)
{
	stream.dataStream() << value;
	return stream;
}

static QString readPascalString(BinaryStream &stream, int unit)
{
	uint8_t count;
	stream >> count;

	int totalCount = count + 1;
	int alignedCount = ((totalCount - 1) / unit + 1) * unit;
	int skipCount = alignedCount - count - 1;

	auto data = stream.read(count);
	stream.move(skipCount);
	return QString::fromUtf8(data.data(), data.size());
}

static QString readUnicodePascalString(BinaryStream &stream)
{
	uint32_t count;
	stream >> count;

	QVector<uint16_t> utf16Vector;
	utf16Vector.reserve(count);
	for (uint32_t i = 0; i < count; ++i)
	{
		uint16_t c;
		stream >> c;
		utf16Vector << c;
	}
	return QString::fromUtf16(utf16Vector.data(), utf16Vector.size());
}

static QByteArray decodePackBits(const QByteArray &src, int dstSize)
{
	QByteArray dst;
	dst.reserve(dstSize);

	int index = 0;
	while (index < src.size())
	{
		char count = src[index];
		index += 1;
		if (count >= 0)
		{
			int len = count + 1;
			dst += src.mid(index, len);
			index += len;
		}
		else
		{
			char c = src[index];
			int len = -count + 1;
			for (int i = 0; i < len; ++i)
				dst += c;
			index += 1;
		}
	}

	return dst;
}

struct PsdFileHeaderSection
{
	QByteArray signature = "8BPS";
	int16_t version = 1;
	int16_t channelCount;
	int32_t height;
	int32_t width;
	int16_t depth;
	int16_t colorMode;
	
	void load(BinaryStream &stream)
	{
		signature = stream.read(4);
		stream >> version;
		stream.move(6);
		stream >> channelCount;
		stream >> height;
		stream >> width;
		stream >> depth;
		stream >> colorMode;
		
		PAINTFIELD_DEBUG << "signature" << QString(signature);
		PAINTFIELD_DEBUG << "channel count" << channelCount;
		PAINTFIELD_DEBUG << "height" << height;
		PAINTFIELD_DEBUG << "width" << width;
		PAINTFIELD_DEBUG << "depth" << depth;
		PAINTFIELD_DEBUG << "color mode" << colorMode;
	}
};

struct PsdColorModeDataSection
{
	void load(BinaryStream &stream)
	{
		uint32_t length;
		stream >> length;
		stream.move(length);

		PAINTFIELD_DEBUG << "length" << length;
	}
};


struct PsdImageResourceSection
{
	void load(BinaryStream &stream)
	{
		uint32_t length;
		stream >> length;
		stream.move(length);

		PAINTFIELD_DEBUG << "length" << length;
	}
};

struct PsdChannelInfo
{
	uint16_t id;
	uint32_t length;

	void load(BinaryStream &stream)
	{
		stream >> id;
		stream >> length;
	}
};

struct PsdLayerMaskData
{
	void load(BinaryStream &stream)
	{
		uint32_t length;
		stream >> length;
		stream.move(length);
	}
};

struct PsdLayerBlendingRangesData
{
	void load(BinaryStream &stream)
	{
		uint32_t length;
		stream >> length;
		stream.move(length);
	}
};

struct PsdLayerRecord
{
	int32_t rectTop;
	int32_t rectLeft;
	int32_t rectBottom;
	int32_t rectRight;

	uint16_t channelCount;
	QVector<PsdChannelInfo> channelInfos;

	QByteArray blendModeKey;

	uint8_t opacity;
	uint8_t clipping;
	uint8_t flags;

	QString layerName;

	uint32_t sectionType = 0;
	QByteArray sectionBlendModeKey;

	void load(BinaryStream &stream)
	{
		PAINTFIELD_DEBUG;
		PAINTFIELD_DEBUG << "READING LAYER RECORD...";

		stream >> rectTop >> rectLeft >> rectBottom >> rectRight;
		PAINTFIELD_DEBUG << "rect" << rectTop << rectLeft << rectBottom << rectRight;

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

			readPascalString(stream, 4);

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
					layerName = readUnicodePascalString(stream);
					PAINTFIELD_DEBUG << "name" << layerName;
				}

				stream.popPos();
				stream.move(length);
			}
		}

		stream.popPos();
		stream.move(extraDataFieldLength);
	}
};

struct PsdChannelData
{
	uint16_t compression = 1;
	QByteArray rawData;

	void load(BinaryStream &stream, const QRect &rect, int depth, int dataSize)
	{
		stream.pushPos();

		int byteDepth = 1;

		switch (depth)
		{
			case 8:
				byteDepth = 1;
				break;
			case 16:
				byteDepth = 2;
				break;
			case 32:
				byteDepth = 4;
				break;
			default:
				throw std::runtime_error("unsupported depth");
		}

		int width = rect.width();
		int height = rect.height();
		int byteCount = width * height * byteDepth;

		stream >> compression;

		switch (compression)
		{
			case 0: // no compression
			{
				PAINTFIELD_DEBUG << "no compression";
				rawData = stream.read(byteCount);
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
					rawData += decodePackBits(srcData, dstScanlineLength);
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
};

struct PsdLayerInfo
{
	QList<Ref<PsdLayerRecord>> layerRecords;
	int16_t signedLayerCount;
	QList<Ref<PsdChannelData>> channelDataList;

	void load(BinaryStream &stream, int depth)
	{
		uint32_t length;
		stream >> length;
		stream >> signedLayerCount;

		PAINTFIELD_DEBUG << signedLayerCount;

		int layerCount = abs(signedLayerCount);

		layerRecords.reserve(layerCount);

		for (int i = 0; i < layerCount; ++i)
		{
			auto record = std::make_shared<PsdLayerRecord>();
			record->load(stream);
			layerRecords << record;
		}

		for (int i = 0; i < layerCount; ++i)
		{
			const auto &layerRecord = layerRecords[i];
			auto channelCount = layerRecord->channelCount;
			QRect rect(layerRecord->rectLeft, layerRecord->rectTop, layerRecord->rectRight - layerRecord->rectLeft, layerRecord->rectBottom - layerRecord->rectTop);

			for (int channel = 0; channel < channelCount; ++channel)
			{
				auto channelData = std::make_shared<PsdChannelData>();
				auto size = layerRecords[i]->channelInfos[channel].length;
				channelData->load(stream, rect, depth, size);
				channelDataList << channelData;
			}
		}
	}
};

struct PsdLayerAndMaskInformationSection
{
	PsdLayerInfo layerInfo;

	void load(BinaryStream &stream, int depth)
	{
		uint32_t length;
		stream >> length;
		PAINTFIELD_DEBUG << "length" << length;

		layerInfo.load(stream, depth);
	}
};

PsdFormatSupport::PsdFormatSupport(QObject *parent) :
	FormatSupport(parent)
{
	setShortDescription("Photoshop PSD");
}

bool PsdFormatSupport::read(QIODevice *device, QList<LayerRef> *layers, QSize *size)
{
	try
	{
		BinaryStream stream(device);

		PsdFileHeaderSection header;
		header.load(stream);

		PsdColorModeDataSection colorData;
		colorData.load(stream);

		PsdImageResourceSection resource;
		resource.load(stream);

		PsdLayerAndMaskInformationSection layerSection;
		layerSection.load(stream, header.depth);
	}
	catch (const std::runtime_error &error)
	{
		PAINTFIELD_WARNING << error.what();
		return false;
	}
	
	return false;
}

bool PsdFormatSupport::write(QIODevice *device, const QList<LayerConstRef> &layers, const QSize &size, const QVariant &option)
{
	return false;
}

} // namespace PaintField
