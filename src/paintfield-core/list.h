#ifndef LIST_H
#define LIST_H

#include <QList>

namespace PaintField
{

template <typename T>
class List : public QList<T>
{
public:
	
	List() : QList<T>() {}
	List(const QList<T> &other) : QList<T>(other) {}
	List(std::initializer_list<T> args) : QList<T>(args) {}
	
	T head() const { return this->first(); }
	List tail() const
	{
		List ret = *this;
		ret.removeFirst();
		return ret;
	}
	List init() const
	{
		List ret = *this;
		ret.removeLast();
		return ret;
	}
	
	List take(int count) const
	{
		return this->mid(0, count);
	}
	List drop(int count) const
	{
		return this->mid(count, -1);
	}
	List dropRight(int count) const
	{
		return this->mid(0, this->size() - count);
	}
	
	template <typename Predicate>
	List filter(Predicate pred) const
	{
		List ret;
		
		for (const T &item : *this)
		{
			if (pred(item))
				ret << item;
		}
		
		return ret;
	}
	
	template <typename Predicate>
	T find(Predicate pred, const T &defaultValue) const
	{
		for (const T &item : *this)
		{
			if (pred(item))
				return item;
		}
		return defaultValue;
	}
	
	operator QList<T> () const
	{
		return *static_cast<QList<T> >(this);
	}
	
	template <typename Predicate>
	List find(Predicate pred) const
	{
		for (const T &item : *this)
		{
			if (pred(item))
				return {item};
		}
		
		return List();
	}
};

}

#endif // LIST_H
