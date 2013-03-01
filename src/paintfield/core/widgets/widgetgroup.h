#ifndef FSWIDGETGROUP_H
#define FSWIDGETGROUP_H

#include <QObject>

namespace PaintField
{

class WidgetGroup : public QObject
{
	Q_OBJECT
public:
	explicit WidgetGroup(QObject *parent = 0);
	~WidgetGroup();
	
	void addWidget(QWidget *widget);
	
	bool isVisible() const;
	bool isEnabled() const;
	
signals:
	
public slots:
	
	void setVisible(bool visible);
	void setEnabled(bool enabled);
	
private:
	
	struct Data;
	Data *d;
};

}

#endif // FSWIDGETGROUP_H
