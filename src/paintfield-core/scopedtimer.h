#ifndef FSSCOPEDTIMER_H
#define FSSCOPEDTIMER_H

#include <QElapsedTimer>
#include <QDebug>

namespace PaintField {

class ScopedTimer
{
public:
	ScopedTimer(const QString &functionName) :
		_functionName(functionName)
	{
		_timer.start();
	}
	
	~ScopedTimer()
	{
		qDebug() << _functionName.toLocal8Bit() << "took" << _timer.elapsed() << "ms";
	}
	
private:
	QElapsedTimer _timer;
	const QString _functionName;
};

}

#endif // FSSCOPEDTIMER_H
