#ifndef MLColorWHEEL_H
#define MLColorWHEEL_H

#include <QWidget>
#include "mlcolor.h"

class FSColorWheel : public QWidget
{
	Q_OBJECT
public:
	
	enum Metric {
		OuterCurcleRadius = 80,
		InnerCurcleRadius = 60,
		CenterSquareRadius = 40
	};
	
	explicit FSColorWheel(QWidget *parent = 0);
	
	QSize sizeHint() const;
	
	MLColor color() const { return _color; }
	
signals:
	void colorChanged(const MLColor &color);
	
public slots:
	void setColor(const MLColor &color);
	
protected:
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *);
	void paintEvent(QPaintEvent *);
	
private:
	QPoint center() const { return QPoint(width() / 2, height() / 2); }
	QPainterPath wheel() const;
	QRect square() const { return QRect(center().x() - CenterSquareRadius, center().y() - CenterSquareRadius, CenterSquareRadius * 2, CenterSquareRadius * 2); }
	void calculateH(const QPointF &pos);
	void calculateSV(const QPointF &pos);
	
	void updateHsv();
	
	bool _wheelBeingDragged, _squareBeingDragged;
	QImage _hueSelectorImage;
	MLColor _color;
};

#endif // MLColorWHEEL_H
