#pragma once
#ifndef _ALGO_H_
#define _ALGO_H_
#include "algobase.h"
#ifndef __STL_CLASS_PARTIAL_SPECIALIZATION
#define __STL_CLASS_PARTIAL_SPECIALIZATION
#include "tstl_iterator_base.h"
#endif // !__STL_CLASS_PARTIAL_SPECIALIZATION


namespace TSTL
{
	//寻找满足条件的第一组相邻元素
	//第一组满足相邻两个元素相同
	//第二组满足用户提供的二元运算
	template<typename ForwardIterator>
	ForwardIterator adjacent_find(ForwardIterator __first, ForwardIterator __last)
	{
		if (__first == __last)
			return __last;
		ForwardIterator __next = __first;
		while (++__next != __last)
		{
			if (*__first == *__next)
				return __first;
			__first = __next;
		}
		return __last;
	}

	template<typename ForwardIterator, typename BinaryPredicate>
	ForwardIterator adjacent_find(ForwardIterator __first, ForwardIterator __last, BinaryPredicate __binary_pred)
	{
		if (__first == __last)
			return __last;
		ForwardIterator __next = __first;
		while (++__next != __last)
		{
			if (__binary_pred(*__first, *__next))
				return __first;
			__first = __next;
		}
		return __last;
	}

	//count
	template<typename InputIterator, typename _T>
	typename iterator_traits<InputIterator>::difference_type
		count(InputIterator __first, InputIterator __last, const _T& __value)
	{
		typename iterator_traits<InputIterator>::difference_type n = 0;
		while (__first != __last)
		{
			if (*__first == __value)
				++n;
			++__first;
		}
		return n;
	}
	//输出满足使得用户提供的函数为true的元素个数
	template<typename InputIterator, typename predicate>
	typename iterator_traits<InputIterator>::difference_type
		count_if(InputIterator __first, InputIterator __last, predicate pred)
	{
		typename iterator_traits<InputIterator>::difference_type n = 0;
		while (__first != __last)
		{
			if (pred(*__first))
				++n;
			++__first;
		}
		return n;
	}

	//find
	template<typename InputIterator, typename _T>
	InputIterator find(InputIterator __first, InputIterator __last, const _T& __value)
	{
		while (__first != __last && *__first != __value)
		{
			++__first;
		}
		return __first;
	}

	template<typename InputIterator, typename predicate>
	InputIterator find_if(InputIterator __first, InputIterator __last, predicate pred)
	{
		while (__first != __last && !pred(*__first))
		{
			++__first;
		}
		return __first;
	}

	template<typename InputIterator, typename function>
	function for_each(InputIterator __first, InputIterator __last, function f)
	{
		while (__first != __last)
		{
			f(*__first);
			++__first;
		}
		return f;
	}

	template<typename InputIterator, typename Generator>
	void generate(InputIterator __first, InputIterator __last, Generator gen)
	{
		while (__first != __last)
		{
			*__first = gen();
			++__first;
		}
	}

	template<typename InputIterator, typename size, typename Generator>
	void generator_n(InputIterator __first, size n, Generator gen)
	{
		for (; n > 0; --n, ++__first)
		{
			*__first = gen();
		}
	}
	template<typename InputIterator, typename OutputIterator, typename UnaryOperation>
	OutputIterator transform(InputIterator __first, InputIterator __last, OutputIterator __result, UnaryOperation op)
	{
		for (; __first != __last; ++__first)
		{
			*__result = op(*__first);
		}
		return __result;
	}

	template<typename InputIterator, typename OutputIterator, typename UnaryOperation>
	OutputIterator transform(InputIterator __first1, InputIterator __last, InputIterator __first2, OutputIterator __result, UnaryOperation op)
	{
		for (; __first1 != __last; ++__first1, ++__first2, ++__result)
		{
			*__result = op(*__first1, *__first2);
		}
		return __result;
	}
}

#endif // !_ALGO_H_
