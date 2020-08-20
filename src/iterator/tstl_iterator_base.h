#pragma once
#ifndef _TSTL_ITERATOR_BASE_H_
#define _TSTL_ITERATOR_BASE_H_

namespace TSTL
{
	//�����ֵ��������
	struct input_iterator_tag {};
	struct output_iterator_tag {};
	struct forward_iterator_tag :public input_iterator_tag {};
	struct bidirectional_iterator_tag :public forward_iterator_tag {};
	struct random_access_iterator_tag :public bidirectional_iterator_tag {};

	/*
	*iterators�����������ش�algorithm������
	*�����ڱ�׼���������֣�����������reference��pointer
	*��δ��ʹ��
	*/
	template<typename _Tp, typename _Distance>
	class input_iterator
	{
	public:
		typedef input_iterator_tag iterator_category;
		typedef _Tp value_type;
		typedef _Distance difference_type;
		typedef _Tp* pointer;
		typedef _Tp& reference;
	};

	template<typename _Tp, typename _Distance>
	class output_iterator
	{
	public:
		typedef output_iterator_tag iterator_category;
		typedef void value_type;
		typedef void difference_type;
		typedef void pointer;
		typedef void reference;
	};

	template<typename _Tp, typename _Distance>
	class forward_iterator
	{
	public:
		typedef forward_iterator_tag iterator_category;
		typedef _Tp value_type;
		typedef _Distance difference_type;
		typedef _Tp* pointer;
		typedef _Tp& reference;
	};

	template<typename _Tp, typename _Distance>
	class bidirectional_iterator
	{
	public:
		typedef bidirectional_iterator_tag iterator_category;
		typedef _Tp value_type;
		typedef _Distance difference_type;
		typedef _Tp* pointer;
		typedef _Tp& reference;
	};

	template<typename _Tp, typename _Distance>
	class random_access_iterator
	{
	public:
		typedef random_access_iterator_tag iterator_category;
		typedef _Tp value_type;
		typedef _Distance difference_type;
		typedef _Tp* pointer;
		typedef _Tp& reference;
	};

	//�û��Լ�д�ĵ�������ü̳��ڴ�
#ifdef __STL_USE_NAMESPACES
	template<typename _Category, typename _Tp, typename _Distance = ptrdiff_t,
		typedef _Pointer = _Tp*, typedef _Reference = _Tp&>
		class iterator
	{
	public:
		typedef _Category iterator_category;
		typedef _Tp value_type;
		typedef _Distance difference_type;
		typedef _Pointer pointer;
		typedef _Reference reference;
	};
#endif // __STL_USE_NAMESPACES


	/*
	* �ʣ�Ϊʲô��Ҫ��ȡ��(traits)?
	* �𣺵�iterator��class����ʱ�����Իش��㷨�����ʣ�
	* ����iterator��native pointer��������Ϊһ���˻���iterator��ʱ�����޷��ش�
	* ����ʹ����ȡ������������ȡ�������൱�������һ���м�㣩��
	*/


#ifdef __STL_CLASS_PARTIAL_SPECIALIZATION
	// traits ��ȡ����������������(��Ӧ����)-----����������
	template<typename _Iterator>
	class iterator_traits
	{
	public:
		typedef _Iterator::iterator_category iterator_category;		//������������
		typedef _iterator::value_type value_type;					//������������ú��ֵ������
		typedef _iterator::difference_type difference_type;			//����������֮��ľ���
		typedef _iterator::pointer pointer;							//ָ�򱻵������͵�ָ��
		typedef _iterator::reference reference;						//���������͵�����
	};
	// ���ԭ��ָ��(native pointer)����Ƶ� traits ƫ�ػ���
	template<typename _Tp>
	class iterator_traits<_Tp*>
	{
	public:
		typedef random_access_iterator_tag iterator_category;
		typedef ptrdiff_t difference_type;
		typedef _Tp value_type;
		typedef _Tp* pointer;
		typedef _Tp& reference;
	};

	template<typename _Tp>
	class iterator_traits<const _Tp*>
	{
	public:
		typedef random_access_iterator_tag iterator_category;
		typedef ptrdiff_t difference_type;
		typedef _Tp value_type;
		typedef const _Tp* pointer;
		typedef const _Tp& reference;
	};

	template<typename _Iter>
	inline typename iterator_traits<_Iter>::iterator_category
		__iterator_category(const _Iter&)
	{
		typedef typename iterator_traits<_Iter>::iterator_category _Category;
		return _Category();
	}

	template<typename _Iter>
	inline typename iterator_traits<_Iter>::difference_type*
		__difference_type(const _Iter&)
	{
		return static_cast<typename iterator_traits<_Iter>::difference_type*>(0);
	}

	template<typename _Iter>
	inline typename iterator_traits<_Iter>::value_type*
		__value_type(const _Iter&)
	{
		return static_cast<typename iterator_traits<_Iter>::value_type*>(0);
	}

	//��װ
	template<typename _Iter>
	inline typename iterator_traits<_Iter>::iterator_category
		iterator_category(const _Iter& __i)
	{
		return __iterator_category(__i);
	}

	template<typename _Iter>
	inline typename iterator_traits<_Iter>::difference_type*
		difference_type(const _Iter& __i)
	{
		return __difference_type(__i);
	}

	template<typename _Iter>
	inline typename iterator_traits<_Iter>::value_type*
		value_type(const _Iter& __i)
	{
		return __value_type(__i);
	}
#define __ITERATOR_CATEGORY(__i) __iterator_category(__i)
#define __DISTANCE_TYPE(__i)     __distance_type(__i)
#define __VALUE_TYPE(__i)        __value_type(__i)


#else
	template<typename _Tp,typename _Distance>
	inline input_iterator_tag
		iterator_category(const input_iterator<_Tp, _Distance>&)
	{
		return input_iterator_tag();
	}

	template<typename _Tp,typename _Distance>
	inline output_iterator_tag
		iterator_category(const output_iterator<_Tp, _Distance>&)
	{
		return output_iterator_tag();
	}

	template<typename _Tp, typename _Distance>
	inline forward_iterator_tag
		iterator_category(const forward_iterator<_Tp, _Distance>&)
	{
		return forward_iterator_tag();
	}

	template<typename _Tp, typename _Distance>
	inline bidirectional_iterator_tag
		iterator_category(const bidirectional_iterator<_Tp, _Distance>&)
	{
		return bidirectional_iterator_tag
	}

	template<typename _Tp, typename _Distance>
	inline random_access_iterator_tag
		iterator_category(const random_access_iterator<_Tp, _Distance>&)
	{
		return random_access_iterator_tag
	}

	template<typename _Tp>
	inline random_access_iterator_tag
		iterator_category(const _Tp*)
	{
		return random_access_iterator_tag();
	}

	//value_type
	template<typename _Tp,typename _Distance>
	inline _Tp* value_type(const input_iterator<_Tp, _Distance>&)
	{
		return (_Tp*)(0);
	}

	template<typename _Tp,typename _Distance>
	inline _Tp* value_type(const forward_iterator<_Tp, _Distance>&)
	{
		return (_Tp*)(0);
	}

	template<typename _Tp, typename _Distance>
	inline _Tp* value_type(const bidirectional_iterator<_Tp, _Distance>&)
	{
		return (_Tp*)(0);
	}

	template<typename _Tp, typename _Distance>
	inline _Tp* value_type(const random_access_iterator<_Tp, _Distance>&)
	{
		return (_Tp*)(0);
	}

	template<typename _Tp>
	inline _Tp* value_type(const _Tp*) {
		return (_Tp*)(0);
	}



#endif // __STL_CLASS_PARTIAL_SPECIALIZATION


}

#endif // !_TSTL_ITERATOR_BASE_H_
