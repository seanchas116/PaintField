#ifndef UTIL_H
#define UTIL_H

#include <QPointer>

namespace PaintField
{

template <class T>
T *findQObjectReverse(const QList<T *> &list, const QString &id)
{
	QListIterator<T *> iter(list);
	iter.toBack();
	
	while (iter.hasPrevious())
	{
		T *p = iter.previous();
		
		if (p->objectName() == id)
			return p;
	}
	return 0;
}

template <class T>
class ScopedQObjectPointer
{
public:
	
	ScopedQObjectPointer() {}
	ScopedQObjectPointer(T *p) : _p(p) {}
	
	~ScopedQObjectPointer()
	{
		if (_p)
			reinterpret_cast<QObject *>(_p.data())->deleteLater();
	}
	
	T *data() const { return _p; }
	bool isNull() const { return data(); }
	void reset(T *p) { _p = p; }
	void swap(ScopedQObjectPointer<T> &other)
	{
		T *p = _p;
		_p = other._p;
		other._p = p;
	}
	
	T *take()
	{
		T *p = _p;
		_p = 0;
		return p;
	}
	
	operator bool() const { return _p; }
	bool operator!() const { return !_p; }
	T &operator*() const { return *_p; }
	T *operator->() const { return _p; }
	
private:
	
	QPointer<T> _p;
};

QVariant loadJsonFromFile(const QString &path);


}

#endif // UTIL_H
