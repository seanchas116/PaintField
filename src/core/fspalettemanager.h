#ifndef FSPALETTEMANAGER_H
#define FSPALETTEMANAGER_H

#include <QObject>

#include "mlcolor.h"

class FSPaletteManager : public QObject
{
	Q_OBJECT
	
public:
	
	enum {
		ColorCount = 7
	};
	
	explicit FSPaletteManager(QObject *parent = 0);
	
	int currentIndex() const { return _currentIndex; }
	MLColor currentColor() const { return _colors.at(_currentIndex); }
	MLColor color(int index) const { return _colors.at(index); }
	int colorCount() const { return ColorCount; }
	
signals:
	
	void colorChanged(int index, const MLColor &color);
	void currentColorChanged(const MLColor &color);
	void currentIndexChanged(int index);
	
public slots:
	
	void setColor(int index, const MLColor &color);
	void setColor(const MLColor &color) { setColor(_currentIndex, color); }
	void setCurrentIndex(int index);
	
private:
	
	int _currentIndex;
	QList<MLColor> _colors;
};

#endif // FSPALETTEMANAGER_H
