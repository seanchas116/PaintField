#include "brushpreferencesmanager.h"

namespace PaintField {

BrushPreferencesManager::BrushPreferencesManager(QObject *parent) :
    QObject(parent)
{
}

void BrushPreferencesManager::setBrushSize(int size)
{
	if (_brushSize != size)
	{
		_brushSize = size;
		emit brushSizeChanged(size);
	}
}

} // namespace PaintField
