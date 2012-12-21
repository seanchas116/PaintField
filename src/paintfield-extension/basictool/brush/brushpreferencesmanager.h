#ifndef PAINTFIELD_BRUSHPREFERENCESMANAGER_H
#define PAINTFIELD_BRUSHPREFERENCESMANAGER_H

#include <QObject>

namespace PaintField {

class BrushPreferencesManager : public QObject
{
	Q_OBJECT
public:
	explicit BrushPreferencesManager(QObject *parent = 0);
	
	int brushSize() const { return _brushSize; }
	
public slots:
	
	void setBrushSize(int brushSize);
	
signals:
	
	void brushSizeChanged(int brushSize);
	
private:
	
	int _brushSize = 10;
};

} // namespace PaintField

#endif // PAINTFIELD_BRUSHPREFERENCESMANAGER_H
