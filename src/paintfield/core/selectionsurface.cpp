#include "selectionsurface.h"

namespace PaintField {

SelectionImage::SelectionImage(const QSize &size) :
	mImage(size, QImage::Format_Mono)
{
}

void SelectionImage::fill(uint x)
{
	mImage.fill(x);
}

void SelectionImage::paste(const SelectionImage &other, const QPoint &pos)
{
	QPainter painter(&mImage);
	painter.drawImage(pos, other.mImage);
}

} // namespace PaintField
