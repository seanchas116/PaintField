#ifndef FSBORDERWIDGET_H
#define FSBORDERWIDGET_H

#include <QWidget>

class FSBorderWidget : public QWidget
{
	Q_OBJECT
public:
	explicit FSBorderWidget(QWidget *parent = 0);
	
	void setMargins(const QMargins &margins) { _margins = margins; updateHeight(); }
	QMargins margins() const { return _margins; }
	
	void setColor(const QColor &color) { _color = color; }
	QColor color() const { return _color; }
	
	void setHeight(int height) { _height = height; updateHeight(); }
	int height() const { return _height; }
	
signals:
	
public slots:
	
protected:
	
	void paintEvent(QPaintEvent *);
	
private:
	
	void updateHeight();
	
	int _height;
	QMargins _margins;
	QColor _color;
};

#endif // FSBORDERWIDGET_H
