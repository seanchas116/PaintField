#ifndef FSPANEL_H
#define FSPANEL_H

#include <QWidget>

class QVBoxLayout;
class QLabel;

void fsApplyMacSmallSize(QWidget *widget);

class FSPanelItem : public QWidget
{
	Q_OBJECT
public:
	
	enum 
	{
		
	};
	
	FSPanelItem(QWidget *parent = 0) : QWidget(parent) {}
	
	
private:
	
};

class FSPanel : public QWidget
{
	Q_OBJECT
public:
	FSPanel(QWidget *parent = 0);
	
	void setWidget(QWidget *widget);
	QWidget *widget() { return _widget; }
	
signals:
	
public slots:
	
protected:
	
	void closeEvent(QCloseEvent *event);
	
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	
private:
	
	QWidget *_widget;
	QLabel *_label;
	QVBoxLayout *_layout;
	
	QPoint _origPanelPos, _origMousePos;
};

#endif // FSPANEL_H
