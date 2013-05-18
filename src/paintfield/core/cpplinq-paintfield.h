#pragma once

#include "cpplinq.hpp"

namespace cpplinq
{

namespace detail
{

template <typename TContainer>
struct to_any_container_builder : base_builder
{
	typedef to_any_container_builder this_type;
	size_t capacity;
	
	explicit to_any_container_builder(size_t capacity = 16U) throw () :
		capacity(capacity)
	{}
	
	to_any_container_builder(const to_any_container_builder &v) throw () :
		capacity(v.capacity)
	{}
	
	to_any_container_builder(to_any_container_builder &&v) throw () :
		capacity(std::move(v.capacity))
	{}
	
	template <typename TRange>
	TContainer build(TRange range)
	{
		TContainer result;
		result.reserve(capacity);
		
		while (range.next())
			result.push_back(range.front());
		
		return std::move(result);
	}
};

}

template <typename TContainer>
inline detail::to_any_container_builder<TContainer> to_any_container(size_t capacity = 16U)
{
	return detail::to_any_container_builder<TContainer>(capacity);
}

}

