#pragma once
#ifndef _TSTL_ALLOC_H_
#define _TSTL_ALLOC_H_

namespace TSTL {

#include <functional>
#include <memory>
#ifndef __THROW_BAD_ALLOC
	/*
	* ������__STL_NO_BAD_ALLOC����û�ж���__STL_USE_EXCEPTIONS��ʹ���쳣��ʱ����������˳�
	*/

#if defined(__STL_NO_BAD_ALLOC)||!defined(__STL_USE_EXCEPTIONS)	
#include <iostream>
#include <cstdlib>
#define __THROW_BAD_ALLOC std::cerr<<"out of memory"<<std::endl; exit(1);
#else
#include <new>
#define __THROW_BAD_ALLOC throw std::bad_alloc()
#endif // define
#endif // !__THROW_BAD_ALLOC

	/*
		��һ��������(���������鳬��128bytesʱ)
	*/
	class __malloc_alloc_template
	{
	private:
		//���������ڴ治������
		static void* _S_oom_malloc(size_t);
		static void* _S_oom_realloc(void*, size_t);

		//����function����������
		using __malloc_alloc_oom_handler = std::function<static void()>;
		using __my_alloc_handler = std::function<void()>;
		__malloc_alloc_oom_handler handler_;

	public:
		//��һ��ֱ�ӵ���malloc�����ڴ�
		static void* allocate(size_t n)
		{
			void* result = malloc(n);
			if (0 == result) result = _S_oom_malloc(n);
			return result;
		}

		static void deallocate(void* p, size_t /*n*/)
		{
			free(p);
		}

		static void* reallocate(void* p, size_t n)
		{
			void* result = realloc(p, n);
			if (0 == result)
				result = _S_oom_realloc(p, n);
			return result;
		}


		static __malloc_alloc_oom_handler __set_malloc_handler(std::function<void()>& handler)
		{
			__malloc_alloc_oom_hander  old_handler = handler_;
			std::move(handler_, static_cast<__malloc_alloc_oom_handler> (handler));
			return old_handler;
		}
	};

	__malloc_alloc_template::handler_ = 0;

	void* __malloc_alloc_template::_S_oom_malloc(size_t n)
	{
		void* result;
		__my_alloc_handler my_handler = handler_;
		for (;;)//���ϳ������ã��ͷ�
		{
			if (0 == my_handler) { __THROW_BAD_ALLOC; }
			my_handler();
			result = malloc(n);
			if (result)
				return result;
		}
	}

	void* __malloc_alloc_template::_S_oom_realloc(void* p, size_t n)
	{
		void* result;
		__my_alloc_handler my_handler = handler_;
		//��һ���Ѿ������˿ռ��ָ�����·���ռ䣬pΪԭ��ָ�룬nΪ�����·���ռ�Ĵ�С
		for (;;)
		{
			my_handler = handler_;
			if (0 == my_handler)	//������δ���ͻ����ã��׳��쳣
				__THROW_BAD_ALLOC;
			my_handler();
			result = realloc(p, n);
			if (result)
				return result;
		}
	}

	//ת���ã����ô��ݸ�����������һ���װ��ʹ_Alloc���б�׼�ӿ�
	template<typename _Tp, typename _Alloc>
	class simple_alloc
	{
	public:
		//����n��Ԫ��
		static _Tp* allocate(size_t n)
		{
			return 0 == n ? 0 : static_cast<_Tp*>(_Alloc::allocate(n * sizeof _Tp));
		}
		static _Tp* allocate(void)
		{
			return static_cast<_Tp*>(_Alloc::allocate(sizeof _Tp));
		}

		static void deallocate(_Tp* p, size_t n)
		{
			if (0 != n) _Alloc::deallocate(p, n * sizeof _Tp);
		}
		static void deallocate(_Tp* p)
		{
			_Alloc::deallocate(p, sizeof _Tp);
		}
	};


	template<typename _Alloc>
	class debug_alloc
	{
	private:
		enum
		{
			_S_extra = 8;
		};
	};

}

#endif // !_TSTL_ALLOC_H_
