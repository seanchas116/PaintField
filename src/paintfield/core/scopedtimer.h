#pragma once

#include <QElapsedTimer>
#include "global.h"

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

#define PAINTFIELD_CALC_SCOPE_ELAPSED_TIME	ScopedTimer timer__(Q_FUNC_INFO)

}
