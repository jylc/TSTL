#pragma once
#ifndef _TSTL_CONSTRUCT_H_
#define _TSTL_CONSTRUCT_H_
//#include<new.h> new.h只有在windows中才有
#include <new>
#include "type_traits.h"
#include "tstl_iterator_base.h"

namespace TSTL
{
	//将初始值构造在指定的地址空间上
	template<typename _T1, typename _T2>
	inline void _Construct(_T1* __p, _T2& value)
	{
		new ((void*)__p) _T1(value); //placement new
	}

	template<typename _T1>
	inline void _Construct(_T1* __p)
	{
		new ((void*)__p) _T1();
	}
	template<typename _T>
	inline void _Destroy(_T* __pointer)
	{
		__pointer->~_T();
	}

	template<typename _ForwardIterator>
	void __destroy_aux(_ForwardIterator __first, _ForwardIterator __last, __false_type)
	{
		for (; __first != __last; ++__first)
			destroy(&(*__first));
	}

	// 若是 __true_type，什么都不做；这是 trivial destructor。
	template<typename _ForwardIterator>
	inline void _destroy_aux(_ForwardIterator, _ForwardIterator, __true_type) {}

	// 利用__type_traits<T>判断该类型的析构函数是否需要做什么。
	template<typename _ForwardIterator, typename _Tp>
	inline void _destroy(_ForwardIterator __first, _ForwardIterator __last, _Tp*)
	{
		//萃取器萃取出类型
		typedef typename __type_traits<_Tp>::has_trivail_destructor _Trivial_destructor;
		__destroy_aux(__first, __last, _Trivial_destructor)();
	}


	template<typename _ForwardIterator>
	inline void _Destroy(_ForwardIterator __first, _ForwardIterator __last)
	{
		_destroy(__first, __last, __VALUE_TYPE(__first));
	}



	template<typename _Tp>
	inline void destroy(_Tp* _pointer)
	{
		_Destroy(_pointer);
	}

	//destroy 泛型特化
	inline void _Destroy(char*, char*) {}
	inline void _Destroy(int*, int*) {}
	inline void _Destroy(long*, long*) {}
	inline void _Destroy(float*, float*) {}
	inline void _Destroy(double*, double*) {}

	template<typename _T1, typename _T2>
	inline void construct(_T1* p, const _T2& value)
	{
		_Construct(p, value);
	}

	template<typename _Tp>
	inline void construct(_Tp* p)
	{
		_Construct(p);
	}

	template<typename _Tp>
	inline void destroy(_Tp *p)
	{
		_Destroy(p);
	}

	//将[__first,__last)上的所有对象释放掉
	template<typename _ForwardIterator>
	inline void destroy(_ForwardIterator __first, _ForwardIterator __last)
	{
		_Destroy(__first, __last);
	}
}


#endif // !_TSTL_CONSTRUCT_H_
