#pragma once
#ifndef _TSTL_UTIL_H_
#define _TSTL_UTIL_H_

namespace TSTL
{
	template<typename _T1, typename _T2>
	class MatchPair
	{
	public:
		typedef _T1 first_type;
		typedef _T2 second_type;
		_T1 _first;
		_T2 _second;
		MatchPair() :_first(_T1()), _second(_T2()) {}
		MatchPair(const _T1& first, const _T2& second) :_first(first), _second(second) {}
	};


	//复制函数
	void* tstl_memmove(void* dst, const void* src, size_t count)
	{
		void* ret = dst;
		if (dst <= src || (char*)dst >= ((char*)+count))
		{
			//dst与src没有交叉，直接拷贝
			while (count--)
			{
				*(char*)dst = *(char*)src;
				dst = (char*)dst + 1;
				src = (char*)src + 1;
			}
		}
		else
		{
			//若dst与src有交叉，则从后往前拷贝，避免数据重叠冲突
			dst = (char*)dst + count - 1;
			src = (char*)src + count - 1;
			while (count--)
			{
				*(char*)dst = *(char*)src;
				dst = (char*)dst - 1;
				src = (char*)src - 1;
			}
		}
		return ret;
	}
}

#endif // !_TSTL_UTIL_H_
