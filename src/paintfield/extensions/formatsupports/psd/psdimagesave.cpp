#include "psdimagesave.h"

namespace PaintField {
namespace PsdImageSave {

template <int channel>
static Ref<PsdChannelData> writeChannel(const Malachite::Image &image)
{
	int pixelCount = image.area();
	QByteArray data;
	data.reserve(pixelCount * 4 * 2);

	auto p = image.constBits();

	for (int i = 0; i < pixelCount; ++i)
	{
		auto value = p->v()[channel];
		uint16_t word = std::round(value * 0xFFFF);
		std::array<uint8_t, 2> array;
		array[0] = word >> 8;
		array[1] = word;
		data.append(reinterpret_cast<const char *>(array.data()), array.size());
	}

	auto channelData = std::make_shared<PsdChannelData>();
	channelData->rawData = data;
	return channelData;
}

void save(Malachite::Image &&image, QVector<Ref<PsdChannelData>> &channelDatas, QVector<PsdChannelInfo> &channelInfos)
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
			{
				auto rec = 1.f / a;
				Malachite::Pixel factor(1.f, rec, rec, rec);
				p->rv() *= factor.v();
			}
			++p;
		}
	}

	auto psdChannel = [](int channel)
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
	};

	{
		auto data = writeChannel<Malachite::Pixel::Index::A>(image);
		PsdChannelInfo info;
		info.id = psdChannel(Malachite::Pixel::Index::A);
		info.length = data->rawData.size() + 2;
		channelDatas << data;
		channelInfos << info;
	}

	{
		auto data = writeChannel<Malachite::Pixel::Index::R>(image);
		PsdChannelInfo info;
		info.id = psdChannel(Malachite::Pixel::Index::R);
		info.length = data->rawData.size() + 2;
		channelDatas << data;
		channelInfos << info;
	}

	{
		auto data = writeChannel<Malachite::Pixel::Index::G>(image);
		PsdChannelInfo info;
		info.id = psdChannel(Malachite::Pixel::Index::G);
		info.length = data->rawData.size() + 2;
		channelDatas << data;
		channelInfos << info;
	}

	{
		auto data = writeChannel<Malachite::Pixel::Index::B>(image);
		PsdChannelInfo info;
		info.id = psdChannel(Malachite::Pixel::Index::B);
		info.length = data->rawData.size() + 2;
		channelDatas << data;
		channelInfos << info;
	}
}
} // namespace PsdImageSave
} // namespace PaintField
