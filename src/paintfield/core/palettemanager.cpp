#include "palettemanager.h"

namespace PaintField
{

using namespace Malachite;

PaletteManager::PaletteManager(QObject *parent) :
	QObject(parent),
	_colors(ColorCount, Color::fromRgbValue(0, 0, 0, 1))
{}

void PaletteManager::setColor(int index, const Color &color)
{
	if (index < 0 || ColorCount <= index)
		return;
	
	if (_colors[index] != color)
	{
		_colors[index] = color;
		emit colorChanged(index, color);
		
		if (index == _currentIndex)
			emit currentColorChanged(color);
	}
}

void PaletteManager::setCurrentIndex(int index)
{
	if (index < 0 || ColorCount <= index)
		return;
	
	if (_currentIndex != index)
	{
		_currentIndex = index;
		emit currentIndexChanged(index);
		emit currentColorChanged(currentColor());
	}
}

}

