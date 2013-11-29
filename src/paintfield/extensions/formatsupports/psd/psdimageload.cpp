#include "psdimageload.h"

namespace PaintField {

namespace PsdImageLoad
{

template <int bpp>
static float loadComponent(const uint8_t *data);

template <int bpp, int channel>
static void loadData(Malachite::Image &image, const QByteArray &data)
{
	auto pixelP = image.begin();
	auto dataP = reinterpret_cast<const uint8_t *>(data.data());
	int count = image.area();

	if (count * (bpp / 8) != data.size())
		throw std::runtime_error("data size wrong");

	while (count--)
	{
		auto value = loadComponent<bpp>(dataP);
		pixelP->rv()[channel] = value;
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
			return;
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

template <> float loadComponent<8>(const uint8_t *data)
{
	return data[0] / float(0xFFU);
}

template <> float loadComponent<16>(const uint8_t *data)
{
	return (data[0] << 8U | data[1]) / float(0xFFFFU);
}

template <> float loadComponent<32>(const uint8_t *data)
{
	return (data[0] << 24U | data[1] << 16U | data[2] << 8U | data[3]) / float(0xFFFFFFFFU);
}

Malachite::Image load(const QVector<SP<PsdChannelData> > &channeldDataList, const QVector<PsdChannelInfo> &channelInfos, const QRect &rect, int bpp)
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
		auto p = image.begin();
		for (int i = 0; i < count; ++i)
		{
			auto a = p->a();
			Malachite::Pixel multiplier(1, a, a, a);
			p->rv() *= multiplier.v();
			++p;
		}
	}

	return image;
}

}

} // namespace PaintField
