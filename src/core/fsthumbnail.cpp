#include <QtGui>

#include "fsthumbnail.h"

FSThumbnail::FSThumbnail(const QPixmap &originalPixmap)
{
	QPixmap pixmap = originalPixmap.scaled(PixmapSize, PixmapSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
	QPixmap shadow;
	QRect pixmapRect(Margin + PixmapSize / 2 - pixmap.size().width() / 2, Margin + PixmapSize / 2 - pixmap.size().height() / 2, pixmap.size().width(), pixmap.size().height());
	
	if (_shadowCache.contains(pixmap.size())) {
		shadow = _shadowCache.value(pixmap.size());
	} else {
		shadow = QPixmap(thumbnailSize());
		shadow.fill(Qt::transparent);
		
		QGraphicsScene scene(shadow.rect());
		QGraphicsRectItem *rectItem = scene.addRect(pixmapRect, Qt::NoPen, Qt::white);
		QGraphicsDropShadowEffect effect(0);
		effect.setBlurRadius(6);
		effect.setOffset(0);
		rectItem->setGraphicsEffect(&effect);
		
		QPainter painter(&shadow);
		scene.render(&painter);
		painter.end();
		
		_shadowCache.insert(pixmap.size(), shadow);
	}
	
	_thumbnail = shadow;
	QPainter painter(&_thumbnail);
	painter.drawPixmap(pixmapRect.topLeft(), pixmap);
}

QPixmap FSThumbnail::createThumbnail(const QPixmap &pixmap)
{
	FSThumbnail newThumbnail(pixmap);
	return newThumbnail.thumbnail();
}

QHash<QSize, QPixmap> FSThumbnail::_shadowCache;

