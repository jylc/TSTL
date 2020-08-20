#pragma once
#ifndef _TSTL_CONSTRUCT_H_
#define _TSTL_CONSTRUCT_H_
//#include<new.h> new.hֻ����windows�в���
#include <new>
#include "type_traits.h"
#include "tstl_iterator_base.h"

namespace TSTL
{
	//����ʼֵ������ָ���ĵ�ַ�ռ���
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

	// ���� __true_type��ʲô������������ trivial destructor��
	template<typename _ForwardIterator>
	inline void _destroy_aux(_ForwardIterator, _ForwardIterator, __true_type) {}

	// ����__type_traits<T>�жϸ����͵����������Ƿ���Ҫ��ʲô��
	template<typename _ForwardIterator, typename _Tp>
	inline void _destroy(_ForwardIterator __first, _ForwardIterator __last, _Tp*)
	{
		//��ȡ����ȡ������
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

	//destroy �����ػ�
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

	//��[__first,__last)�ϵ����ж����ͷŵ�
	template<typename _ForwardIterator>
	inline void destroy(_ForwardIterator __first, _ForwardIterator __last)
	{
		_Destroy(__first, __last);
	}
}


#endif // !_TSTL_CONSTRUCT_H_
