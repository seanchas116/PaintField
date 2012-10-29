#include <QtGui>
#include <Malachite/GenericImage>

#include "drawutil.h"

namespace PaintField
{

using namespace Malachite;

// assumes both dst and src are 16bit aligned
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

}

