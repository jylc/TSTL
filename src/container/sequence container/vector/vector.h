#pragma once
#ifndef _VECTOR_H_
#define _VECTOR_H_
#include "tstl_alloc.h"
#include  <algorithm>
#include <memory>
#include "algobase.h"
#include "tstl_construct.h"

namespace TSTL
{
	template<typename T, typename Alloc = allocator<T>>
	class vector
	{
	public:
		typedef T value_type;
		typedef T* pointer;
		typedef T* iterator;
		typedef T& reference;
		typedef size_t size_type;
		typedef ptrdiff_t difference_type;

	protected:
		typedef simple_alloc<value_type, Alloc> data_allocator;
		iterator start;
		iterator finish;
		iterator end_of_storage;

		void fill_initialize(size_type n, const value_type& v)
		{
			start = allocate_and_fill(n, v);
			finish = start + n;
			end_of_storage = finish;
		}

		iterator allocate_and_fill(size_type n, const value_type& v)
		{
			iterator result = data_allocator::allocate(n);
			iterator cur = result;
			for (; n > 0; --n, ++cur)
			{
				construct(&*cur, v);
			}
			return result;
		}

		void deallocate()
		{
			if (start)
			{
				data_allocator::deallocate(start);
			}
		}

		void insert_aux(iterator pos, const value_type& v)
		{
			if (finish != end_of_storage)
			{
				construct(finish, *(finish - 1));
				++finish;
				value_type v_copy = v;
				std::copy_backward(pos, finish - 2, finish - 1);
				*pos = v_copy;
			}
			else
			{
				difference_type old_size = size();
				const size_type len = old_size != 0 ? 2 * old_size : 1;

				iterator new_start = data_allocator::allocate(len);
				iterator new_finish = new_start;

				try {
					new_finish = std::uninitialized_copy(start, pos, new_start);
					construct(new_finish, v);
					++new_finish;
				}
				catch (std::exception ex)
				{
					destroy(new_start, new_finish);
					data_allocator::deallocate(new_start, len);
					throw;
				}
				destroy(begin(), end());
				deallocate();
				start = new_start;
				finish = new_finish;
				end_of_storage = start + len;
			}
		}
	public:
		iterator begin() { return start; }
		iterator end() { return finish; }
		size_type size()const { return size_type(int(finish - start)); }
		size_type capacity()const { return size_type(int(end_of_storage - start)); }
		bool empty()const { return begin() == end(); }
		reference operator[](size_type n) { return *(begin() + n); }
		reference front() { return *begin(); }
		reference bakc() { return *end(); }

		vector() :start(0), finish(0), end_of_storage(0) {}
		vector(size_type n, const value_type& v) { fill_initialize(n, v); }
		vector(int n, const value_type& v) { fill_initialize(n, v); }
		vector(long n, const value_type& v) { fill_initialize(n, v); }
		explicit vector(size_type n) { fill_initialize(n, T()); }

		~vector()
		{
			destroy(start, finish);//先调用析构函数，释放掉对象控制的资源
			deallcate();//再释放对象占用的空间
		}

		void pop_back()
		{
			--finish;
			destroy(finish);
		}

		void push_back(const value_type& v)
		{
			if (finish != end_of_storage)
			{
				construct(finish, v);
				++finish;
			}
			else {
				insert_aux(end(), v);
			}
		}

		iterator erase(iterator first, iterator last)
		{
			iterator i = std::copy(last, finish, first);
			destroy(i, finish);
			finish = finish - (last - first);
			return first;
		}

		iterator erase(iterator pos)
		{
			if (pos + 1 != end())
			{
				copy(pos + 1, finish, pos);
			}
			--finish;
			destroy(finish);
			return pos;
		}

		//再pos位置插入n个元素，元素初始值为v
		void insert(iterator pos, size_type n, const T& v)
		{
			if (n != 0)
			{
				if (size_type(end_of_storage - finish) > n)
				{
					T v_copy = v;
					const size_type elems_after = finish - pos;
					iterator old_finish = finish;
					if (elems_after > 0)
					{
						uninitialized_copy(finish - n, finish, finish);
						finish += n;
						copy_backward(pos, old_finish - n, old_finish);
						fill(pos, pos + n, v_copy);
					}
					else {
						const size_type old_size = size();
						const size_type len = old_size + max(old_size, n);
						iterator new_start = data_allocator::allocate(len);
						iterator new_finish = new_start;
						new_finish = uninitialized_copy(start, pos, new_start);
						new_finish = uninitialized_fill_n(new_finish, n, x);
						new_finish = uninitialized_copy(pos, finish, new_finish);
						destroy(start, finish);
						deallocate();
						start = new_start;
						finish = new_finish;
						end_of_storage = new_start + len;
					}
				}
			}
		}

		void swap(vector<T, Alloc>& x)
		{
			std::swap(start, x.start);
			std::swap(finish, x.finish);
			std::swap(end_of_storage, x.end_of_storage);
		}

		inline void make_heap(iterator first, iterator last)
		{
			if ((last - first) < 2)return;
			difference_type len = last - first;
			difference_type parent = len / 2;
			while (true)
			{
				_adjust_heap(first, parent, len, T(*(first + parent));
				if (parent == 0)return;
				--parent;
			}
		}

		inline void push_head(iterator first, iterator last)
		{
			difference_type holeIndex = last - first - 1;
			difference_type topIndex = 0;
			T value = T(*(last - 1));
			_push_heap(first, holeIndex, topIndex, value);
		}

		inline void pop_head(iterator first, iterator last)
		{
			_pop_head(first, last - 1, last - 1, T(*(last - 1)));
		}

		inline void sort_heap(iterator first, iterator last)
		{
			while (last - first > 1)
			{
				pop_head(first, last--);
			}
		}
	protected:
		void _pop_head(iterator first, iterator last, iterator result, T value)
		{
			*result = *first;
			_adjust_heap(first, difference_type(0), difference_type(last - first), value);
		}

		void _adjust_heap(iterator first, difference_type holdIndex, difference_type len, T value)
		{
			difference_type topIndex = holdIndex;
			difference_type secondChild = 2 * holdIndex + 2;
			while (secondChild < len)
			{
				if (*(first + secondChild) < *(first + secondChild - 1))
				{
					--secondChild;
				}
				*(first + holdIndex) = *(first + secondChild);
				holdIndex = secondChild;
				secondChild = 2 * (secondChild + 1);
			}

			if (secondChild == len)
			{
				*(first = holdIndex) = *(first + (secondChild - 1));
				holdIndex = secondChild - 1;
			}
			_push_heap(first, holdIndex, topIndex, value);
		}

		void _push_heap(iterator first, difference_type holdIndex, difference_type topIndex, T value)
		{
			difference_type parent = (holdIndex - 1) / 2;
			while (holdIndex > topIndex && *(first + parent) > value)
			{
				*(first + holdIndex) = *(first + parent);
				holdIndex = parent;
				parent = (holdIndex - 1) / 2;
			}
			*(first + holdIndex) = value;
		}

	};
}

#endif