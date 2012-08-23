#ifndef FSWIDGETGROUP_H
#define FSWIDGETGROUP_H

#include <QObject>

class FSWidgetGroup : public QObject
{
	Q_OBJECT
public:
	explicit FSWidgetGroup(QObject *parent = 0) : QObject(parent) {}
	
	void addWidget(QWidget *widget) { _widgets << widget; }
	
	template <class Widget>
	void addWidgets(const QList<Widget *> &widgets)
	{
		foreach (Widget *widget, widgets)
			addWidget(widget);
	}
	
signals:
	
public slots:
	
	void setVisible(bool visible);
	void setEnabled(bool enabled);
	
private:
	
	QList<QWidget *> _widgets;
};

#endif // FSWIDGETGROUP_H
