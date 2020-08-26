#pragma once
#ifndef _DEQUE_ITERATOR_H_
#define _DEQUE_ITERATOR_H_
#include "tstl_iterator_base.h"

namespace TSTL
{
	template<typename T, typename Ref, typename Ptr, size_t BufSiz>
	class __deque_iterator :public random_access_iterator<T, ptrdiff_t>
	{
	public:
		typedef random_access_iterator_tag iterator_category;
		typedef T value_type;
		typedef Ptr pointer;
		typedef Ref reference;
		typedef size_t size_type;
		typedef ptrdiff_t difference_type;
		typedef pointer* map_pointer;		//管理缓冲区
		typedef __deque_iterator self;
		typedef __deque_iterator<T, T&, T*, BUFSIZ> iterator;


		T* cur;				//指向当前位置
		T* first;			//指向缓冲区头部
		T* last;			//指向缓冲区尾部
		map_pointer node;	//迭代器所属缓冲区

		/*
		* 设置新的缓冲区位置
		*/

		void set_node(map_pointer new_node)
		{
			node = new_node;
			first = *node;
			last = first + difference_type(buffer_size());
		}

		/*
		*缓冲区大小默认为512字节
		*1.BufSiz不为0，表示buffer_size 由用户定义，返回BufSiz
		*2.BufSiz为0，表示buffer_size 使用默认值，如果元素大小小于512字节，返回size_t(512/sizeof(T))，
		* 否则返回size_t(1)
		*/
		size_t buffer_size()
		{
			return BufSiz != 0 ? BufSiz : (size_t(T) < size_t(512) ? size_t(512 / size_t(T)) : size_t(1));
		}

		reference operator*()const
		{
			return *cur;
		}

		pointer operator->()const
		{
			return &(operator*());
		}

		self& operator++()
		{
			++cur;//下一个
			if (cur == last)//若走到缓冲区，切换缓冲区；前闭后开原则，缓冲区的最后一个取不到
			{
				set_node(node + 1);
				cur = first;
			}
			return *this;
		}

		self& operator++(int)
		{
			self tmp = *this;
			++* this;
			return tmp;
		}

		self& operator--()
		{
			if (cur == first)
			{
				set_node(node - 1);
				cur = last;
			}
			--cur;//前闭后开原则，最后一个不能存值
			return *this;
		}
		self& operator++(int)
		{
			self tmp = *this;
			--* this;
			return tmp;
		}

		self& operator+=(difference_type n)
		{
			//相对于first的偏移
			difference_type offset = n + (cur - first);
			if (offset >= 0 && offset < difference_type(buffer_size))
			{
				cur += n;
			}
			else
			{
				difference_type node_offset = offset > 0 ? offset / difference_type(buffer_size) : -difference_type((-offset - 1) / buffer_size() - 1);
				set_node(node + node_offset);
				//cur=当前缓冲区+offset-已分配的缓冲区大小
				cur = first + (offset - node_offset * difference_type(buffer_size()));
			}
			return *this;
		}

		self& operator+(difference_type n)
		{
			self tmp = *this;
			return tmp += n;
		}

		self& operator-=(difference_type n)
		{
			return *this += -n;
		}

		self& operator-(difference_type n)
		{
			self tmp = *this;
			return tmp -= n;
		}

		difference_type operator-(const self& x)
		{
			return difference_type(buffer_size()) * (node - x.node - 1) + (cur - first) + (x.cur - x.first);
		}

		reference operator[](difference_type n)const { return *(*this + n); }

		bool operator==(const self& x)const { return cur == x.cur; }
		bool operator!=(const self& x)const { return cur != x.cur; }
		bool operator<(const self& x)const
		{
			return (node == x.node) ? (cur < x.cur) : (node < x.node);
		}
	};
}


#endif