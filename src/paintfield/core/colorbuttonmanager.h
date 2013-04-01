#ifndef PAINTFIELD_COLORBUTTONMANAGER_H
#define PAINTFIELD_COLORBUTTONMANAGER_H

#include <QObject>
#include <Malachite/Color>

class QAbstractButton;

namespace PaintField {

class ColorButton;

class ColorButtonManager : public QObject
{
	Q_OBJECT
public:
	
	explicit ColorButtonManager(QObject *parent = 0);
	~ColorButtonManager();
	
	void add(ColorButton *button);
	void remove(ColorButton *button);
	
	Malachite::Color currentColor() const;
	ColorButton *currentButton();
	
signals:
	
	void currentColorChanged(const Malachite::Color &color);
	void currentButtonChanged(ColorButton *button);
	
public slots:
	
	void setCurrentColor(const Malachite::Color &color);
	void setCurrentButton(ColorButton *button);
	
private:
	
	void onCurrentButtonChanged(QAbstractButton *button);
	
private:
	
	struct Data;
	Data *d;
};

} // namespace PaintField

#endif // PAINTFIELD_COLORBUTTONMANAGER_H
