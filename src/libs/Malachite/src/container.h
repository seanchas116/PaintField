#pragma once

//ExportName: Container

#include "misc.h"
#include <memory>

namespace Malachite
{

template <typename T>
QList<const T *> constList(const QList<T *> &list)
{
	return blindCast<const QList<const T* > >(list);
}

template <typename T>
QList<T *> nonConstList(const QList<const T *> &list)
{
	return blindCast<const QList<T* > >(list);
}

template <typename T>
QList<std::shared_ptr<const T>> constList(const QList<std::shared_ptr<T>> &list)
{
	return blindCast<const QList<std::shared_ptr<const T>>>(list);
}

template <typename T>
QList<std::shared_ptr<T>> nonConstList(const QList<std::shared_ptr<const T>> &list)
{
	return blindCast<const QList<std::shared_ptr<T>>>(list);
}

template <typename Container>
void forwardShiftContainer(Container &container, int start, int end)
{
	Q_ASSERT(0 <= start && start < container.size());
	Q_ASSERT(0 <= end && end < container.size());
	Q_ASSERT(start < end);
	
	typename Container::value_type v = container[end];
	for (int i = start; i != end; ++i)
		container[i+1] = container[i];
	container[start] = v;
}

template <typename Container>
void backwardShiftContainer(Container &container, int start, int end)
{
	Q_ASSERT(0 <= start && start < container.size());
	Q_ASSERT(0 <= end && end < container.size());
	Q_ASSERT(start < end);
	
	typename Container::value_type v = container[start];
	for (int i = end; i != start; --i)
		container[i-1] = container[i];
	container[end] = v;
}

template <typename Container>
void shiftContainer(Container &container, int start, int end, int count)
{
	if (start == end)
		return;
	
	if (start > end)
	{
		qSwap(start, end);
		count = -count;
	}
	
	count = count % (end - start + 1);
	
	if (count > 0)
	{
		while (count--)
			forwardShiftContainer(container, start, end);
	}
	else
	{
		count = -count;
		while (count--)
			backwardShiftContainer(container, start, end);
	}
}

template <typename Container>
class ReverseContainer
{
public:
	
	typedef typename Container::value_type value_type;
	typedef std::reverse_iterator<typename Container::const_iterator> const_iterator;
	
	ReverseContainer(const Container &container) : _container(container) {}
	
	const_iterator begin() const { return const_iterator(_container.end()); }
	const_iterator end() const { return const_iterator(_container.begin()); }
	
	int size() const { return _container.size(); }
	bool empty() const { return _container.empty(); }
	
	const value_type &operator[](int i) const { return _container[size() - i - 1]; }
	value_type at(int i) const { return _container.at(size() - i - 1); }
	value_type front() const { return _container.back(); }
	value_type back() const { return _container.front(); }
	
private:
	
	const Container _container;
};

template <typename Container>
inline ReverseContainer<Container> reverseContainer(const Container &container)
{
	return ReverseContainer<Container>(container);
}

}

