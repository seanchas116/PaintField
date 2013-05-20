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

void BrushPreferencesManager::onCurrentPresetItemChanged(QStandardItem *item, QStandardItem *prev)
{
	_brushSizeHash[prev] = _brushSize;
	setBrushSize(_brushSizeHash.value(item, _brushSize));
}

} // namespace PaintField
