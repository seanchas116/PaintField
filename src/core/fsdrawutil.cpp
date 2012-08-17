#include <QtGui>

#include "mlgenericimage.h"
#include "fsdrawutil.h"


// assumes both dst and src are 16bit aligned
void fsCopyColorFast(int count, MLVec4U8 *dst, const MLVec4F *src)
{
	int countPer4 = count / 4;
	
	for (int i = 0; i < countPer4; ++i)
	{
		MLVec4I32 d0 = mlRound(*src * 0xFF);
		src++;
		MLVec4I32 d1 = mlRound(*src * 0xFF);
		src++;
		
		MLVec8I16 w0 = MLVec8I16::pack(d0, d1);
		
		MLVec4I32 d2 = mlRound(*src * 0xFF);
		src++;
		MLVec4I32 d3 = mlRound(*src * 0xFF);
		src++;
		
		MLVec8I16 w1 = MLVec8I16::pack(d2, d3);
		
		MLVec16U8 b = MLVec16U8::pack(w0, w1);
		
		*(reinterpret_cast<MLVec16U8 *>(dst)) = b;
		
		dst += 4;
	}
}

void fsDrawMLSurface(QPainter *painter, const QPoint &point, const MLSurface &surface)
{
	foreach (const QPoint &key, surface.keys()) {
		fsDrawMLImage(painter, point + key * MLSurface::TileSize, surface.tileForKey(key));
	}
}

void fsDrawMLImage(QPainter *painter, const QPoint &point, const MLImage &image)
{
	QImage qimage(image.size(), QImage::Format_ARGB32_Premultiplied);
	
	auto wrapper = MLGenericImage<ML::ImageFormatArgbFast, MLVec4U8>::wrap(qimage.bits(), qimage.size());
	
	wrapper.paste<ML::ImageFormatArgbFast, MLVec4F>(image);
	painter->drawImage(point, qimage);
}

void fsDrawMLImageFast(QPainter *painter, const QPoint &point, const MLImage &image)
{
	QSize size = image.size();
	int pixelCount = size.width() * size.height();
	
	uint8_t *buffer = reinterpret_cast<uint8_t *>(mlAllocateAlignedMemory(pixelCount * 4, 16));
	fsCopyColorFast(pixelCount, reinterpret_cast<MLVec4U8 *>(buffer), image.constBits());
	
	QImage qimage(buffer, size.width(), size.height(), QImage::Format_ARGB32_Premultiplied);
	painter->drawImage(point, qimage);
	
	mlFreeAlignedMemory(buffer);
}

