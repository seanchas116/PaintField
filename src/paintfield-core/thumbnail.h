#ifndef FSTHUMBNAIL_H
#define FSTHUMBNAIL_H

#include <QPixmap>
#include <QHash>
#include <Malachite/Misc>
#include "global.h"

namespace PaintField {

class Thumbnail
{
public:
	enum Metric {
		PixmapSize = 48,
		Margin = 6
	};
	
	Thumbnail(const QPixmap &originalPixmap);
	
	QPixmap thumbnail() const { return _thumbnail; }
	static QPixmap createThumbnail(const QPixmap &pixmap);
	static QSize thumbnailSize() { return QSize(PixmapSize + 2 * Margin, PixmapSize + 2 * Margin); }
	
private:
	QPixmap _thumbnail;
	static QHash<QSize, QPixmap> _shadowCache;
};

}

#endif // FSTHUMBNAIL_H
