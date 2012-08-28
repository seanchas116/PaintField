#ifndef FSAPPLICATION_H
#define FSAPPLICATION_H

#include <QApplication>
#include <QTabletEvent>

class FSApplication : public QApplication
{
	Q_OBJECT
public:
	FSApplication(int &argv, char **args);
	
	bool event(QEvent *event);
	
	bool isTabletActive() const { return _isTabletActive; }
	QTabletEvent::PointerType tabletPointerType() const { return _tabletPointerType; }
	quint64 tabletId() const { return _tabletId; }
	
signals:
	
	void tabletActivated();
	void tabletDeactivated();
	void tabletActiveChanged(bool active);
	void tabletPointerTypeChanged(QTabletEvent::PointerType type);
	void tabletIdChanged(quint64 id);
	
public slots:
	
private:
	
	bool _isTabletActive;
	QTabletEvent::PointerType _tabletPointerType;
	qint64 _tabletId;
};

#endif // FSAPPLICATION_H
