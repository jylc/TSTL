#pragma once
#ifndef _STACK_H_
#define _STACK_H_
#include "../deque/deque.h"

namespace TSTL
{
	template<typename T, typename Sequence = deque<T>>
	class stack
	{
	public:
		typedef typename Sequence::value_type value_type;
		typedef typename Sequence::size_type size_type;
		typedef typename Sequence::reference reference;
	protected:
		Sequence c;
	public:
		stack() :c() {}
		void push(const value_type& value) { c.push_back(); }
		void pop() { c.pop_back(); }
		size_type size()const { return c.size(); }
		bool empty()const { return c.empty(); }
		reference top() { return c.back(); }

	};
}

#endif // !_STACK_H_
