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
* 定义了__STL_NO_BAD_ALLOC或者没有定义__STL_USE_EXCEPTIONS（使用异常）时，输出错误并退出
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
		第一级配置器(当配置区块超过128bytes时)
	*/
	class __malloc_alloc_template
	{
	private:
		//用来处理内存不足的情况
		static void* _S_oom_malloc(size_t);
		static void* _S_oom_realloc(void*, size_t);

		//利用function创建处理函数
		using __malloc_alloc_oom_handler = std::function<void()>;
		using __my_alloc_handler = std::function<void()>;
		static __malloc_alloc_oom_handler handler_;
		std::function<void()> test_func;

	public:
		//第一级直接调用malloc分配内存
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
		for (;;)//不断尝试配置，释放
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
		//给一个已经分配了空间的指针重新分配空间，p为原有指针，n为需重新分配空间的大小
		for (;;)
		{
			my_handler = handler_;
			if (0 == my_handler)	//处理函数未被客户设置，抛出异常
				__THROW_BAD_ALLOC;
			my_handler();
			result = realloc(p, n);
			if (result)
				return result;
		}
	}

	typedef __malloc_alloc_template malloc_alloc;

	//转调用，调用传递给配置器；多一层包装，使_Alloc具有标准接口
	template<typename _Tp, typename _Alloc>
	class simple_alloc
	{
	public:
		//配置n个元素
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
			//创建n+(int)_S_extra个字节内存空间
			char* result = (char*)_Alloc::allocate(n + (int)_S_extra);
			//前八个字节用来存储n
			*(size_t*)result = n;
			//返回n个字节单位内存空间
			return result + (int)_S_extra;
		}

		static void deallocate(void* p, size_t n)
		{
			//__real_p为之前分配的完整的内存空间
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

	//二级配置器，GCC默认使用二级配置器，以避免太多小额区块造成内存碎片
	template<bool threads>
	class __default_alloc_template
	{
	private:
		enum { _ALIGN = 8 };//小型区块的上调边界
		enum { _MAX_BYTES = 128 };	//小区区块的上界（以128字节为分界点，大于128使用第一级配置器，小于使用第二级）
		enum { _NFREELISTS = 16 };	//_MAX_BYTES/_ALIGN free-list个数

		//将任何小额区块的内存需求量上调至 8 的倍数
		static size_t _S_round_up(size_t bytes)
		{
			return (bytes + (size_t)_ALIGN - 1) & ~((size_t)_ALIGN - 1);
		}

		union _Obj {
			union _Obj* _M_free_link_list;
			char _M_client_data[i];
		};

		//16个空闲链表
		static _Obj* __STL_VOLATILE _S_free_list[_NFREELISTS];
		//根据所给的大小，找到相应空闲链表的下标（从0开始算）
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
		//n必须大于0
		static void* allocate(size_t n)
		{
			void* result;
			assert(n != 0);
			//n大于128使用第一级配置器
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


	typedef __default_alloc_template<__NODE_ALLOCATOR_THREADS> alloc;//第二级配置器
	typedef __default_alloc_template<false> single_client_alloc;

	template<bool __threads>
	inline bool operator==(const __default_alloc_template<__threads>&, const __default_alloc_template<__threads>&)
	{
		return true;
	}


	//从内存池中取空间
	template<bool __threads>
	char* __default_alloc_template<__threads>::_S_chunk_alloc(size_t size, int& __nobjs)
	{
		char* result;
		size_t total_size = size * __nobjs;
		size_t idle_size = _S_end_state - _S_start_state;//内存池中的空闲内存
		if (idle_size >= total_size)
		{
			//内存空间足够
			result = _S_start_state;
			_S_start_state += total_size;
			return result;
		}
		else if (idle_size >= size)
		{
			//内存空间不够但是还能分配几个
			__nobjs = (int)(idle_size / size);
			total_size = __nobjs * size;
			result = _S_start_state;
			_S_start_state += total_size;
			return result;
		}
		else
		{
			//内存空间完全不够
			//申请2倍大小内存空间+附加量
			size_t __size_to_get = 2 * total_size + _S_round_up(_S_heap_size >> 2);
			if (idle_size > 0)
			{//将内存池剩余的空间分配给合适的空闲链表
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
		int nobjs = 20;//默认分配20块
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
			//__next_obj现在是连续的一段空间，要将其转化为指针相连的形式
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
