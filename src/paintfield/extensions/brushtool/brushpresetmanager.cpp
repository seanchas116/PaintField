#include "brushpresetmanager.h"

#include "paintfield/core/json.h"
#include <boost/range/adaptors.hpp>

namespace PaintField {

BrushPresetManager::BrushPresetManager(QObject *parent) :
    QObject(parent)
{
}

void BrushPresetManager::setMetadata(const QVariantMap &metadata)
{
	m_metadata = metadata;
	emit metadataChanged(metadata);
}

void BrushPresetManager::setStroker(const QString &stroker)
{
	m_stroker = stroker;
	emit strokerChanged(stroker);
}

void BrushPresetManager::setSettings(const QVariantMap &settings)
{
	m_settings = settings;
	emit settingsChanged(settings);
}

void BrushPresetManager::setPreset(const QString &path)
{
	if (path.isEmpty())
		return;
	
	auto preset = Json::readFromFile(path).toMap();
	
	if (preset.isEmpty())
		return;
	
	m_metadata = preset["metadata"].toMap();
	m_stroker = preset["stroker"].toString();
	m_settings = preset["settings"].toMap();
	emit presetChanged(preset, path);
	emit metadataChanged(metadata());
	emit strokerChanged(stroker());
	emit settingsChanged(settings());
}


} // namespace PaintField
