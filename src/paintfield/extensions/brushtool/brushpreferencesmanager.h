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
	
public slots:
	
	void setBrushSize(int brushSize);
	
	void onCurrentPresetItemChanged(QStandardItem *item, QStandardItem *prev);
	
signals:
	
	void brushSizeChanged(int brushSize);
	
private:
	
	int _brushSize = 5;
	QHash<QStandardItem *, int> _brushSizeHash;
};

} // namespace PaintField

#endif // PAINTFIELD_BRUSHPREFERENCESMANAGER_H
