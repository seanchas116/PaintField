#pragma once

#include <QObject>
#include <Malachite/Color>
#include "global.h"

namespace Malachite
{
class Color;
}

class QAbstractButton;

namespace PaintField {

class ColorButton;

class ColorButtonGroup : public QObject
{
	Q_OBJECT
public:
	explicit ColorButtonGroup(QObject *parent = 0);
	~ColorButtonGroup();
	
	void add(ColorButton *button);
	
	Malachite::Color currentColor() const;
	
	bool eventFilter(QObject *object, QEvent *event);
	
signals:
	
	void currentColorChanged(const Malachite::Color &color);
	
public slots:
	
	void setCurrentColor(const Malachite::Color &color);
	void setCurrentButton(ColorButton *button);
	
private slots:
	
	void onCurrentButtonChanged(QAbstractButton *button);
	void changeDisabledCurrentButton();
	
private:
	
	struct Data;
	Data *d;
};

} // namespace PaintField

