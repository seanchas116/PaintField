#include "paintfield-core/debug.h"
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
		PAINTFIELD_DEBUG;
		_brushSize = size;
		emit brushSizeChanged(size);
	}
}

} // namespace PaintField
