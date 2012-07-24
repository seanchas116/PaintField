#include <QtGui>

#include "mlgenericwrapperimage.h"
#include "fsdrawutil.h"

void fsDrawMLSurface(QPainter *painter, const QPoint &point, const MLSurface &surface)
{
	foreach (const QPoint &key, surface.keys()) {
		fsDrawMLImage(painter, point + key * MLSurface::TileSize, surface.tileForKey(key));
	}
}

void fsDrawMLImage(QPainter *painter, const QPoint &point, const MLImage &image)
{
	QImage qimage(image.size(), QImage::Format_ARGB32_Premultiplied);
	qimage.fill(Qt::lightGray);
	MLGenericWrapperImage<MLFastArgb8> wrapperImage(qimage.bits(), qimage.size(), qimage.bytesPerLine());
	wrapperImage.paste(0, 0, image);
	painter->drawImage(point, qimage);
}

