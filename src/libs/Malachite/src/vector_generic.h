#pragma once

#include <boost/preprocessor/comma.hpp>
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

#define ML_IMPL_VECTOR_OPERATORS_GLOBAL(PREFIX, SELF_TYPE, VALUE_TYPE) \
	PREFIX SELF_TYPE operator+(const SELF_TYPE &v, VALUE_TYPE s) { return v + SELF_TYPE(s); } \
	PREFIX SELF_TYPE operator-(const SELF_TYPE &v, VALUE_TYPE s) { return v - SELF_TYPE(s); } \
	PREFIX SELF_TYPE operator*(const SELF_TYPE &v, VALUE_TYPE s) { return v * SELF_TYPE(s); } \
	PREFIX SELF_TYPE operator/(const SELF_TYPE &v, VALUE_TYPE s) { return v / SELF_TYPE(s); } \
	PREFIX SELF_TYPE operator+(VALUE_TYPE s, const SELF_TYPE &v) { return SELF_TYPE(s) + v; } \
	PREFIX SELF_TYPE operator-(VALUE_TYPE s, const SELF_TYPE &v) { return SELF_TYPE(s) - v; } \
	PREFIX SELF_TYPE operator*(VALUE_TYPE s, const SELF_TYPE &v) { return SELF_TYPE(s) * v; } \
	PREFIX SELF_TYPE operator/(VALUE_TYPE s, const SELF_TYPE &v) { return SELF_TYPE(s) / v; }

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
	
	Vector() = default;
	
	Vector(T s)
	{
		std::fill(begin(), end(), s);
	}
	
	Vector(const std::array<T, N> &array) : _array(array) {}
	
	Vector(std::initializer_list<T> list)
	{
		Q_ASSERT(list.size() == size());
		std::copy(list.begin(), list.end(), begin());
	}
	
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
		std::transform(v1.begin(), v1.end(), v2.begin(), r.begin(), op);
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
		std::transform(v1.begin(), v1.end(), v2.begin(), r.begin(), cmp);
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

ML_IMPL_VECTOR_OPERATORS_GLOBAL(template <typename T BOOST_PP_COMMA() size_t N>, Vector<T BOOST_PP_COMMA() N>, T)

}

