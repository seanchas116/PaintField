#pragma once

//ExportName: List

#include <QList>
#include <type_traits>

namespace Malachite
{

template <typename T>
class List : public QList<T>
{
public:
	
	typedef QList<T> super;
	
	List() : super() {}
	List(const super &other) : super(other) {}
	List(std::initializer_list<T> args) : super(args) {}
	
	T max() const
	{
		if (this->size() == 0)
			return T();
		
		T value = head();
		
		for (int i = 1; i < this->size(); ++i)
		{
			T valueAt = this->at(i);
			
			if (valueAt > value)
				value = valueAt;
		}
		
		return value;
	}
	
	T min() const
	{
		if (this->size() == 0)
			return T();
		
		T value = head();
		
		for (int i = 1; i < this->size(); ++i)
		{
			T valueAt = this->at(i);
			
			if (valueAt < value)
				value = valueAt;
		}
		
		return value;
	}
	
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
	void removeIf(Predicate pred)
	{
		int count = this->size();
		for (int i = 0; i < count; ++i)
		{
			if (pred(this->at(i)))
			{
				this->removeAt(i);
				i--;
				count--;
			}
		}
	}
	
	template <typename Predicate>
	List select(Predicate pred) const
	{
		List ret;
		
		for (const T &item : *this)
		{
			if (pred(item))
				ret << item;
		}
		
		return ret;
	}
	
	template <typename Func>
	List<typename std::result_of<Func>::type> map(Func func)
	{
		List<typename std::result_of<Func>::type> result;
		result.reserve(this->size());
		
		for (const T &item : *this)
			result << func(item);
		
		return result;
	}
	
	template <typename Predicate>
	int foundIndex(Predicate pred) const
	{
		int count = 0;
		for (const T &item : *this)
		{
			if (pred(item))
				return count;
			
			count++;
		}
		return -1;
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

