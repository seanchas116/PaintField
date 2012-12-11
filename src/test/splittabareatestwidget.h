#ifndef SPLITTABAREATESTWIDGET_H
#define SPLITTABAREATESTWIDGET_H

#include <QWidget>
#include "paintfield-core/widgets/splittabareacontroller.h"

using namespace PaintField;

class SplitTabAreaTestWidget : public QWidget
{
	Q_OBJECT
public:
	explicit SplitTabAreaTestWidget(QWidget *parent = 0);
	
signals:
	
	void focusChanged(bool x);
	
public slots:
	
	void addTab();
	void splitHorizontal() { _controller->splitCurrentSplit(Qt::Horizontal); }
	void splitVertical() { _controller->splitCurrentSplit(Qt::Vertical); }
	void close() { _controller->closeCurrentSplit(); }
	
	void onTabChanged(QWidget *tab);
	
protected:
	
	void focusInEvent(QFocusEvent *);
	void focusOutEvent(QFocusEvent *);
	
private:
	
	int _tabCount = 0;
	SplitTabAreaController *_controller;
};

#endif // SPLITTABAREATESTWIDGET_H
