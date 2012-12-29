#include "brushstroker.h"

#include "brushstrokingthread.h"

namespace PaintField {

BrushStrokingThread::BrushStrokingThread(QObject *parent) :
    QThread(parent)
{
}

BrushStrokingThread::~BrushStrokingThread()
{
}

void BrushStrokingThread::reset(BrushStroker *stroker)
{
	_stroker.reset(stroker);
	_moveToRequested = false;
	_finishRequested = false;
	_queue.clear();
}

void BrushStrokingThread::moveTo(const TabletInput &data)
{
	QMutexLocker locker(&_mutex);
	_queue.enqueue(data);
	_moveToRequested = true;
}

void BrushStrokingThread::lineTo(const TabletInput &data)
{
	QMutexLocker locker(&_mutex);
	_queue.enqueue(data);
}

void BrushStrokingThread::waitForFinish()
{
	QMutexLocker locker(&_mutex);
	_finishRequested = true;
	locker.unlock();
	
	wait();
}

void BrushStrokingThread::run()
{
	forever
	{
		QMutexLocker locker(&_mutex);
		
		if (_queue.size() == 0)
		{
			if (_finishRequested)
				break;
			else
				continue;
		}
		
		TabletInput data = _queue.dequeue();
		
		bool moveToRequested = _moveToRequested;
		_moveToRequested = false;
		
		locker.unlock();
		
		if (moveToRequested)
			_stroker->moveTo(data);
		else
			_stroker->lineTo(data);
	}
}

} // namespace PaintField
