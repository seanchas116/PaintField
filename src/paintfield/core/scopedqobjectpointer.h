#pragma once

#include <QScopedPointer>
#include <QObject>

namespace PaintField
{

struct DeleterOObject
{
	static inline void cleanup(QObject *obj)
	{
		obj->deleteLater();
	}
};

template <class T>
using ScopedQObjectPointer = QScopedPointer<T, DeleterOObject>;

}
