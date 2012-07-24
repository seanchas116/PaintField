#ifndef FSPANEL_H
#define FSPANEL_H

#include <QWidget>

void fsApplyMacSmallSize(QWidget *widget);

class FSPanel : public QWidget
{
	Q_OBJECT
public:
	FSPanel(QWidget *parent = 0);
	
signals:
	
public slots:
	
protected:
	
	void closeEvent(QCloseEvent *event);
};

#endif // FSPANEL_H
