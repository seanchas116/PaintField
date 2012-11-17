#include <QtGui>
#include <Malachite/GenericImage>
#include <Malachite/PixelConversion>
#include "util.h"

#include "drawutil.h"

namespace PaintField
{

using namespace Malachite;

// assumes both dst and src are 16bit aligned
void copyColorFast(int count, Vec4U8 *dst, const Vec4F *src)
{
	int countPer4 = count / 4;
	int rem = count % 4;
	
	while (countPer4--)
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
	
	while (rem--)
		convertPixel<ImageFormatArgbFast, Vec4U8, ImageFormatArgbFast, Vec4F>(*dst++, *src++);
}

void drawMLSurface(QPainter *painter, const QPoint &point, const Surface &surface)
{
	for (const QPoint &key : surface.keys())
		drawMLImage(painter, point + key * Surface::TileSize, surface.tileForKey(key));
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
	
	Vec4U8 *buffer = reinterpret_cast<Vec4U8 *>(allocateAlignedMemory(pixelCount * sizeof(Vec4U8), 16));
	
	copyColorFast(pixelCount, buffer, image.constBits());
	
	QImage qimage(reinterpret_cast<uint8_t *>(buffer), size.width(), size.height(), QImage::Format_ARGB32_Premultiplied);
	painter->drawImage(point, qimage);
	
	freeAlignedMemory(buffer);
}

void drawMLImageFast(QPainter *painter, const QPoint &point, const Image &image, const QRect &rect)
{
	QRect copyRect = rect & image.rect();
	
	copyRect.moveTop(copyRect.top() / 4 * 4);
	
	if (copyRect.width() % 4)
		copyRect.setWidth(copyRect.width() / 4 * 4 + 4);
	
	int pixelCount = copyRect.width() * copyRect.height();
	
	Vec4U8 *buffer = reinterpret_cast<Vec4U8 *>(allocateAlignedMemory(pixelCount * sizeof(Vec4U8), 16));
	
	for (int i = 0; i < copyRect.height(); ++i)
		copyColorFast(copyRect.width(), buffer + i * copyRect.width(), image.constPixelPointer(copyRect.left(), copyRect.top() + i));
	
	QImage qimage(reinterpret_cast<uint8_t *>(buffer), copyRect.width(), copyRect.height(), QImage::Format_ARGB32_Premultiplied);
	painter->drawImage(point + copyRect.topLeft(), qimage, rect.translated(-copyRect.topLeft()));
	
	freeAlignedMemory(buffer);
}

}

