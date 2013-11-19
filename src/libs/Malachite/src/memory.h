#pragma once

//ExportName: Memory

#include <QtGlobal>
#include "global.h"

namespace Malachite
{

#ifdef QT_DEBUG
#define ML_DEBUG_MEMORY
#endif

template <class T>
class MALACHITESHARED_EXPORT Pointer
{
public:
	
	using value_type = T;
	
#ifdef ML_DEBUG_MEMORY
	
	Pointer() :
		_start(0),
		_byteSize(0),
		_p(0)
	{}
	
	Pointer(const void *rangeStart, int rangeByteSize, T *p) :
		_start(rangeStart),
		_byteSize(rangeByteSize),
		_p(p)
	{}
	
	Pointer(T *p, int rangeByteSize) :
		_start(p),
		_byteSize(rangeByteSize),
		_p(p)
	{}
	
	Pointer(T &value) :
		_start(&value),
		_byteSize(sizeof(T)),
		_p(&value)
	{}
	
#else
	
	Pointer() :
		_p(0)
	{}
	
	Pointer(const void *rangeStart, int rangeByteSize, T *p) :
		_p(p)
	{ Q_UNUSED(rangeStart); Q_UNUSED(rangeByteSize); }
	
	Pointer(T *p, int rangeByteSize) :
		_p(p)
	{ Q_UNUSED(rangeByteSize); }
	
	Pointer(T *p) :
		_p(p)
	{}
	
	Pointer(T &value) :
		_p(&value)
	{}
	
#endif
	
	bool isNull() const { return !_p; }
	bool isValid() const { return _p; }
	
	Pointer byteOffset(int offset) const
	{
#ifdef ML_DEBUG_MEMORY
		return Pointer(_start, _byteSize, reinterpret_cast<T *>(reinterpret_cast<size_t>(_p) + offset));
#else
		return Pointer(reinterpret_cast<T *>(reinterpret_cast<size_t>(_p) + offset));
#endif
	}
	
	void pasteArray(Pointer<const T> otherPointer, int elemCount)
	{
		pasteByte(otherPointer, elemCount * sizeof(T));
	}
	
	void pasteByte(Pointer<const T> otherPointer, int byteCount)
	{
#ifdef ML_DEBUG_MEMORY
		Q_ASSERT(isValid());
		Q_ASSERT(otherPointer.isValid());
		
		Q_ASSERT(remainingByteCount() >= byteCount);
		Q_ASSERT(otherPointer.remainingByteCount() >= byteCount);
#endif
		memcpy(_p, otherPointer, byteCount);
	}
	
	void fill(const T &value, int count)
	{
#ifdef ML_DEBUG_MEMORY
		Q_ASSERT(size_t(remainingByteCount()) >= size_t(count) * sizeof(T));
#endif
		
		Pointer<T> p = *this;
		for (int i = 0; i < count; ++i)
			*p++ = value;
	}
	
	template <class S>
	Pointer<S> reinterpret() const
	{
#ifdef ML_DEBUG_MEMORY
		return Pointer<S>(_start, _byteSize, reinterpret_cast<S *>(_p));
#else
		return Pointer<S>(reinterpret_cast<S *>(_p));
#endif
	}
	
	template <class S>
	Pointer<S> constCast() const
	{
#ifdef ML_DEBUG_MEMORY
		return Pointer<S>(_start, _byteSize, const_cast<S *>(_p));
#else
		return Pointer<S>(const_cast<S *>(_p));
#endif
	}
	
	Pointer operator+(int i) const
	{
#ifdef ML_DEBUG_MEMORY
		return Pointer(_start, _byteSize, _p + i);
#else
		return Pointer(_p + i);
#endif
	}
	Pointer operator-(int i) const { return operator+(-i); }
	
	Pointer &operator+=(int i) { _p += i; return *this; }
	Pointer &operator-=(int i) { _p -= i; return *this; }
	
	Pointer &operator++() { ++_p; return *this; }
	Pointer operator++(int) { Pointer p = *this; ++_p; return p; }
	Pointer &operator--() { --_p; return *this; }
	Pointer operator--(int) { Pointer p = *this; --_p;; return p; }
	
	operator Pointer<const T>() const
	{
#ifdef ML_DEBUG_MEMORY
		return Pointer<const T>(_start, _byteSize, _p);
#else
		return Pointer<const T>(_p);
#endif
	}
	
	operator T*() const
	{
#ifdef ML_DEBUG_MEMORY
		Q_ASSERT(rangeCorrect());
		Q_ASSERT(isValid());
#endif
		return _p;
	}
	
	T &operator*() const
	{
#ifdef ML_DEBUG_MEMORY
		Q_ASSERT(rangeCorrect());
		Q_ASSERT(isValid());
#endif
		return *_p;
	}
	
	T *operator->() const
	{
#ifdef ML_DEBUG_MEMORY
		Q_ASSERT(rangeCorrect());
		Q_ASSERT(isValid());
#endif
		return _p;
	}
	
	T &operator[](int index) const { return *(*this + index); }
	
#ifdef ML_DEBUG_MEMORY
	int remainingByteCount() const { return reinterpret_cast<size_t>(_start) + _byteSize - reinterpret_cast<size_t>(_p); }

	bool rangeCorrect() const
	{
		int index = reinterpret_cast<size_t>(_p) - reinterpret_cast<size_t>(_start);
		return 0 <= index && index <= _byteSize - (int)sizeof(T);
	}
#endif
	
private:
	
#ifdef ML_DEBUG_MEMORY
	const void * _start;
	int _byteSize;
#endif
	T *_p;
};

template <class T>
inline Pointer<T> wrapPointer(const void *rangeStart, int rangeByteSize, T *p)
{
	return Pointer<T>(rangeStart, rangeByteSize, p);
}

template <class T>
inline Pointer<const T> wrapPointer(const void *rangeStart, int rangeByteSize, const T *p)
{
	return Pointer<const T>(rangeStart, rangeByteSize, p);
}

template <class T>
inline Pointer<T> wrapPointer(T *p, int valueCount)
{
	return Pointer<T>(p, valueCount * sizeof(T), p);
}

template <class T>
inline Pointer<const T> wrapPointer(const T *p, int valueCount)
{
	return Pointer<const T>(p, valueCount * sizeof(T), p);
}

template <class T>
class MALACHITESHARED_EXPORT Array
{
public:
	
	Array(int size) :
		_data(new T[size]),
		_size(size)
	{}
	
	~Array()
	{
		delete[] _data;
	}
	
	int size() const { return _size; }
	
	Pointer<T> data() { return p(0); }
	Pointer<const T> data() const { return p(0); }
	
	Pointer<T> p(int index) { return Pointer<T>(_data, _size * sizeof(T), _data + index); }
	Pointer<const T> p(int index) const { return Pointer<const T>(_data, _size * sizeof(T), _data + index); }
	
	T &operator[](int index) { Q_ASSERT(0 <= index && index < _size); return _data[index]; }
	const T &operator[](int index) const { Q_ASSERT(0 <= index && index < _size); return _data[index]; }
	
private:
	
	T *_data;
	int _size;
};

}

