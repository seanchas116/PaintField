#ifndef FSPALETTEMANAGER_H
#define FSPALETTEMANAGER_H

#include <QObject>

#include "Malachite/mlcolor.h"

namespace PaintField
{

class PaletteManager : public QObject
{
	Q_OBJECT
	
public:
	
	enum {
		ColorCount = 7
	};
	
	explicit PaletteManager(QObject *parent = 0);
	
	int currentIndex() const { return _currentIndex; }
	Malachite::Color currentColor() const { return _colors.at(_currentIndex); }
	Malachite::Color color(int index) const { return _colors.at(index); }
	int colorCount() const { return ColorCount; }
	
signals:
	
	void colorChanged(int index, const Malachite::Color &color);
	void currentColorChanged(const Malachite::Color &color);
	void currentIndexChanged(int index);
	
public slots:
	
	void setColor(int index, const Malachite::Color &color);
	void setColor(const Malachite::Color &color) { setColor(_currentIndex, color); }
	void setCurrentIndex(int index);
	
private:
	
	int _currentIndex;
	QList<Malachite::Color> _colors;
};

}

#endif // FSPALETTEMANAGER_H
