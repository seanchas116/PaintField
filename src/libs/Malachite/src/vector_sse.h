#pragma once

//ExportName: Vector

#include <stdint.h>
#include <emmintrin.h>
#include <new>
#include "vector_generic.h"

#if !__x86_64__

__attribute__((__weak__))
void *operator new(std::size_t size) throw(std::bad_alloc)
{
	if (size == 0)
		size = 1;
	
	void *p;
	while (posix_memalign(&p, 16, size))
	{
		auto nh = std::set_new_handler(nullptr);
		std::set_new_handler(nh);
		if (nh)
			nh();
		else
			throw std::bad_alloc();
	}
	return p;
}

__attribute__((__weak__))
void *operator new(std::size_t size, const std::nothrow_t &) noexcept
{
	void *p = 0;
	try
	{
		p = ::operator new(size);
	}
	catch (...)
	{
	}
	return p;
}

__attribute__((__weak__))
void *operator new[](std::size_t size) throw(std::bad_alloc)
{
	return ::operator new(size);
}

__attribute__((__weak__))
void *operator new[](std::size_t size, const std::nothrow_t&) noexcept
{
	void *p = 0;
	try
	{
		p = ::operator new[](size);
	}
	catch (...)
	{
	}
	return p;
}

#endif

namespace Malachite
{

template<>
class Vector<double, 2>
{
public:
	
	typedef double value_type;
	typedef size_t size_type;
	typedef typename std::array<double, 2>::iterator iterator;
	typedef typename std::array<double, 2>::const_iterator const_iterator;
	
	typedef value_type ValueType;
	typedef size_type SizeType;
	typedef iterator Iterator;
	typedef const_iterator ConstIterator;
	
	Vector() {}
	
	Vector(ValueType s)
	{
		_array[0] = s;
		_data = _mm_unpacklo_pd(_data, _data);
	}
	
	Vector(std::array<ValueType, 2> &array)
	{
		_array = array;
	}
	
	Vector(std::initializer_list<ValueType> list)
	{
		if (list.size() != size())
			return;
		std::copy(list.begin(), list.end(), _array.begin());
	}
	
	Vector(__m128d data) { _data = data; }
	
	Vector(const Vector<ValueType, 2> &other) { _v = other._v; }
	
	// attributes
	
	static constexpr size_t size() { return 2; }
	
	// access
	
	Vector extract(size_t index) const
	{
		switch (index)
		{
			default:
			case 0:
				return _mm_unpacklo_pd(_data, _data);
			case 1:
				return _mm_unpackhi_pd(_data, _data);
		}
	}
	
	ValueType &operator[](size_t index) { return _array[index]; }
	const ValueType &operator[](size_t index) const { return _array[index]; }
	
	ValueType &at(size_t index) { return _array[index]; }
	const ValueType &at(size_t index) const { return _array[index]; }
	
	// arithmetic
	
	const Vector operator+(const Vector &other) const { Vector r; r._v = _v + other._v; return r; }
	const Vector operator-(const Vector &other) const { Vector r; r._v = _v - other._v; return r; }
	const Vector operator*(const Vector &other) const { Vector r; r._v = _v * other._v; return r; }
	const Vector operator/(const Vector &other) const { Vector r; r._v = _v / other._v; return r; }
	
	Vector sqrt() const { return _mm_sqrt_pd(*this); }
	
	Vector bound(const Vector &lower, const Vector &upper) const
	{
		return minimum(maximum(*this, lower), upper);
	}
	
	static Vector minimum(const Vector &v1, const Vector &v2)
	{
		return _mm_min_pd(v1, v2);
	}
	
	static Vector maximum(const Vector &v1, const Vector &v2)
	{
		return _mm_max_pd(v1, v2);
	}
	
	// comparison
	
	class ComparisonResult
	{
	public:
		
		ComparisonResult(__m128d data) { _data = data; }
		
		uint64_t &operator[](size_t index) { return _array[index]; }
		const uint64_t &operator[](size_t index) const { return  _array[index]; }
		
		operator __m128d() const { return _data; }
		
	private:
		
		union
		{
			__m128d _data;
			std::array<uint64_t, 2> _array;
		};
	};
	
	static ComparisonResult lessThan(const Vector &v1, const Vector &v2)
	{
		return _mm_cmplt_pd(v1, v2);
	}
	
	static ComparisonResult lessThanEqual(const Vector &v1, const Vector &v2)
	{
		return _mm_cmple_pd(v1, v2);
	}
	
	static ComparisonResult greaterThan(const Vector &v1, const Vector &v2)
	{
		return _mm_cmpgt_pd(v1, v2);
	}
	
	static ComparisonResult greaterThanEqual(const Vector &v1, const Vector &v2)
	{
		return _mm_cmpge_pd(v1, v2);
	}
	
	static ComparisonResult equal(const Vector &v1, const Vector &v2)
	{
		return _mm_cmpeq_pd(v1, v2);
	}
	
	static ComparisonResult notEqual(const Vector &v1, const Vector &v2)
	{
		return _mm_cmpneq_pd(v1, v2);
	}
	
	static Vector choose(const ComparisonResult &selector, const Vector &vTrue, const Vector &vFalse)
	{
		return _mm_and_pd( _mm_and_pd(selector, vTrue), _mm_andnot_pd(selector, vFalse) );
	}
	
	bool operator==(const Vector &other) const { return _array == other._array; }
	
	// original
	
	operator __m128d() const { return _data; }
	
	ML_IMPL_VECTOR_OPERATORS(Vector, ValueType)
	
protected:
	
	union
	{
		__m128d _data;
		__v2df _v;
		std::array<double, 2> _array;
	};
};

typedef Vector<double, 2> Vector_double_2;

ML_IMPL_VECTOR_OPERATORS_GLOBAL(inline Vector_double_2, Vector_double_2, double)

template<>
class Vector<float, 4>
{
public:
	
	typedef float value_type;
	typedef size_t size_type;
	typedef typename std::array<float, 4>::iterator iterator;
	typedef typename std::array<float, 4>::const_iterator const_iterator;
	
	typedef value_type ValueType;
	typedef size_type SizeType;
	typedef iterator Iterator;
	typedef const_iterator ConstIterator;
	
	Vector() {}
	
	Vector(ValueType s)
	{
		_array[0] = s;
		_data = _mm_unpacklo_ps(_data, _data);
		_data = _mm_unpacklo_ps(_data, _data);
	}
	
	Vector(std::array<ValueType, 4> &array)
	{
		_array = array;
	}
	
	Vector(std::initializer_list<ValueType> list)
	{
		if (list.size() != size())
			return;
		std::copy(list.begin(), list.end(), _array.begin());
	}
	
	Vector(__m128 data) { _data = data; }
	
	Vector(const Vector<ValueType, 4> &other) { _v = other._v; }
	
	// attributes
	
	constexpr static size_t size() { return 4; }
	
	// access
	
	Vector extract(size_t index) const
	{
		switch (index)
		{
			default:
			case 0:
			{
				__m128 rd = _mm_unpacklo_ps(_data, _data);
				rd = _mm_unpacklo_ps(rd, rd);
				return rd;
			}
			case 1:
			{
				__m128 rd = _mm_unpacklo_ps(_data, _data);
				rd = _mm_unpackhi_ps(rd, rd);
				return rd;
			}
			case 2:
			{
				__m128 rd = _mm_unpackhi_ps(_data, _data);
				rd = _mm_unpacklo_ps(rd, rd);
				return rd;
			}
			case 3:
			{
				__m128 rd = _mm_unpackhi_ps(_data, _data);
				rd = _mm_unpackhi_ps(rd, rd);
				return rd;
			}
		}
	}
	
	ValueType &operator[](size_t index) { return _array[index]; }
	const ValueType &operator[](size_t index) const { return _array[index]; }
	
	ValueType &at(size_t index) { return _array[index]; }
	const ValueType &at(size_t index) const { return _array[index]; }
	
	// arithmetic
	
	const Vector operator+(const Vector &other) const { Vector r; r._v = _v + other._v; return r; }
	const Vector operator-(const Vector &other) const { Vector r; r._v = _v - other._v; return r; }
	const Vector operator*(const Vector &other) const { Vector r; r._v = _v * other._v; return r; }
	const Vector operator/(const Vector &other) const { Vector r; r._v = _v / other._v; return r; }
	
	Vector bound(const Vector &lower, const Vector &upper) const
	{
		return minimum(maximum(*this, lower), upper);
	}
	
	static Vector minimum(const Vector &v1, const Vector &v2)
	{
		return _mm_min_ps(v1, v2);
	}
	
	static Vector maximum(const Vector &v1, const Vector &v2)
	{
		return _mm_max_ps(v1, v2);
	}
	
	Vector sqrt() const
	{
		return _mm_sqrt_ps(*this);
	}
	
	Vector rsqrt() const
	{
		return _mm_rsqrt_ps(*this);
	}
	
	// comparison
	
	class ComparisonResult
	{
	public:
		
		ComparisonResult(__m128 data) { _data = data; }
		
		template <size_t T_Index> uint32_t &e() { return _array[T_Index]; }
		template <size_t T_Index> const uint32_t &e() const { return _array[T_Index]; }
		
		uint32_t &operator[](size_t index) { return _array[index]; }
		const uint32_t &operator[](size_t index) const { return  _array[index]; }
		
		operator __m128() const { return _data; }
		
		static ComparisonResult vectorAnd(const ComparisonResult &lhs, const ComparisonResult &rhs)
		{
			return _mm_and_ps(lhs, rhs);
		}
		
		static ComparisonResult vectorOr(const ComparisonResult &lhs, const ComparisonResult &rhs)
		{
			return _mm_or_ps(lhs, rhs);
		}
		
	private:
		
		union
		{
			__m128 _data;
			std::array<uint32_t, 4> _array;
		};
	};
	
	static ComparisonResult lessThan(const Vector &v1, const Vector &v2)
	{
		return _mm_cmplt_ps(v1, v2);
	}
	
	static ComparisonResult lessThanEqual(const Vector &v1, const Vector &v2)
	{
		return _mm_cmple_ps(v1, v2);
	}
	
	static ComparisonResult greaterThan(const Vector &v1, const Vector &v2)
	{
		return _mm_cmpgt_ps(v1, v2);
	}
	
	static ComparisonResult greaterThanEqual(const Vector &v1, const Vector &v2)
	{
		return _mm_cmpge_ps(v1, v2);
	}
	
	static ComparisonResult equal(const Vector &v1, const Vector &v2)
	{
		return _mm_cmpeq_ps(v1, v2);
	}
	
	static ComparisonResult notEqual(const Vector &v1, const Vector &v2)
	{
		return _mm_cmpneq_ps(v1, v2);
	}
	
	static Vector choose(const ComparisonResult &selector, const Vector &vTrue, const Vector &vFalse)
	{
		return _mm_or_ps( _mm_and_ps(selector, vTrue), _mm_andnot_ps(selector, vFalse) );
	}
	
	bool operator==(const Vector &other) const { return _array == other._array; }
	
	// original
	
	operator __m128() const { return _data; }
	
	ML_IMPL_VECTOR_OPERATORS(Vector, ValueType)
	
protected:
	
	union
	{
		__m128 _data;
		__v4sf _v;
		std::array<float, 4> _array;
	};
};

typedef Vector<float, 4> Vector_float_4;

ML_IMPL_VECTOR_OPERATORS_GLOBAL(inline Vector_float_4, Vector_float_4, float)

}
