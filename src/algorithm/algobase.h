#pragma once
#ifndef _ALOGBASE_H_
#define _ALGOBASE_H_
#include <memory.h>
#include "tstl_util.h"
#include "functionbase.h"
#include "type_traits.h"
#ifndef __STL_CLASS_PARTIAL_SPECIALIZATION
#define __STL_CLASS_PARTIAL_SPECIALIZATION
#include "tstl_iterator_base.h"
#endif // !__STL_CLASS_PARTIAL_SPECIALIZATION

namespace TSTL
{

	//ȡ���ֵ
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

	//ȡ��Сֵ
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
		//�ڶ������бȵ�һ�����г���������������ͬλ��Ԫ����ͬʱ����true
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
		for (; n > 0; --n, ++__first)
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

	//�ֵ����з�ʽ�Ƚϴ�С
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

	template<typename InputIterator1, typename InputIterator2, typename compare>
	bool lexicographical_compare(InputIterator1 __first1, InputIterator1 __last1, InputIterator2 __first2, InputIterator2 __last2, compare comp)
	{
		for (; __first1 != __last1; ++__first1, ++__first2)
		{
			if (comp(*__first1, *__first2))
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
		//memcmp�Ƚ���������ͬ�Ĳ���
		const int result = memcmp(__first1, __first2, min(len1, len2));
		//��ͬ���Ȳ�����ͬ�������г�����Ϊ�ϴ�
		return result != 0 ? result < 0 : len1 < len2;
	}

	//�Ƚ��������������������߲�ƥ��ĵ㣬����ƥ�䷵��last������
	template<typename InputIterator1, typename InputIterator2>
	MatchPair<InputIterator1, InputIterator2> mismatch(InputIterator1 __first1, InputIterator1 __last1,
		InputIterator2 __first2)
	{
		while (__first1 != __last1 && *__first1 == *__first2)
		{
			++__first1;
			++__first2;
		}
		return MatchPair<InputIterator1, InputIterator2>(__first1, __first2);
	}

	template<typename InputIterator1, typename InputIterator2, typename BinaryPredicator>
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

	//�Ƚϴ�С���̳�binary_function
	template<typename _T = void>
	struct less :public binary_function<_T, _T, bool>
	{
		bool operator()(const _T& __left, const _T& __right)
		{
			return (__left < __right);
		}
	};

	//��
	template<typename _T = void>
	struct plus :public binary_function<_T, _T, _T>
	{
		_T operator()(const _T& __left, const _T& __right)const
		{
			return (__left + __right);
		}
	};

	//��
	template<typename _T = void>
	struct minus :public binary_function<_T, _T, _T>
	{
		_T operator()(const _T& __left, const _T& __right)const
		{
			return (__left - __right);
		}
	};

	//��
	template<typename _T = void>
	struct multiplies :public binary_function<_T, _T, _T>
	{
		_T operator()(const _T& __left, const _T& __right)const
		{
			return (__left * __right);
		}
	};


	//copy

	//������


	template<typename RandomAccessIterator, typename OutputIterator, typename Distance>
	OutputIterator __copy_d(RandomAccessIterator first, RandomAccessIterator last, OutputIterator result, Distance*)
	{
		for (Distance n = last - first; n > 0; --n, ++first, ++result)
		{
			*result = *first;
		}
	}

	//ԭ��ָ��Ϊtrue_type
	template<typename T>
	inline T* __copy_t(const T* first, const T* last, T* result, __true_type)
	{
		tstl_memmove(result, first, size_t(last - first));
		return result + (last - first);
	}

	//������Ϊfalse_type
	template<typename T>
	inline T* __copy_t(const T* first, const T* last, T* result, __false_type)
	{
		return __copy_d(first, last, result, (ptrdiff_t*)0);
	}

	template<typename InputIterator, typename OutputIterator>
	OutputIterator __copy(InputIterator first, InputIterator last, OutputIterator result, TSTL::input_iterator_tag)
	{
		while (first != last)
		{
			*result = *first;
			++result;
			++first;
		}
		return result;
	}

	template<typename RandomAccessIterator, typename OutputIterator>
	OutputIterator __copy(RandomAccessIterator first, RandomAccessIterator last, OutputIterator result, TSTL::random_access_iterator_tag)
	{
		typedef typename iterator_traits<RandomAccessIterator>::distance distance;
		return __copy_d(first, last, result, (distance*)0);
	}

	

	//�ڶ���
	template<typename InputIterator, typename OutputIterator>
	class copy_dispatch
	{
	public:
		OutputIterator operator()(InputIterator first, InputIterator last, OutputIterator result)
		{
			return __copy<InputIterator, OutputIterator>(first, last, result, TSTL::iterator_category(first));
		}
	};

	template<typename T>
	class copy_dispatch<T*, T*>
	{
	public:
		T* operator()(T* first, T* last, T* result)
		{
			typedef typename __type_traits<T>::has_trivial_assignment_operator t;
			return __copy_t(first, last, result, t());
		}
	};

	template<typename T>
	class copy_dispatch<const T*, T*>
	{
	public:
		T* operator()(T* first, T* last, T* result)
		{
			typedef typename __type_traits<T>::has_trivial_assignment_operator t;
			return __copy_t(first, last, result, t());
		}
	};



	//��һ��
	//������
	template<typename InputIterator, typename OutputIterator>
	inline OutputIterator copy(InputIterator first, InputIterator last, OutputIterator result)
	{
		return copy_dispatch<InputIterator, OutputIterator>()(first, last, result);
	}

	//�ػ���(const char*,const char*)
	inline char* copy(const char* first, const char* last, char* result)
	{
		tstl_memmove(result, first, size_t(last - first));
		return result + (last - first);
	}

	//�ػ���(const wchar_t*,const wchar_t*)
	inline wchar_t* copy(const wchar_t* first, const wchar_t* last, wchar_t* result)
	{
		tstl_memmove(result, first, size_t(last - first));
		return result + (last - first);
	}

	
	//copy_backward:�����һ��Ԫ�ظ��Ƶ���һ��Ԫ��
	
	//������
	template<typename InputIterator, typename OutputIterator>
	OutputIterator __copy_backward(InputIterator first, InputIterator last, OutputIterator result)
	{
		while (first != last)
		{
			*result = *first;
			--first;
			--result;
		}
		return result;
	}


	template<typename T>
	T* __copy_backward_t(const T* first, const T* last, T* result, __true_type)
	{
		const ptrdiff_t d = last - first;
		tstl_memmove(result - d, first, sizeof(T) * d);
		return result - d;
	}

	template<typename T>
	T* __copy_backward_t(const T* first, const T* last, T* result, __false_type)
	{
		return __copy_backward(first, last, result);
	}

	//�ڶ���
	template<typename InputIterator, typename OutputIterator>
	class __copy_backward_dispatch
	{
	public:
		OutputIterator operator()(InputIterator first, InputIterator last, OutputIterator result)
		{
			return __copy_backward(first, last, result);
		}
	};

	template<typename T>
	class __copy_backward_dispatch<T*, T*>
	{
	public:
		T* operator()(T* first, T* last, T* result)
		{
			typedef typename __type_traits<T>::has_trivial_assignment_operator t;
			return __copy_backward_t(first, last, result, t());
		}
	};

	//��һ��
	template<typename InputIterator,typename OutputIterator>
	OutputIterator copy_backward(InputIterator first, InputIterator last, OutputIterator result)
	{
		return __copy_backward_dispatch<InputIterator, OutputIterator>()(first, last, result);
	}

	



	
}


#endif // !_ALOGBASE_H_
