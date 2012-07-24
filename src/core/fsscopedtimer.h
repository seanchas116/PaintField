#ifndef FSSCOPEDTIMER_H
#define FSSCOPEDTIMER_H

#include <QElapsedTimer>
#include <QDebug>

class FSScopedTimer
{
public:
	FSScopedTimer(const QString &functionName) :
		_functionName(functionName)
	{
		_timer.start();
	}
	
	~FSScopedTimer()
	{
		qDebug() << _functionName << "took" << _timer.elapsed() << "ms";
	}
	
private:
	QElapsedTimer _timer;
	const QString _functionName;
};

#endif // FSSCOPEDTIMER_H
