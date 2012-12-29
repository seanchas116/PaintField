#ifndef PAINTFIELD_BRUSHSTROKINGTHREAD_H
#define PAINTFIELD_BRUSHSTROKINGTHREAD_H

#include <QThread>
#include <QMutexLocker>
#include <QQueue>
#include <QScopedPointer>
#include "paintfield-core/tabletinputdata.h"

namespace PaintField {

class BrushStroker;

class BrushStrokingThread : public QThread
{
	Q_OBJECT
public:
	explicit BrushStrokingThread(QObject *parent = 0);
	~BrushStrokingThread();
	
	void reset(BrushStroker *stroker = 0);
	
	void moveTo(const TabletInput &data);
	void lineTo(const TabletInput &data);
	
	void waitForFinish();
	
	BrushStroker *stroker() { return _stroker.data(); }
	
	bool isStroking() const { return _stroker; }
	
signals:
	
public slots:
	
private:
	
	void run();
	
	QMutex _mutex;
	QQueue<TabletInput> _queue;
	bool _moveToRequested = false, _finishRequested = false;
	
	QScopedPointer<BrushStroker> _stroker;
};

} // namespace PaintField

#endif // PAINTFIELD_BRUSHSTROKINGTHREAD_H
