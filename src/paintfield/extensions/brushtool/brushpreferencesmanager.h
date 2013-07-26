#pragma once

#include <QObject>
#include <QHash>
#include <QVariant>

class QStandardItem;

namespace PaintField {

class BrushPreferencesManager : public QObject
{
	Q_OBJECT
public:
	explicit BrushPreferencesManager(QObject *parent = 0);
	
	int brushSize() const { return _brushSize; }
	bool isSmoothEnabled() const { return _smoothEnabled; }
	
public slots:
	
	void setBrushSize(int brushSize);
	void setSmoothEnabled(bool enabled);
	
	void onPresetChanged(const QVariantMap &preset, const QString &filePath);
	
signals:
	
	void brushSizeChanged(int brushSize);
	void smoothEnabledChanged(bool enabled);
	
private:
	
	int _brushSize = 5;
	QHash<QString, int> _brushSizeHash;
	bool _smoothEnabled = false;
	QString _currentPresetPath;
};

} // namespace PaintField

