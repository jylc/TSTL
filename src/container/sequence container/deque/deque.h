#pragma once
#ifndef _DEQUE_H_
#define _DEQUE_H_
#include "deque_iterator.h"
#include "tstl_alloc.h"
#include  <algorithm>
#include "tstl_construct.h"
#include "algobase.h"
namespace TSTL
{
	template<typename T, typename Alloc = alloc, size_t BufSiz = 0>
	class deque
	{
	public:
		typedef T value_type;
		typedef value_type* pointer;
		typedef value_type& reference;
		typedef size_t size_type;
		typedef ptrdiff_t difference_type;
		typedef typename __deque_iterator<T, T&, T*, BufSiz>::iterator iterator;

	protected:
		typedef pointer* map_pointer;

	protected:
		iterator start;
		iterator finish;
		map_pointer map;
		size_type map_size;

	protected:
		difference_type buffer_size()
		{
			return BufSiz != 0 ? BufSiz : (sizeof(T) < 512 ? size_t(512 / sizeof(T)) : size_t(1));
		}

	protected:
		typedef simple_alloc<value_type, Alloc> data_allocator;//deque�ڵ�Ŀռ�������
		typedef simple_alloc<pointer, Alloc> map_allocator;//map�Ŀռ�������
	public:
		//���캯��
		deque(int n, const value_type& value) :start(), finish(), map(0), map_size(0)
		{
			fill_initialize(n, value);
		}

		deque() :start(), finish(), map(0), map_size(0)
		{
			fill_initialize(0, 0);
		}

	protected:
		void fill_initialize(size_type n, const value_type& value)
		{
			create_node_and_map(n);
			map_pointer cur;
			for (cur = start.node; cur < finish.node; ++cur)
			{
				std::uninitialized_fill(*cur, *cur + buffer_size(), value);
			}
			std::uninitialized_fill(finish.first, finish.cur, value);
		}

		void create_node_and_map(size_type elem_nums)
		{
			//�ڵ����=Ԫ�ظ���/ÿ��������������Ԫ�صĸ���+1
			size_type node_nums = elem_nums / difference_type(buffer_size()) + 1;
			size_type map_size = std::max(init_map_size(), node_nums + 2);
			map = map_allocator::allocate(map_size);
			map_pointer nstart = map + (map_size - node_nums) / 2;
			map_pointer nfinish = nstart + node_nums - 1;

			map_pointer cur;
			for (cur = nstart; cur <= nfinish; ++cur)
			{
				*cur = allocate_node();
			}

			start.set_node(nstart);//�ν�start������
			finish.set_node(nfinish);//�ν�finish������
			start.cur = start.first;//����start���������α�
			finish.cur = finish.first + elem_nums % buffer_size();//����finish���������α�
		}

		//��ʼ��map�Ĵ�С��map��С��СΪ8
		size_type init_map_size()
		{
			return (size_type)8;
		}

		pointer allocate_node()
		{
			return data_allocator::allocate(buffer_size() / sizeof(T));
		}

		void deallocate_node(void* node)
		{
			data_allocator::deallocate(node);
		}
	public:

		iterator begin() { return start; }
		iterator end() { return finish; }
		reference operator[](size_type n) { return start[difference_type(n)]; }
		reference front() { return *start; }
		reference back()
		{
			iterator tmp = finish;
			--tmp;
			return tmp;
		}
		size_type size() const { return finish - start; }
		size_type max_size()const { return finish - start; }
		bool empty()const { return finish == start; }


	public:
		void push_back(const value_type& t)
		{
			if (finish.cur == finish.last - 1)
			{
				construct(finish.cur, t);
				++finish.cur;
			}
			else
			{
				//��������
				push_back_aux(t);
			}
		}


		void push_front(const value_type& t)
		{
			if (start.cur != start.first)
			{
				construct(start.cur - 1, t);
				--start.cur;
			}
			else
			{
				push_front_aux(t);
			}
		}

		void pop_back()
		{
			if (finish.cur != finish.first)
			{
				--finish.cur;
				destroy(finish.cur);
			}
			else
			{
				pop_back_aux();
			}
		}

		void pop_front()
		{
			if (start.cur != start.last - 1)
			{
				destroy(start.cur);
				++start.cur;
			}
			else
			{
				pop_front_aux();
			}

		}

		//�������Ԫ��
		void clear()
		{
			for (map_pointer node = start.node + 1; node < finish.node; ++node)
			{
				destroy(*node, *node + buffer_size());
				data_allocator::deallocate(*node, buffer_size());
			}

			if (start.node != finish.node)
			{
				destroy(start.cur, start.last);
				destroy(finish.start, finish.cur);
			}
			else
			{
				destroy(start.cur, finish.cur);
			}
			finish = start;
		}

		iterator insert(iterator pos, const value_type& v)
		{
			if (pos.cur == start.cur)
			{
				push_front(v);
				return start;
			}
			else if (pos.cur == finish.cur)
			{
				push_back(v);
				return finish;
			}
			else
			{
				return insert_aux(pos, v);
			}
		}

	protected:
		void push_back_aux(const value_type& x)
		{
			value_type t_copy = x;
			*(finish.node + 1) = allocate_node();
			construct(finish.cur, t_copy);
			finish.set_node(finish.node + 1);
			finish.cur = finish.start;
		}

		void push_front_aux(const value_type& x)
		{
			value_type t_copy = x;
			*(start.node - 1) = allocate_node();
			start.set_node(start.node - 1);
			start.cur = start.last - 1;
			construct(start.cru, t_copy);
		}

		void pop_back_aux()
		{
			deallocate_node(finish.first);
			finish.set_node(finish.node - 1);
			finish.cur = finish.last - 1;
			destroy(finish.cur);
		}

		void pop_front_aux()
		{
			destroy(start.cur);
			deallocate_node(start.first);
			start.set_node(start.node + 1);
			start.cur = start.first;
		}

		iterator insert_aux(iterator pos, const value_type& v)
		{
			difference_type index = pos - start;
			value_type v_copy = v;
			//�����볤��С���ܳ���һ�룬��ǰ��
			if (index < size() / 2)
			{
				//��Ԫ��ǰ��һ����λ
				push_front(front());//start.curǰ����һ����λ
				iterator front1 = start;//�ȸ�������������ֹ�ı�start
				++front1;//������ָ��ԭ�ȵ�startλ��
				iterator front2 = front1;
				++front2;
				pos = start + index;
				iterator pos1 = pos;
				++pos1;
				copy(front2, pos1, front1);
			}
			else
			{
				push_back(back());
				iterator back1 = finish;
				--back1;
				iterator back2 = back1;
				--back2;
				pos = start + index;
				iterator pos1 = pos;
				copy(back2, pos, back1);
			}

			*pos = v_copy;
			return pos;
		}

	};

}

#endif // ! _DEQUE_H_
