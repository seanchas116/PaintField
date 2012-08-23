#ifndef FSPANEL_H
#define FSPANEL_H

#include <QWidget>

class QVBoxLayout;
class QLabel;

void fsApplyMacSmallSize(QWidget *widget);

class FSPanelWidget : public QWidget
{
	Q_OBJECT
public:
	
	enum
	{
		DefaultWidth = 200
	};
	
	FSPanelWidget(QWidget *parent = 0) : QWidget(parent) {}
	
protected:
	
	void applyMacSmallSize() { fsApplyMacSmallSize(this); }
	
private:
};

class FSPanel : public QWidget
{
	Q_OBJECT
public:
	FSPanel(QWidget *parent = 0);
	
	void setWidget(FSPanelWidget *widget);
	FSPanelWidget *widget() { return _widget; }
	
signals:
	
public slots:
	
protected:
	
	void closeEvent(QCloseEvent *event);
	
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	
private:
	
	FSPanelWidget *_widget;
	QLabel *_label;
	QVBoxLayout *_layout;
	
	QPoint _origPanelPos, _origMousePos;
};

#endif // FSPANEL_H
