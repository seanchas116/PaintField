#ifndef PAINTFIELD_BRUSHPREFERENCESMANAGER_H
#define PAINTFIELD_BRUSHPREFERENCESMANAGER_H

#include <QObject>
#include <QHash>

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
	
	void onCurrentPresetItemChanged(QStandardItem *item, QStandardItem *prev);
	
signals:
	
	void brushSizeChanged(int brushSize);
	void smoothEnabledChanged(bool enabled);
	
private:
	
	int _brushSize = 5;
	QHash<QStandardItem *, int> _brushSizeHash;
	bool _smoothEnabled = false;
};

} // namespace PaintField

#endif // PAINTFIELD_BRUSHPREFERENCESMANAGER_H
