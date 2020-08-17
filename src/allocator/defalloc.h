#pragma once
#ifndef _DEFALLOC_H_
#define _DEFALLOC_H_
#include <new>
#include <climits>
#include <iostream>
#include "algobase.h"


namespace TSTL
{

	template<typename T>
	inline T* allocate(ptrdiff_t size, T*)
	{
		set_new_handler(0);
		T* tmp = (T*)(::operator new((size_t)size * sizeof T));
		if (tmp == 0)
		{
			std::cerr << "out of memory" << std::endl;
			exit(1);
		}

		return tmp;
	}

	template<typename T>
	inline void deallocate(T* buffer)
	{
		::operator delete(buffer);
	}

	template<typename T>
	class allocator
	{
	public:
		typedef T value_type;
		typedef T* pointer;
		typedef const T* const_pointer;
		typedef T& reference;
		typedef const T& const_reference;
		typedef size_t size_type;
		typedef ptrdiff_t difference_type;

		pointer allocate(size_type size)
		{
			return ::allocate((difference_type)size, (pointer)0);
		}

		void deallocate(pointer p)
		{
			::deallocate(p);
		}
		pointer address(reference r)
		{
			return static_cast<pointer>(&r);
		}

		const_pointer const_address(const_reference r)
		{
			return static_cast<const_pointer>(&r);
		}

		/*
		* 初始化页大小
		* T大小大于4096，页大小置为1
		*/
		size_type init_page_size()
		{
			return max(size_type(1), size_type(4096 / sizeof T));
		}

		size_type max_size() const
		{
			return max(size_type(1), size_type(UINT_MAX / sizeof T));
		}
	};

	class allocator<void>
	{
	public:
		typedef void* pointer;
	};
}


#endif // !_DEFALLOC_H_
