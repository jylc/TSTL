#pragma once
#ifndef _QUEUE_H_
#define _QUEUE_H_
#include "../deque/deque.h"
namespace TSTL 
{
	template<typename T,typename Sequence=deque<T>>
	class queue
	{
	public:
		typedef typename Sequence::value_type value_type;
		typedef typename Sequence::reference reference;
		typedef typename Sequence::size_type size_type;

	protected:
		Sequence c;

	public:
		queue():c(){}
		bool empty()const { return c.empty(); }
		size_type size()const { return c.size(); }
		reference front() { return c.front(); }
		reference back() { return c.back(); }
		void push(const value_type& v) { c.push_back(v); }
		void pop() { c.pop_front(); }
	};
}
#endif // !_QUEUE_H_
