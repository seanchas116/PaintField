#ifndef FSCOLORBUTTON_H
#define FSCOLORBUTTON_H

#include <QAbstractButton>
#include "mlcolor.h"

class FSColorButton : public QAbstractButton
{
	Q_OBJECT
public:
	
	enum
	{
		ButtonSize = 22,
		ButtonMargin = 2
	};
	
	explicit FSColorButton(QWidget *parent = 0);
	
	QSize sizeHint() const;
	MLColor color() const { return _color; }
	
signals:
	
public slots:
	void setColor(const MLColor &c);
	
protected:
	void paintEvent(QPaintEvent *e);
	
private:
	
	MLColor _color;
};

#endif // FSCOLORBUTTON_H
