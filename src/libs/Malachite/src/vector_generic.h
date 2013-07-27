#pragma once

#include <initializer_list>
#include <array>
#include <cstddef>
#include <algorithm>

#define ML_IMPL_VECTOR_OPERATORS(SELF_TYPE, VALUE_TYPE) \
	SELF_TYPE &operator+=( const SELF_TYPE &other ) { return *this = *this + other; } \
	SELF_TYPE &operator-=( const SELF_TYPE &other ) { return *this = *this - other; } \
	SELF_TYPE &operator*=( const SELF_TYPE &other ) { return *this = *this * other; } \
	SELF_TYPE &operator/=( const SELF_TYPE &other ) { return *this = *this / other; } \
	SELF_TYPE &operator+=( VALUE_TYPE s ) { return operator+=(SELF_TYPE(s)); } \
	SELF_TYPE &operator-=( VALUE_TYPE s ) { return operator-=(SELF_TYPE(s)); } \
	SELF_TYPE &operator*=( VALUE_TYPE s ) { return operator*=(SELF_TYPE(s)); } \
	SELF_TYPE &operator/=( VALUE_TYPE s ) { return operator/=(SELF_TYPE(s)); } \
	bool operator!=( const SELF_TYPE &other ) const { return !(*this == other); }

#define ML_IMPL_VECTOR_OPERATORS_GLOBAL(RETURN_TYPE, SELF_TYPE, VALUE_TYPE) \
	RETURN_TYPE operator+(const SELF_TYPE &v, VALUE_TYPE s) { return v + SELF_TYPE(s); } \
	RETURN_TYPE operator-(const SELF_TYPE &v, VALUE_TYPE s) { return v - SELF_TYPE(s); } \
	RETURN_TYPE operator*(const SELF_TYPE &v, VALUE_TYPE s) { return v * SELF_TYPE(s); } \
	RETURN_TYPE operator/(const SELF_TYPE &v, VALUE_TYPE s) { return v / SELF_TYPE(s); } \
	RETURN_TYPE operator+(VALUE_TYPE s, const SELF_TYPE &v) { return SELF_TYPE(s) + v; } \
	RETURN_TYPE operator-(VALUE_TYPE s, const SELF_TYPE &v) { return SELF_TYPE(s) - v; } \
	RETURN_TYPE operator*(VALUE_TYPE s, const SELF_TYPE &v) { return SELF_TYPE(s) * v; } \
	RETURN_TYPE operator/(VALUE_TYPE s, const SELF_TYPE &v) { return SELF_TYPE(s) / v; }

namespace Malachite
{

template <class T, size_t N>
class Vector
{
public:
	
	typedef T value_type;
	typedef size_t size_type;
	typedef typename std::array<T, N>::iterator iterator;
	typedef typename std::array<T, N>::const_iterator const_iterator;
	
	typedef value_type ValueType;
	typedef size_type SizeType;
	typedef iterator Iterator;
	typedef const_iterator ConstIterator;
	
	Vector() {}
	
	Vector(T s)
	{
		for (auto iter = _array.begin(); iter != _array.end(); ++iter)
			*iter = s;
	}
	
	Vector(std::array<T, N> &array) { _array = array; }
	
	Vector(std::initializer_list<T> list)
	{
		if (list.size() != size())
			return;
		std::copy(list.begin(), list.end(), _array.begin());
	}
	
	Vector(const Vector &other) { _array = other._array; }
	
	// attributes
	
	constexpr static size_t size() { return N; }
	
	// iterators
	
	iterator begin() { return _array.begin(); }
	const_iterator begin() const { return _array.begin(); }
	const_iterator cbegin() const { return _array.cbegin(); }
	
	iterator end() { return _array.end(); }
	const_iterator end() const { return _array.end(); }
	const_iterator cend() const { return _array.cend(); }
	
	// access
	
	Vector extract(size_t index) const { return Vector(_array[index]); }
	
	T &operator[](size_t index) { return _array[index]; }
	const T &operator[](size_t index) const { return  _array[index]; }
	
	T &at(size_t index) { return _array[index]; }
	const T &at(size_t index) const { return  _array[index]; }
	
	// arithmetic
	
	template <typename Operation>
	static Vector operate(const Vector &v1, const Vector &v2, Operation op)
	{
		Vector r;
		
		auto iter1 = v1.begin();
		auto iter2 = v2.begin();
		auto iterR = r.begin();
		
		while (iterR != r.end())
			*iterR++ = op(*iter1++, *iter2++);
		
		return r;
	}
	
	Vector operator+(const Vector &other) const
	{
		return operate(*this, other, [](T a, T b){ return a+b; });
	}
	
	Vector operator-(const Vector &other) const
	{
		return operate(*this, other, [](T a, T b){ return a-b; });
	}
	
	Vector operator*(const Vector &other) const
	{
		return operate(*this, other, [](T a, T b){ return a*b; });
	}

	Vector operator/(const Vector &other) const
	{
		return operate(*this, other, [](T a, T b){ return a/b; });
	}
	
	Vector bound(const Vector &lower, const Vector &upper) const
	{
		return minimum(maximum(lower, *this), upper);
	}
	
	static Vector minimum(const Vector &v1, const Vector &v2)
	{
		return operate(v1, v2, [](T a, T b){ return a < b ? a : b; });
	}
	
	static Vector maximum(const Vector &v1, const Vector &v2)
	{
		return operate(v1, v2, [](T a, T b){ return a > b ? a : b; });
	}
	
	// comparison
	
	template <typename Compare>
	static Vector<bool, N> compare(const Vector &v1, const Vector &v2, Compare cmp)
	{
		Vector<bool, N> r;
		
		auto iter1 = v1.begin();
		auto iter2 = v2.begin();
		auto iterR = r.begin();
		
		while (iterR != r.end())
			*iterR++ = cmp(*iter1++, *iter2++);
		
		return r;
	}
	
	static Vector<bool, N> lessThan(const Vector &v1, const Vector &v2)
	{
		return compare( v1, v2, [](T a, T b) { return a < b; } );
	}
	
	static Vector<bool, N> lessThanEqual(const Vector &v1, const Vector &v2)
	{
		return compare( v1, v2, [](T a, T b) { return a <= b; } );
	}
	
	static Vector<bool, N> greaterThan(const Vector &v1, const Vector &v2)
	{
		return compare( v1, v2, [](T a, T b) { return a > b; } );
	}
	
	static Vector<bool, N> greaterThanEqual(const Vector &v1, const Vector &v2)
	{
		return compare( v1, v2, [](T a, T b) { return a >= b; } );
	}
	
	static Vector<bool, N> equal(const Vector &v1, const Vector &v2)
	{
		return compare( v1, v2, [](T a, T b) { return a == b; } );
	}
	
	static Vector<bool, N> notEqual(const Vector &v1, const Vector &v2)
	{
		return compare( v1, v2, [](T a, T b) { return a != b; } );
	}
	
	static Vector choose(const Vector<bool, N> &selector, const Vector &vTrue, const Vector &vFalse)
	{
		Vector r;
		
		auto iterS = selector.begin();
		auto iterT = vTrue.begin();
		auto iterF = vFalse.begin();
		auto iterR = r.begin();
		
		while (iterS != selector.end())
			*iterR++ = *iterS++ ? *iterT++ : *iterF++;
		
		return r;
	}
	
	bool operator==(const Vector &other) const { return _array == other._array; }
	
	ML_IMPL_VECTOR_OPERATORS(Vector, ValueType)
	
private:
	
	std::array<T, N> _array;
};

template <typename T, size_t N> inline Vector<T, N> operator+(const Vector<T, N> &v, T s) { return v + SELF_TYPE(s); }
template <typename T, size_t N> inline Vector<T, N> operator-(const Vector<T, N> &v, T s) { return v - SELF_TYPE(s); }
template <typename T, size_t N> inline Vector<T, N> operator*(const Vector<T, N> &v, T s) { return v * SELF_TYPE(s); }
template <typename T, size_t N> inline Vector<T, N> operator/(const Vector<T, N> &v, T s) { return v / SELF_TYPE(s); }
template <typename T, size_t N> inline Vector<T, N> operator+(T s, const Vector<T, N> &v) { return SELF_TYPE(s) + v; }
template <typename T, size_t N> inline Vector<T, N> operator-(T s, const Vector<T, N> &v) { return SELF_TYPE(s) - v; }
template <typename T, size_t N> inline Vector<T, N> operator*(T s, const Vector<T, N> &v) { return SELF_TYPE(s) * v; }
template <typename T, size_t N> inline Vector<T, N> operator/(T s, const Vector<T, N> &v) { return SELF_TYPE(s) / v; }

}

