#include "brushpresetmanager.h"

namespace PaintField {

BrushPresetMetadata::BrushPresetMetadata(const QVariantMap &variantMap)
{
	_title = variantMap["title"].toString();
	_description = variantMap["description"].toString();
	_tags = variantMap["tags"].toStringList();
}

QVariantMap BrushPresetMetadata::toVariantMap() const
{
	QVariantMap variantMap;
	
	variantMap["title"] = _title;
	variantMap["description"] = _description;
	variantMap["tags"] = _tags;
	
	return variantMap;
}

BrushPresetManager::BrushPresetManager(QObject *parent) :
    QObject(parent)
{
}

void BrushPresetManager::setMetadata(const BrushPresetMetadata &metadata)
{
	_preset["metadata"] = metadata.toVariantMap();
	emit metadataChanged(metadata);
}

void BrushPresetManager::setStroker(const QString &source)
{
	_preset["stroker"] = source;
	emit strokerChanged(source);
}

void BrushPresetManager::setSettings(const QVariantMap &settings)
{
	_preset["settings"] = settings;
	emit settingsChanged(settings);
}

void BrushPresetManager::setPreset(const QVariantMap &preset)
{
	_preset = preset;
	emit metadataChanged(metadata());
	emit strokerChanged(stroker());
	emit settingsChanged(settings());
}


} // namespace PaintField
