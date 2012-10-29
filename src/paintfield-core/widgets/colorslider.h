#ifndef FSCOLORSLIDER_H
#define FSCOLORSLIDER_H

#include <QWidget>
#include <Malachite/Color>

namespace PaintField
{

class ColorSlider : public QWidget
{
	Q_OBJECT
public:
	
	enum {
		BarMargin = 2,
		BarHeight = 12
	};
	
	explicit ColorSlider(QWidget *parent = 0);
	ColorSlider(Malachite::Color::Component component, int stepCount, QWidget *parent = 0);
	
	QSize sizeHint() const;
	
	Malachite::Color color() const { return _color; }
	Malachite::Color::Component component() const { return _component; }
	double value() const { return _color.component(_component); }
	int value8Bit() const { return round(value() * 255); }
	int stepCount() const { return _stepCount; }
	
	void setComponent(Malachite::Color::Component component);
	void setStepCount(int count) { _stepCount = count; }
	
public slots:
	
	void setColor(const Malachite::Color &color);
	void setValue(double x);
	void setValue8Bit(int x);
	
signals:
	
	void valueChanged(double x);
	void value8BitChanged(int x);
	void colorChanged(const Malachite::Color &color);
	
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
	
	Malachite::Color _color;
	Malachite::Color::Component _component;
	int _stepCount;
	
	bool _mouseTracked;
	QImage _image;
};

}

#endif // FSCOLORSLIDER_H
