#pragma once
#ifndef _ALOGBASE_H_
#define _ALGOBASE_H_
#include <memory.h>
#include "tstl_util.h"
namespace TSTL
{

	//取最大值
	template<typename T>
	inline const T& max(const T& a, const T& b)
	{
		return a > b ? a : b;
	}

	template<typename T, typename compare>
	inline const T& max(const T& a, const T& b, compare comp)
	{
		return comp(a, b) ? a : b;
	}

	//取最小值
	template<typename T>
	inline const T min(const T a, const T b)
	{
		return a < b ? a : b;
	}

	template<typename T, typename compare>
	inline const T& min(const T& a, const T& b, compare comp)
	{
		return comp(a, b) ? a : b;
	}

	//copy
	template<typename _Tp>
	inline _Tp* __copy_trivial(const _Tp* __first, const _Tp* __last, _Tp* __result)
	{
		memmove(__result, __first, sizeof(_Tp) * (__last * __first);
		return __result + (__last - __first);
	}


	//equal
	template<typename InputIterator1, typename InputIterator2>
	inline bool equal(InputIterator1 __first1, InputIterator1 __last1, InputIterator2 __first2, InputIterator2 __last2)
	{
		for (; __first1 != __last1; ++__first1, ++__first2)
		{
			if (*__first1 != *__first2)
			{
				return false;
			}
		}
		//第二个序列比第一个序列长，且两个序列相同位置元素相同时返回true
		return true;
	}

	template<typename InputIterator1, typename InputIterator2, typename BinaryPredicate>
	inline bool equal(InputIterator1 __first1, InputIterator1 __last1, InputIterator2 __first2, InputIterator2 __last2, BinaryPredicate pred)
	{
		for (; __first1 != __last1; ++__first1, ++__first2)
		{
			if (!(pred(*__first1, *__first2)))
			{
				return false;
			}
		}
		return true;
	}

	template<typename ForwardIterator, typename _T>
	void fill(ForwardIterator __first, ForwardIterator __last, const _T& __value)
	{
		for (; __first != __last; ++__first)
		{
			*__first = __value;
		}
	}

	template<typename OutputIterator, typename size, typename _T>
	OutputIterator fill(OutputIterator __first, size n, const _T& __value)
	{
		for (; n > 0; --n; ++__first)
		{
			*__first = __value;
		}
		return __first;
	}

	template<typename ForwardIterator1, typename ForwardIterator2>
	void iter_swap(ForwardIterator1 __first, ForwardIterator2 __last)
	{
		typename iterator_traits<ForwardIterator1>::value_type tmp = *__first;
		*__first = *__last;
		*__last = tmp;
	}

	//字典排列方式比较大小
	template<typename InputIterator1, typename InputIterator2>
	bool lexicographical_compare(InputIterator1 __first1, InputIterator1 __last1, InputIterator2 __first2, InputIterator2 __last2)
	{
		for (; __first1 != __last1; ++__first1, ++__first2)
		{
			if (*__first1 < *__first2)
				return true;
			if (*__first2 < *__first1)
				return false;
		}

		return __first1 == __last1 && __first2 != __last2;
	}

	template<typename InputIterator1, typename InputIterator2,typename compare>
	bool lexicographical_compare(InputIterator1 __first1, InputIterator1 __last1, InputIterator2 __first2, InputIterator2 __last2,compare comp)
	{
		for (; __first1 != __last1; ++__first1, ++__first2)
		{
			if (comp(*__first1,*__first2))
				return true;
			if (comp(*__first2, *__first1))
				return false;
		}

		return __first1 == __last1 && __first2 != __last2;
	}

	bool lexicographical_compare(const unsigned char* __first1, const unsigned char* __last1, const unsigned char* __first2, const unsigned char* __last2)
	{
		const size_t len1 = __last1 - __first1;
		const size_t len2 = __last2 - __first2;
		//memcmp比较两者中相同的部分
		const int result = memcmp(__first1, __first2, min(len1, len2)); 
		//相同长度部分相同，两者中长的视为较大方
		return result != 0 ? result < 0 : len1 < len2;
	}

	//比较两个迭代器，返回两者不匹配的点，若都匹配返回last迭代器
	template<typename InputIterator1,typename InputIterator2>
	MatchPair<InputIterator1, InputIterator2> mismatch(InputIterator1 __first1,InputIterator1 __last1,
		InputIterator2 __first2)
	{
		while (__first1!=__last1&&*__first1==*__first2)
		{
			++__first1;
			++__first2;
		}
		return MatchPair<InputIterator1, InputIterator2>(__first1, __first2);
	}

	template<typename InputIterator1,typename InputIterator2,typename BinaryPredicator>
	MatchPair<InputIterator1, InputIterator2> mismatch(InputIterator1 __first1, InputIterator2 __last1,
		InputIterator2 __first2, BinaryPredicator  pred)
	{
		while (__first1 != __last1 && pred(*__first1, __first2))
		{
			++__first1;
			++__first2;
		}
		return MatchPair<InputIterator1, InputIterator2>(__first1, __first2);
	}
}


#endif // !_ALOGBASE_H_
