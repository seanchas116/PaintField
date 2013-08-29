#include "psdimagesave.h"

namespace PaintField {
namespace PsdImageSave {

template <int channel, int bpp>
static Ref<PsdChannelData> writeChannel(const Malachite::Image &image)
{
	int pixelCount = image.area();
	QByteArray data;
	data.reserve(pixelCount * 4 * (bpp / 8));

	auto p = image.constBits();

	for (int i = 0; i < pixelCount; ++i)
	{
		auto value = p->v()[channel];

		switch (bpp)
		{
			default:
			case 8:
			{
				uint8_t intValue = std::round(value * 0xFF);
				std::array<uint8_t, 1> array;
				array[0] = intValue;
				data.append(reinterpret_cast<const char *>(array.data()), array.size());
				break;
			}
			case 16:
			{
				uint16_t intValue = std::round(value * 0xFFFF);
				std::array<uint8_t, 2> array;
				array[0] = intValue >> 8;
				array[1] = intValue;
				data.append(reinterpret_cast<const char *>(array.data()), array.size());
				break;
			}
			case 32:
			{
				uint32_t intValue = std::round(double(value) * 0xFFFFFFFF);
				std::array<uint8_t, 4> array;
				array[0] = intValue >> 24;
				array[1] = intValue >> 16;
				array[2] = intValue >> 8;
				array[3] = intValue;
				data.append(reinterpret_cast<const char *>(array.data()), array.size());
				break;
			}
		}

		++p;
	}

	auto channelData = std::make_shared<PsdChannelData>();
	channelData->rawData = data;
	return channelData;
}

static int psdChannel(int channel)
{
	switch (channel)
	{
	default:
	case Malachite::Pixel::Index::A:
		return -1;
	case Malachite::Pixel::Index::R:
		return 0;
	case Malachite::Pixel::Index::G:
		return 1;
	case Malachite::Pixel::Index::B:
		return 2;
	}
}

template <int channel, int bpp>
static void addChannel(const Malachite::Image &image, QVector<Ref<PsdChannelData>> &channelDatas, QVector<PsdChannelInfo> &channelInfos)
{
	auto data = writeChannel<channel, bpp>(image);
	PsdChannelInfo info;
	info.id = psdChannel(channel);
	info.length = data->rawData.size() + 2;
	channelDatas << data;
	channelInfos << info;
}

template <int bpp>
static void addChannels(const Malachite::Image &image, QVector<Ref<PsdChannelData>> &channelDatas, QVector<PsdChannelInfo> &channelInfos)
{
	addChannel<Malachite::Pixel::Index::A, bpp>(image, channelDatas, channelInfos);
	addChannel<Malachite::Pixel::Index::R, bpp>(image, channelDatas, channelInfos);
	addChannel<Malachite::Pixel::Index::G, bpp>(image, channelDatas, channelInfos);
	addChannel<Malachite::Pixel::Index::B, bpp>(image, channelDatas, channelInfos);
}


void save(Malachite::Image &&image, QVector<Ref<PsdChannelData>> &channelDatas, QVector<PsdChannelInfo> &channelInfos, int bpp)
{
	int pixelCount = image.area();
	int dataSize = pixelCount * 4 * 2;

	QByteArray data;
	data.reserve(dataSize);

	{
		auto p = image.bits();
		for (int i = 0; i < pixelCount; ++i)
		{
			auto a = p->a();
			if (a)
				p->rv() /= Malachite::Pixel(1.f, a, a, a).v();
			++p;
		}
	}

	switch (bpp)
	{
		default:
		case 8:
			addChannels<8>(image, channelDatas, channelInfos);
			break;
		case 16:
			addChannels<16>(image, channelDatas, channelInfos);
			break;
		case 32:
			addChannels<32>(image, channelDatas, channelInfos);
			break;
	}
}

void saveEmpty(QVector<Ref<PsdChannelData> > &channelDatas, QVector<PsdChannelInfo> &channelInfos)
{
	for (int channel : {-1, 0, 1, 2})
	{
		PsdChannelInfo info;
		info.id = channel;
		info.length = 2;
		channelInfos << info;

		auto data = std::make_shared<PsdChannelData>();
		channelDatas << data;
	}
}

QByteArray saveAsImageData(Malachite::Image &&image, int bpp)
{
	QVector<Ref<PsdChannelData>> channelDatas;
	QVector<PsdChannelInfo> channelInfos;

	save(std::move(image), channelDatas, channelInfos, bpp);

	QByteArray data;
	data += channelDatas[1]->rawData;
	data += channelDatas[2]->rawData;
	data += channelDatas[3]->rawData;
	data += channelDatas[0]->rawData;

	return data;
}

} // namespace PsdImageSave
} // namespace PaintField
