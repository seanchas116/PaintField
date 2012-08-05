#include <QtGui>

#include "mlgenericwrapperimage.h"
#include "fsdrawutil.h"


// assumes both dst and src are 16bit aligned
void fsCopyColorFast(int count, MLFastArgb8 *dst, const MLFastArgbF *src)
{
	int countPer4 = count / 4;
	
	for (int i = 0; i < countPer4; ++i)
	{
		MLSimd32I4 d0 = mlSimdRound(src->v * 0xFF);
		src++;
		MLSimd32I4 d1 = mlSimdRound(src->v * 0xFF);
		src++;
		
		__v8hi w0 = __builtin_ia32_packssdw128(d0.v, d1.v);
		
		MLSimd32I4 d2 = mlSimdRound(src->v * 0xFF);
		src++;
		MLSimd32I4 d3 = mlSimdRound(src->v * 0xFF);
		src++;
		
		__v8hi w1 = __builtin_ia32_packssdw128(d2.v, d3.v);
		
		__v16qi b = __builtin_ia32_packuswb128(w0, w1);
		
		*(reinterpret_cast<__v16qi *>(dst)) = b;
		
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
	MLGenericWrapperImage<MLFastArgb8> wrapperImage(qimage.bits(), qimage.size(), qimage.bytesPerLine());
	wrapperImage.paste(0, 0, image);
	painter->drawImage(point, qimage);
}

void fsDrawMLImageFast(QPainter *painter, const QPoint &point, const MLImage &image)
{
	QSize size = image.size();
	int pixelCount = size.width() * size.height();
	
	uint8_t *buffer = reinterpret_cast<uint8_t *>(mlAllocateAlignedMemory(pixelCount * 4, 16));
	fsCopyColorFast(pixelCount, reinterpret_cast<MLFastArgb8 *>(buffer), image.constData());
	
	QImage qimage(buffer, size.width(), size.height(), QImage::Format_ARGB32_Premultiplied);
	painter->drawImage(point, qimage);
	
	mlFreeAlignedMemory(buffer);
}

