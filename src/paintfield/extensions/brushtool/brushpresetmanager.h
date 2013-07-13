#ifndef PAINTFIELD_BRUSH_PRESETMANAGER_H
#define PAINTFIELD_BRUSH_PRESETMANAGER_H

#include <QObject>
#include <QStringList>
#include <QVariant>

namespace PaintField {

class BrushPresetMetadata
{
public:
	
	BrushPresetMetadata() {}
	BrushPresetMetadata(const QVariantMap &variantMap);
	
	QString title() const { return _title; }
	void setTitle(const QString &title) { _title = title; }
	
	QString description() const { return _description; }
	void setDescription(const QString &description) { _description = description; }
	
	QStringList tags() const { return _tags; }
	void setTags(const QStringList &tags) { _tags = tags; }
	
	QVariantMap toVariantMap() const;
	
private:
	
	QString _title, _description;
	QStringList _tags;
};

class BrushPresetManager : public QObject
{
	Q_OBJECT
public:
	explicit BrushPresetManager(QObject *parent = 0);
	
	BrushPresetMetadata metadata() const { return BrushPresetMetadata(_preset["metadata"].toMap()); }
	void setMetadata(const BrushPresetMetadata &metadata);
	
	QString stroker() const { return _preset["stroker"].toString(); }
	void setStroker(const QString &stroker);
	
	QVariantMap settings() const { return _preset["settings"].toMap(); }
	void setSettings(const QVariantMap &settings);
	
	QVariantMap preset() const { return _preset; }
	
public slots:
	
	void setPreset(const QString &path);
	
signals:
	
	void presetChanged(const QVariantMap &preset, const QString &filePath);
	void metadataChanged(const BrushPresetMetadata &metadata);
	void strokerChanged(const QString &stroker);
	void settingsChanged(const QVariantMap &settings);
	
private:
	
	QVariantMap _preset;
};

} // namespace PaintField

#endif // PAINTFIELD_BRUSH_PRESETMANAGER_H
