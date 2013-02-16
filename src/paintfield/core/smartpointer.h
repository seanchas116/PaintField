#ifndef SMARTPOINTER_H
#define SMARTPOINTER_H

#include <QPointer>

namespace PaintField
{

template <class T>
class ScopedQObjectPointer
{
public:
	
	ScopedQObjectPointer() {}
	ScopedQObjectPointer(T *p) : _p(p) {}
	
	~ScopedQObjectPointer()
	{
		destroy();
	}
	
	T *data() const { return _p; }
	
	bool isNull() const { return data(); }
	
	void reset(T *p = 0)
	{
		destroy();
		_p = p;
	}
	
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
	
	void destroy()
	{
		if (_p) _p->deleteLater();
		_p = 0;
	}
	
	QPointer<T> _p;
};

class GuardedQObjectList : public QObject
{
	Q_OBJECT
	
public:
	
	GuardedQObjectList(QObject *parent = 0) : QObject(parent) {}
	
	void append(QObject *object)
	{
		connect(object, SIGNAL(destroyed(QObject*)), this, SLOT(onObjectDestroyed(QObject*)));
		_list << object;
	}
	
	QObjectList list() { return _list; }
	
private slots:
	
	void onObjectDestroyed(QObject *obj)
	{
		_list.removeAll(obj);
	}
	
private:
	
	QObjectList _list;
};

template <class T>
class CountableSharedQObjectPointer
{
public:
	
	CountableSharedQObjectPointer(T *obj) :
		_count(new int),
		_obj(obj)
	{
		rcount() = 1;
	}
	
	CountableSharedQObjectPointer(const CountableSharedQObjectPointer &other) :
		_count(other._count),
		_obj(other._obj)
	{
		rcount()++;
	}
	
	~CountableSharedQObjectPointer()
	{
		rcount()--;
		if (rcount() <= 0)
		{
			delete _count;
			_obj->deleteLater();
		}
	}
	
	int count() const { return *_count; }
	T *obj() const { return _obj; }
	
	T *operator->() const { return _obj; }
	
private:
	
	int &rcount() { return *_count; }
	
	int *_count;
	T *_obj;
};

}

#endif // SMARTPOINTER_H
