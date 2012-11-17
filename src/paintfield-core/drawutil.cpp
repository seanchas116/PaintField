#include <QtGui>
#include <Malachite/GenericImage>
#include "util.h"

#include "drawutil.h"

namespace PaintField
{

using namespace Malachite;

// assumes both dst and src are 16bit aligned
// count must be a multiple of 4. reminder data is ignored.
void copyColorFast(int count, Vec4U8 *dst, const Vec4F *src)
{
	int countPer4 = count / 4;
	
	for (int i = 0; i < countPer4; ++i)
	{
		Vec4I32 d0 = vecRound(*src * 0xFF);
		src++;
		Vec4I32 d1 = vecRound(*src * 0xFF);
		src++;
		
		Vec8I16 w0 = Vec8I16::pack(d0, d1);
		
		Vec4I32 d2 = vecRound(*src * 0xFF);
		src++;
		Vec4I32 d3 = vecRound(*src * 0xFF);
		src++;
		
		Vec8I16 w1 = Vec8I16::pack(d2, d3);
		
		Vec16U8 b = Vec16U8::pack(w0, w1);
		
		*(reinterpret_cast<Vec16U8 *>(dst)) = b;
		
		dst += 4;
	}
}

void drawMLSurface(QPainter *painter, const QPoint &point, const Surface &surface)
{
	foreach (const QPoint &key, surface.keys()) {
		drawMLImage(painter, point + key * Surface::TileSize, surface.tileForKey(key));
	}
}

void drawMLImage(QPainter *painter, const QPoint &point, const Image &image)
{
	QImage qimage(image.size(), QImage::Format_ARGB32_Premultiplied);
	
	auto wrapper = GenericImage<ImageFormatArgbFast, Vec4U8>::wrap(qimage.bits(), qimage.size());
	
	wrapper.paste(image);
	painter->drawImage(point, qimage);
}

void drawMLImageFast(QPainter *painter, const QPoint &point, const Image &image)
{
	QSize size = image.size();
	int pixelCount = size.width() * size.height();
	
	uint8_t *buffer = reinterpret_cast<uint8_t *>(allocateAlignedMemory(pixelCount * 4, 16));
	
	copyColorFast(pixelCount, reinterpret_cast<Vec4U8 *>(buffer), image.constBits());
	
	QImage qimage(buffer, size.width(), size.height(), QImage::Format_ARGB32_Premultiplied);
	painter->drawImage(point, qimage);
	
	freeAlignedMemory(buffer);
}

void drawMLImageFast(QPainter *painter, const QPoint &point, const Image &image, const QRect &rect)
{
	QRect copyRect = rect & image.rect();
	
	copyRect.setTop(copyRect.top() / 4 * 4);
	
	if (copyRect.width() % 4)
		copyRect.setWidth(copyRect.width() / 4 * 4 + 4);
	
	int pixelCount = copyRect.width() * copyRect.height();
	
	Vec4U8 *buffer = reinterpret_cast<Vec4U8 *>(allocateAlignedMemory(pixelCount * 4, 16));
	
	for (int y = copyRect.top(); y <= copyRect.bottom(); ++y)
	{
		copyColorFast(copyRect.width(), buffer + y * copyRect.width(), image.constPixelPointer(copyRect.x(), y));
	}
	
	QImage qimage(reinterpret_cast<uint8_t *>(buffer), copyRect.width(), copyRect.height(), QImage::Format_ARGB32_Premultiplied);
	painter->drawImage(point + rect.topLeft(), qimage);
	
	freeAlignedMemory(buffer);
}

}

