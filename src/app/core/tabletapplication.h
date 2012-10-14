#ifndef FSTABLETAPPLICATION_H
#define FSTABLETAPPLICATION_H

#include <QApplication>
#include <QTabletEvent>

namespace PaintField {

class TabletApplication : public QApplication
{
	Q_OBJECT
public:
	TabletApplication(int &argv, char **args);
	
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
	
	bool _isTabletActive = false;
	QTabletEvent::PointerType _tabletPointerType = QTabletEvent::UnknownPointer;
	qint64 _tabletId = 0;
};

}


#endif // FSTABLETAPPLICATION_H
