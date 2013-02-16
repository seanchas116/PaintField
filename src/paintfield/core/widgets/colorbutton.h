#ifndef FSCOLORBUTTON_H
#define FSCOLORBUTTON_H

#include <QAbstractButton>
#include <Malachite/Color>

namespace PaintField
{

class ColorButton : public QAbstractButton
{
	Q_OBJECT
public:
	
	enum
	{
		ButtonSize = 20,
		ButtonMargin = 2
	};
	
	explicit ColorButton(QWidget *parent = 0);
	
	QSize sizeHint() const;
	Malachite::Color color() const { return _color; }
	
signals:
	
public slots:
	void setColor(const Malachite::Color &c);
	
protected:
	void paintEvent(QPaintEvent *e);
	
private:
	
	Malachite::Color _color;
};

}

#endif // FSCOLORBUTTON_H
