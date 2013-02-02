#include <QtGui>
#include <Malachite/GenericImage>
#include <Malachite/PixelConversion>
#include "util.h"

#include "drawutil.h"

namespace PaintField
{

namespace DrawUtil
{

using namespace Malachite;

// assumes both dst and src are 16bit aligned
void copyColorFast(int count, BgraPremultU8 *dst, const Pixel *src)
{
	int countPer4 = count / 4;
	int rem = count % 4;
	
	while (countPer4--)
	{
		__m128i d0 = _mm_cvtps_epi32((src->v() * 0xFF).data());
		src++;
		__m128i d1 = _mm_cvtps_epi32((src->v() * 0xFF).data());
		src++;
		
		__m128i w0 = _mm_packs_epi32(d0, d1);
		
		__m128i d2 = _mm_cvtps_epi32((src->v() * 0xFF).data());
		src++;
		__m128i d3 = _mm_cvtps_epi32((src->v() * 0xFF).data());
		src++;
		
		__m128i w1 = _mm_packs_epi32(d2, d3);
		
		__m128i b = _mm_packus_epi16(w0, w1);
		
		*(reinterpret_cast<__m128i *>(dst)) = b;
		
		dst += 4;
	}
	
	if (rem)
	{
		auto dstDwords = reinterpret_cast<uint32_t *>(dst);
		
		auto convert1 = [](const Pixel &p) -> uint32_t
		{
			union
			{
				uint32_t dwords[4];
				__m128i d;
			} u;
			
			u.d = _mm_cvtps_epi32((p.v() * 0xFF).data());
			u.d = _mm_packs_epi32(u.d, u.d);
			u.d = _mm_packus_epi16(u.d, u.d);
			
			return u.dwords[0];
		};
		
		while (rem--)
			*dstDwords++ = convert1(*src++);
	}
}

void drawMLSurface(QPainter *painter, const QPoint &point, const Surface &surface)
{
	for (const QPoint &key : surface.keys())
		drawMLImage(painter, point + key * Surface::TileSize, surface.tileForKey(key));
}

void drawMLImage(QPainter *painter, const QPoint &point, const Image &image)
{
	QImage qimage(image.size(), QImage::Format_ARGB32_Premultiplied);
	
	auto wrapper = GenericImage<BgraPremultU8>::wrap(qimage.bits(), qimage.size());
	
	wrapper.paste(image);
	painter->drawImage(point, qimage);
}

void drawMLImageFast(QPainter *painter, const QPoint &point, const Image &image)
{
	QSize size = image.size();
	int pixelCount = size.width() * size.height();
	
	QScopedArrayPointer<BgraPremultU8> buffer(new BgraPremultU8[pixelCount]);
	
	copyColorFast(pixelCount, buffer.data(), image.constBits());
	
	QImage qimage(reinterpret_cast<uint8_t *>(buffer.data()), size.width(), size.height(), QImage::Format_ARGB32_Premultiplied);
	painter->drawImage(point, qimage);
}

void drawMLImageFast(QPainter *painter, const QPoint &point, const Image &image, const QRect &rect)
{
	QRect copyRect = rect & image.rect();
	
	copyRect.moveTop(copyRect.top() / 4 * 4);
	
	if (copyRect.width() % 4)
		copyRect.setWidth(copyRect.width() / 4 * 4 + 4);
	
	int pixelCount = copyRect.width() * copyRect.height();
	
	QScopedArrayPointer<BgraPremultU8> buffer(new BgraPremultU8[pixelCount]);
	
	for (int i = 0; i < copyRect.height(); ++i)
		copyColorFast(copyRect.width(), buffer.data() + i * copyRect.width(), image.constPixelPointer(copyRect.left(), copyRect.top() + i));
	
	QImage qimage(reinterpret_cast<uint8_t *>(buffer.data()), copyRect.width(), copyRect.height(), QImage::Format_ARGB32_Premultiplied);
	painter->drawImage(point + copyRect.topLeft(), qimage, rect.translated(-copyRect.topLeft()));
}

}

}

