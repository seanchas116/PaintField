#ifndef DEBUG_H
#define DEBUG_H

#include <QDebug>
#include <QElapsedTimer>

#define PAINTFIELD_WARNING qWarning() << Q_FUNC_INFO << ":"
#define PAINTFIELD_DEBUG qDebug() << Q_FUNC_INFO << ":"

namespace PaintField
{

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
		qDebug() << _functionName.toLocal8Bit().data() << "took" << _timer.elapsed() << "ms";
	}
	
private:
	QElapsedTimer _timer;
	const QString _functionName;
};

}

#define PAINTFIELD_CALC_SCOPE_ELAPSED_TIME	ScopedTimer timer__(Q_FUNC_INFO)


#endif // DEBUG_H
