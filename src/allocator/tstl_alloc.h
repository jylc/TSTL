#pragma once
#ifndef _TSTL_ALLOC_H_
#define _TSTL_ALLOC_H_


#include <functional>
#include <memory>
#include <cassert>
#ifndef __RESTRICT
#define __RESTRICT
#endif // !__RESTRICT

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


#ifdef __STL_THREADS
#else
#define __NODE_ALLOCATOR_LOCK
#define __NODE_ALLOCATOR_UNLOCK
#define __NODE_ALLOCATOR_THREADS false

#endif // __STL_THREADS
namespace TSTL {
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
		using __malloc_alloc_oom_handler = std::function<void()>;
		using __my_alloc_handler = std::function<void()>;
		static __malloc_alloc_oom_handler handler_;
		std::function<void()> test_func;

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


		static __malloc_alloc_oom_handler __set_malloc_handler(std::function<void()> handler)
		{
			auto  old_handler = handler_;
			handler_ = handler;
			return old_handler;
		}
	};

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

	typedef __malloc_alloc_template malloc_alloc;

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
		enum { _S_extra = 8 };
	public:
		static void* allocate(size_t n)
		{
			//����n+(int)_S_extra���ֽ��ڴ�ռ�
			char* result = (char*)_Alloc::allocate(n + (int)_S_extra);
			//ǰ�˸��ֽ������洢n
			*(size_t*)result = n;
			//����n���ֽڵ�λ�ڴ�ռ�
			return result + (int)_S_extra;
		}

		static void deallocate(void* p, size_t n)
		{
			//__real_pΪ֮ǰ������������ڴ�ռ�
			char* __real_p = (char*)p - (int)_S_extra;
			assert(*(size_t*)__real_p == n);
			_Alloc::deallocate(__real_p, n + (int)_S_extra);
		}

		static void* reallocate(void* p, size_t old_size, size_t new_size)
		{
			char* __read_p = (char*)p - (int)_S_extra;
			assert(*(size_t*)__read_p == n);
			char* result = (char*)_Alloc::reallocate(p, new_size + (int)_S_extra);
			*(size_t*)result = new_size;
			return result + (int)_S_extra;
		}
	};


#ifdef __USE_MALLOC_
	typedef malloc_alloc alloc;
	typedef malloc_alloc single_client_alloc; ss
#else

	//������������GCCĬ��ʹ�ö������������Ա���̫��С����������ڴ���Ƭ
	template<bool threads>
	class __default_alloc_template
	{
	private:
		enum { _ALIGN = 8 };//С��������ϵ��߽�
		enum { _MAX_BYTES = 128 };	//С��������Ͻ磨��128�ֽ�Ϊ�ֽ�㣬����128ʹ�õ�һ����������С��ʹ�õڶ�����
		enum { _NFREELISTS = 16 };	//_MAX_BYTES/_ALIGN free-list����

		//���κ�С��������ڴ��������ϵ��� 8 �ı���
		static size_t _S_round_up(size_t bytes)
		{
			return (bytes + (size_t)_ALIGN - 1) & ~((size_t)_ALIGN - 1);
		}

		union _Obj {
			union _Obj* _M_free_link_list;
			char _M_client_data[i];
		};

		//16����������
		static _Obj* __STL_VOLATILE _S_free_list[_NFREELISTS];
		//���������Ĵ�С���ҵ���Ӧ����������±꣨��0��ʼ�㣩
		static size_t _S_freelist_index(size_t bytes)
		{
			return (bytes + (size_t)_ALIGN - 1) / (size_t)_ALIGN - 1;
		}

		static void* _S_refill(size_t n);
		static char* _S_chunk_alloc(size_t size, int& nobjs);

		static char* _S_start_state;
		static char* _S_end_state;
		static size_t _S_heap_size;


		class _Lock;
		friend class _Lock;
		class _Lock
		{
		public:
			_Lock() { __NODE_ALLOCATOR_LOCK; }
			~log() { __NODE_ALLOCATOR_UNLOCK; }
		};

	public:
		//n�������0
		static void* allocate(size_t n)
		{
			void* result;
			assert(n != 0);
			//n����128ʹ�õ�һ��������
			if (n > (size_t)_MAX_BYTES)
				result = malloc_alloc::allocate(n);
			else {
				_Obj* __STL_VOLATILE* my_free_list = _S_free_list + _S_freelist_index(n);
#ifndef _NOTHREADS
				_Lock __lock_instance;
#endif // !_NOTHREADS
				_Obj* __RESTRICT __result = *my_free_list;
				if (0 == __result)
				{
					result = _S_refill(_S_round_up(n));
				}
				else
				{
					*my_free_list = __result->_M_free_link_list;
					result = __result;
				}
			}
			return result;
		}


		static void deallocate(void* p, size_t n)
		{
			if (n > (size_t)_MAX_BYTES)
				malloc_alloc::deallocate(p, n);
			else
			{
				_Obj* __STL_VOLATILE* my_free_list = _S_free_list + _S_freelist_index(n);
				_Obj* q = static_cast<_Obj*>(p);
#ifndef _NOTHREADS
				_Lock _lock_instance;
#endif // !_NOTHREADS
				q->_M_free_link_list = my_free_list;
				*my_free_list = q;
			}
		}

		static void* reallocate(void* p, size_t old_size, size_t new_size);
	};


	typedef __default_alloc_template<__NODE_ALLOCATOR_THREADS> alloc;//�ڶ���������
	typedef __default_alloc_template<false> single_client_alloc;

	template<bool __threads>
	inline bool operator==(const __default_alloc_template<__threads>&, const __default_alloc_template<__threads>&)
	{
		return true;
	}


	//���ڴ����ȡ�ռ�
	template<bool __threads>
	char* __default_alloc_template<__threads>::_S_chunk_alloc(size_t size, int& __nobjs)
	{
		char* result;
		size_t total_size = size * __nobjs;
		size_t idle_size = _S_end_state - _S_start_state;//�ڴ���еĿ����ڴ�
		if (idle_size >= total_size)
		{
			//�ڴ�ռ��㹻
			result = _S_start_state;
			_S_start_state += total_size;
			return result;
		}
		else if (idle_size >= size)
		{
			//�ڴ�ռ䲻�����ǻ��ܷ��伸��
			__nobjs = (int)(idle_size / size);
			total_size = __nobjs * size;
			result = _S_start_state;
			_S_start_state += total_size;
			return result;
		}
		else
		{
			//�ڴ�ռ���ȫ����
			//����2����С�ڴ�ռ�+������
			size_t __size_to_get = 2 * total_size + _S_round_up(_S_heap_size >> 2);
			if (idle_size > 0)
			{//���ڴ��ʣ��Ŀռ��������ʵĿ�������
				_Obj* __STL_VOLATILE* my_free_list = _S_free_list + _S_freelist_index(__size_to_get);
				((_Obj*)_S_start_state)->_M_free_link_list = *my_free_list;
				*my_free_list = (_Obj*)_S_start_state;
			}
			_S_start_state = (char*)malloc(__size_to_get);
			if (0 == _S_start_state)
			{
				size_t __i;
				_Obj* __p;
				_Obj* __STL_VOLATILE* my_free_list;
				for (__i = size; __i <= _MAX_BYTES; i += _ALIGN)
				{
					my_free_list = _S_free_list + _S_freelist_index(__i);
					p = *my_free_list;
					if (0 != p)
					{
						*my_free_list = p->_M_free_link_list;
						_S_start_state = (char*)p;
						_S_end_state = _S_start_state + __i;
						return (_S_chunk_alloc(size, __nobjs));
					}
				}
				_S_end_state = 0;
				_S_start_state = (char*)malloc_alloc::allocate(__size_to_get);
			}
			_S_heap_size += __size_to_get;
			_S_end_state = _S_start_state + __size_to_get;
			return (_S_chunk_alloc(size, __nobjs));
		}
	}

	template<bool __threads>
	void* __default_alloc_template<__threads>::_S_refill(size_t n)
	{
		int nobjs = 20;//Ĭ�Ϸ���20��
		char* chunk = _S_chunk_alloc(n, nobjs);
		_Obj* __STL_VOLATILE* my_free_list;
		_Obj* __result;
		_Obj* __next_obj;
		_Obj* __current_obj;
		if (1 == nobjs) return chunk;
		my_free_list = _S_free_list + _S_freelist_index(n);
		__result = (_Obj*)chunk;
		*my_free_list = __next_obj = (_Obj*)(chunk + n);
		for (size_t i = 1;; i++)
		{
			__current_obj = __next_obj;
			//__next_obj������������һ�οռ䣬Ҫ����ת��Ϊָ����������ʽ
			__next_obj = (_Obj*)((char*)__next_obj + n);
			if (nobjs - 1 == i)
				__current_obj->_M_free_link_list = 0;
			else
			{
				__current_obj->_M_free_link_list = __next_obj;
			}
		}
		return __result;
	}

	template<bool __threads>
	void* __default_alloc_template<__threads>::reallocate(void* p, size_t old_size, size_t new_size)
	{
		void* __result;
		size_t __copy_size;
		if (old_size > (size_t)_MAX_BYTES && new_size > (size_t)_MAX_BYTES)
			return realloc(p, new_size);
		if (_S_round_up(old_size) == _S_round_up(new_size))return p;
		__result = allocate(new_size);
		__copy_size = (new_size > old_size) ? old_size : new_size;
		memcpy(__result, p, __copy_size);
		deallocate(p, old_size);
		return __result;
	}

	template<bool __threads>
	char *__default_alloc_template<__threads>::_S_start_state = 0;
	
	template<bool __threads>
	char* __default_alloc_template<__threads>::_S_end_state = 0;

	template<bool __threads>
	size_t __default_alloc_template<__threads>::_S_heap_size = 0;


#endif // __USE_MALLOC_


}

#endif // !_TSTL_ALLOC_H_
