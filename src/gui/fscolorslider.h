#ifndef FSCOLORSLIDER_H
#define FSCOLORSLIDER_H

#include <QWidget>
#include "mlcolor.h"

class FSColorSlider : public QWidget
{
	Q_OBJECT
public:
	
	enum {
		BarMargin = 2,
		BarHeight = 12
	};
	
	explicit FSColorSlider(QWidget *parent = 0);
	FSColorSlider(MLColor::Component component, QWidget *parent = 0);
	
	QSize sizeHint() const;
	
	MLColor color() const { return _color; }
	MLColor::Component component() const { return _component; }
	double value() const { return _color.component(_component); }
	int stepCount() const { return _stepCount; }
	
	void setComponent(MLColor::Component component);
	void setStepCount(int count) { _stepCount = count; }
	
public slots:
	
	void setColor(const MLColor &color);
	void setValue(double x);
	
signals:
	
	void valueChanged(double x);
	void colorChanged(const MLColor &color);
	
protected:
	
	void resizeEvent(QResizeEvent *);
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void paintEvent(QPaintEvent *);
	
private slots:
	
private:
	
	void init();
	
	void createNewImage();
	void updateImage();
	
	QSize barSize() const { return QSize(width() - 2 * BarMargin, BarHeight); }
	QRect barRect() const { return QRect(QPoint(BarMargin, BarMargin), barSize()); }
	
	MLColor _color;
	MLColor::Component _component;
	int _stepCount;
	
	bool _mouseTracked;
	QImage _image;
};

#endif // FSCOLORSLIDER_H
