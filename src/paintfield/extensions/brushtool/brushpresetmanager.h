#pragma once

#include <QObject>
#include <QStringList>
#include <QVariant>

namespace PaintField {

class BrushPresetManager : public QObject
{
	Q_OBJECT
public:
	explicit BrushPresetManager(QObject *parent = 0);

	QVariantMap metadata() const { return m_metadata; }
	void setMetadata(const QVariantMap &metadata);
	
	QString stroker() const { return m_stroker; }
	void setStroker(const QString &stroker);
	
	QVariantMap settings() const { return m_settings; }
	void setSettings(const QVariantMap &settings);

public slots:
	
	void setPreset(const QString &path);
	
signals:
	
	void presetChanged(const QVariantMap &preset, const QString &filePath);
	void metadataChanged(const QVariantMap &metadata);
	void strokerChanged(const QString &stroker);
	void settingsChanged(const QVariantMap &settings);
	
private:
	
	QVariantMap m_metadata;
	QString m_stroker;
	QVariantMap m_settings;
};

} // namespace PaintField

