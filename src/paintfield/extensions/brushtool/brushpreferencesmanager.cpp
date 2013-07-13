

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

void BrushPreferencesManager::setSmoothEnabled(bool enabled)
{
	if (_smoothEnabled != enabled)
	{
		_smoothEnabled = enabled;
		emit smoothEnabledChanged(enabled);
	}
}

void BrushPreferencesManager::onPresetChanged(const QVariantMap &preset, const QString &filePath)
{
	_brushSizeHash[_currentPresetPath] = _brushSize;
	
	int defaultBrushSize;
	
	if (preset.contains("default-size"))
		defaultBrushSize = preset["default-size"].toInt();
	else
		defaultBrushSize = _brushSize;
	
	setBrushSize(_brushSizeHash.value(filePath, defaultBrushSize));
	
	_currentPresetPath = filePath;
}

} // namespace PaintField
