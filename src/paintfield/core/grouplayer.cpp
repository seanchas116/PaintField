#include "grouplayer.h"

namespace PaintField {

void GroupLayer::updateThumbnail(const QSize &size)
{
	QPixmap folderIcon(":/icons/22x22/folder.png");
	if (folderIcon.isNull())
		qDebug() << "resource not found";
	
	int width = ThumbnailSize;
	int offset = (ThumbnailSize - 22) / 2;
	
	QPixmap thumbnail(width, width);
	thumbnail.fill(Qt::transparent);
	
	QPainter painter(&thumbnail);
	painter.drawPixmap(offset, offset, folderIcon);
	
	setThumbnail(thumbnail);
}

QString GroupLayerFactory::name() const { return "group"; }

} // namespace PaintField
