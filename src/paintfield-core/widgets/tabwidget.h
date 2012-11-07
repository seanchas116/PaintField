#ifndef TABWIDGET_H
#define TABWIDGET_H

#include <QFrame>
#include <QTabBar>

class QVBoxLayout;

namespace PaintField
{

class TabWidget : public QFrame
{
	Q_OBJECT
public:
	explicit TabWidget(QWidget *parent = 0);
	TabWidget(QTabBar *tabBar, QWidget *defaultWidget, QWidget *parent = 0);
	
	int addTab(QWidget *widget, const QString &label) { insertTab(count(), widget, label); return count(); }
	void insertTab(int index, QWidget *widget, const QString &label);
	QWidget *takeTab(int index);
	void takeTab(QWidget *widget);
	
	bool contains(int index) const { return 0 <= index && index < count(); }
	bool contains(QWidget *widget) { return _widgets.contains(widget); }
	int count() const { return _widgets.size(); }
	int indexOf(QWidget *widget) { return _widgets.indexOf(widget); }
	
signals:
	
	void currentIndexChanged(int index);
	
public slots:
	
	void setCurrentIndex(int index);
	void setCurrentWidget(QWidget *widget);
	
protected:
	
	QTabBar *tabBar() { return _tabBar; }
	
private slots:
	
	void onWidgetDeleted(QObject *obj);
	
private:
	
	void addWidgetToLayout(QWidget *widget);
	void removeWidgetFromLayout(QWidget *widget);
	void updateDisplayWidget();
	
	QTabBar *_tabBar = 0;
	QWidget *_defaultWidget = 0;
	QWidgetList _widgets;
	int _currentIndex = -1;
	QWidget *_currentWidget = 0;
	bool _isDefaultWidgetShown = true;
	
	QVBoxLayout *_layout = 0;
};

}

#endif // TABWIDGET_H
