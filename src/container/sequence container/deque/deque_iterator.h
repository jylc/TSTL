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
		typedef pointer* map_pointer;		//��������
		typedef __deque_iterator self;
		typedef __deque_iterator<T, T&, T*, BUFSIZ> iterator;


		T* cur;				//ָ��ǰλ��
		T* first;			//ָ�򻺳���ͷ��
		T* last;			//ָ�򻺳���β��
		map_pointer node;	//����������������

		/*
		* �����µĻ�����λ��
		*/

		void set_node(map_pointer new_node)
		{
			node = new_node;
			first = *node;
			last = first + difference_type(buffer_size());
		}

		/*
		*��������СĬ��Ϊ512�ֽ�
		*1.BufSiz��Ϊ0����ʾbuffer_size ���û����壬����BufSiz
		*2.BufSizΪ0����ʾbuffer_size ʹ��Ĭ��ֵ�����Ԫ�ش�СС��512�ֽڣ�����size_t(512/sizeof(T))��
		* ���򷵻�size_t(1)
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
			++cur;//��һ��
			if (cur == last)//���ߵ����������л���������ǰ�պ�ԭ�򣬻����������һ��ȡ����
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
			--cur;//ǰ�պ�ԭ�����һ�����ܴ�ֵ
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
			//�����first��ƫ��
			difference_type offset = n + (cur - first);
			if (offset >= 0 && offset < difference_type(buffer_size))
			{
				cur += n;
			}
			else
			{
				difference_type node_offset = offset > 0 ? offset / difference_type(buffer_size) : -difference_type((-offset - 1) / buffer_size() - 1);
				set_node(node + node_offset);
				//cur=��ǰ������+offset-�ѷ���Ļ�������С
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