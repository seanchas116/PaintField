#include "fspalettemanager.h"

FSPaletteManager::FSPaletteManager(QObject *parent) :
	QObject(parent),
	_currentIndex(0)
{
	_colors.reserve(ColorCount);
	
	for (int i = 0; i < ColorCount; ++i) {
		_colors << MLColor::fromRgbValue(1.0, 1.0, 1.0);
	}
}

void FSPaletteManager::setColor(int index, const MLColor &color)
{
	if (index < 0 || ColorCount <= index)
		return;
	
	_colors[index] = color;
	emit colorChanged(index, color);
	
	if (index == _currentIndex)
		emit currentColorChanged(color);
}

void FSPaletteManager::setCurrentIndex(int index)
{
	if (index < 0 || ColorCount <= index)
		return;
	
	_currentIndex = index;
	emit currentIndexChanged(index);
	emit colorChanged(index, currentColor());
	emit currentColorChanged(currentColor());
}

