#include <array>
#include <stdexcept>
#include <type_traits>
#include <memory>
#include <QStack>
#include <QHash>
#include <boost/range/adaptor/reversed.hpp>
#include "paintfield/core/rasterlayer.h"
#include "paintfield/core/grouplayer.h"

#include "psdformatsupport.h"

namespace PaintField {

class BinaryStream
{
public:

	BinaryStream(QIODevice *device) :
		m_dataStream(device)
	{
	}

	BinaryStream(QByteArray *byteArray, QIODevice::OpenMode mode) :
		m_dataStream(byteArray, mode)
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

struct PsdChannelData
{
	uint16_t compression = 1;
	QByteArray rawData;

	void load(BinaryStream &stream, const QRect &rect, int depth, int dataSize)
	{
		stream.pushPos();

		int byteDepth = depth / 8;

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


struct PsdChannelInfo
{
	int16_t id;
	uint32_t length;

	void load(BinaryStream &stream)
	{
		stream >> id;
		PAINTFIELD_DEBUG << "id" << id;
		stream >> length;
		PAINTFIELD_DEBUG << "length" << length;
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

	QVector<Ref<PsdChannelData>> channelDatas;

	enum struct Flag
	{
		TransparencyProtected = 1 << 0,
		Visible = 1 << 1
	};

	enum struct SectionType
	{
		Other = 0,
		OpenFolder = 1,
		ClosedFolder = 2,
		BoundingSectionDivider = 3
	};

	QRect getRect() const
	{
		return QRect(rectLeft, rectTop, rectRight - rectLeft, rectBottom - rectTop);
	}

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
			PAINTFIELD_DEBUG << "channel id" << info.id << "length" << info.length;
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

struct PsdLayerInfo
{
	QList<Ref<PsdLayerRecord>> layerRecords;
	int16_t signedLayerCount;

	void load(BinaryStream &stream, int depth)
	{
		uint32_t length;
		stream >> length;
		stream >> signedLayerCount;

		PAINTFIELD_DEBUG << signedLayerCount;

		int layerCount = abs(signedLayerCount);

		layerRecords.reserve(layerCount);

		// read layer records
		for (int i = 0; i < layerCount; ++i)
		{
			auto record = std::make_shared<PsdLayerRecord>();
			record->load(stream);
			layerRecords << record;
		}

		// read channel data
		for (const auto &layerRecord : layerRecords)
		{
			auto rect = layerRecord->getRect();

			for (const auto &channelInfo : layerRecord->channelInfos)
			{
				auto channelData = std::make_shared<PsdChannelData>();
				channelData->load(stream, rect, depth, channelInfo.length);

				layerRecord->channelDatas << channelData;
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


static QHash<QByteArray, Malachite::BlendMode> makeHashToBlendMode()
{
	QHash<QByteArray, Malachite::BlendMode> hash;
	hash["pass"] = Malachite::BlendMode::PassThrough;
	hash["norm"] = Malachite::BlendMode::Normal;
	hash["dark"] = Malachite::BlendMode::Darken;
	hash["mul "] = Malachite::BlendMode::Multiply;
	hash["idiv"] = Malachite::BlendMode::ColorBurn;
	hash["lite"] = Malachite::BlendMode::Lighten;
	hash["scrn"] = Malachite::BlendMode::Screen;
	hash["div "] = Malachite::BlendMode::ColorDodge;
	hash["over"] = Malachite::BlendMode::Overlay;
	hash["sLit"] = Malachite::BlendMode::SoftLight;
	hash["hLit"] = Malachite::BlendMode::HardLight;
	hash["diff"] = Malachite::BlendMode::Difference;
	hash["smud"] = Malachite::BlendMode::Exclusion;
	hash["hue "] = Malachite::BlendMode::Hue;
	hash["sat "] = Malachite::BlendMode::Saturation;
	hash["lum "] = Malachite::BlendMode::Luminosity;
	return hash;
}

static auto hashToBlendMode = makeHashToBlendMode();

class PsdSurfaceLoader
{
public:

	PsdSurfaceLoader(const QVector<Ref<PsdChannelData>> &channeldDataList, const QVector<PsdChannelInfo> &channelInfos, const QRect &rect, int bpp) :
		channeldDataList(channeldDataList),
		channelInfos(channelInfos),
		rect(rect),
		bpp(bpp)
	{}

	Malachite::Surface load() const
	{
		Q_ASSERT(channeldDataList.size() == channelInfos.size());

		int count = rect.width() * rect.height();
		Malachite::Image image(rect.size());

		auto channelDataI = channeldDataList.begin();

		for (const PsdChannelInfo &info : channelInfos)
		{
			PAINTFIELD_DEBUG << "image size" << image.area() << "data size" << (*channelDataI)->rawData.size();
			loadDataForBppAndChannel(image, (*channelDataI)->rawData, bpp, info.id);
			++channelDataI;
		}

		// premultiply pixels
		{
			auto p = image.bits();
			for (int i = 0; i < count; ++i)
			{
				auto a = p->a();
				Malachite::Pixel multiplier(1, a, a, a);
				p->rv() *= multiplier.v();
				++p;
			}
		}

		Malachite::Surface surface;
		surface.paste(image, rect.topLeft());
		return surface;
	}

private:

	template <int bpp>
	static float loadComponent(const uint8_t *data);

	template <int channel>
	static void setComponent(Malachite::Pixel *pixel, float value);

	template <int bpp, int channel>
	static void loadData(Malachite::Image &image, const QByteArray &data)
	{
		auto pixelP = image.bits();
		auto dataP = reinterpret_cast<const uint8_t *>(data.data());
		int count = image.area();

		if (count != data.size())
			throw std::runtime_error("data size wrong");

		while (count--)
		{
			auto value = loadComponent<bpp>(dataP);
			setComponent<channel>(pixelP, value);
			pixelP++;
			dataP += bpp / 8;
		}
	}

	template <int bpp>
	static void loadDataForChannel(Malachite::Image &image, const QByteArray &data, int psdChannel)
	{
		switch (psdChannel)
		{
			case -1:
				return loadData<bpp, Malachite::Pixel::Index::A>(image, data);
			case 0:
				return loadData<bpp, Malachite::Pixel::Index::R>(image, data);
			case 1:
				return loadData<bpp, Malachite::Pixel::Index::G>(image, data);
			case 2:
				return loadData<bpp, Malachite::Pixel::Index::B>(image, data);
			default:
				throw std::runtime_error("unsupported channel");
		}
	}

	static void loadDataForBppAndChannel(Malachite::Image &image, const QByteArray &data, int bpp, int psdChannel)
	{
		switch (bpp)
		{
			case 8:
				return loadDataForChannel<8>(image, data, psdChannel);
			case 16:
				return loadDataForChannel<16>(image, data, psdChannel);
			case 32:
				return loadDataForChannel<32>(image, data, psdChannel);
			default:
				throw std::runtime_error("unsupported bpp");
		}
	}

	QVector<Ref<PsdChannelData>> channeldDataList;
	QVector<PsdChannelInfo> channelInfos;
	QRect rect;
	int bpp;
};

template <> float PsdSurfaceLoader::loadComponent<8>(const uint8_t *data)
{
	return data[0] / float(0xFFU);
}

template <> float PsdSurfaceLoader::loadComponent<16>(const uint8_t *data)
{
	return (data[0] << 8U | data[1]) / float(0xFFFFU);
}

template <> float PsdSurfaceLoader::loadComponent<32>(const uint8_t *data)
{
	return (data[0] << 24U | data[1] << 16U | data[2] << 8U | data[3]) / float(0xFFFFFFFFU);
}

template <> void PsdSurfaceLoader::setComponent<Malachite::Pixel::Index::A>(Malachite::Pixel *pixel, float value)
{
	pixel->setA(value);
}

template <> void PsdSurfaceLoader::setComponent<Malachite::Pixel::Index::R>(Malachite::Pixel *pixel, float value)
{
	pixel->setR(value);
}

template <> void PsdSurfaceLoader::setComponent<Malachite::Pixel::Index::G>(Malachite::Pixel *pixel, float value)
{
	pixel->setG(value);
}

template <> void PsdSurfaceLoader::setComponent<Malachite::Pixel::Index::B>(Malachite::Pixel *pixel, float value)
{
	pixel->setB(value);
}

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

			layer->setBlendMode(hashToBlendMode.value(layerRecord->blendModeKey, Malachite::BlendMode::Normal));

			if (!isGroup)
			{
				auto rasterLayer = std::static_pointer_cast<RasterLayer>(layer);

				PsdSurfaceLoader surfaceLoader(layerRecord->channelDatas, layerRecord->channelInfos , layerRecord->getRect(), header.depth);
				rasterLayer->setSurface(surfaceLoader.load());
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
						throw std::runtime_error("too many close folder");
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

bool PsdFormatSupport::write(QIODevice *device, const QList<LayerConstRef> &layers, const QSize &size, const QVariant &option)
{
	return false;
}

} // namespace PaintField
