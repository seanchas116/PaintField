#include <QtGui>

#include "fsgrouplayer.h"

void FSGroupLayer::updateThumbnail(const QSize &size)
{
	FSLayer::updateThumbnail(size);
	
	QPixmap folderIcon(":/icons/22x22/folder.png");
	if (folderIcon.isNull())
		qDebug() << "resource not found";
	
	QPixmap thumbnail = this->thumbnail();
	QPainter painter(&thumbnail);
	painter.drawPixmap(38, 38, folderIcon);
	
	setThumbnail(thumbnail);
}
