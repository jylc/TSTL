#pragma once
#ifndef _TSTL_UTIL_H_
#define _TSTL_UTIL_H_

template<typename _T1,typename _T2>
class MatchPair
{
public:
	typedef _T1 first_type;
	typedef _T2 second_type;
	_T1 _first;
	_T2 _second;
	MatchPair():_first(_T1()),_second(_T2()){}
	MatchPair(const _T1& first,const _T2& second):_first(first),_second(second){}
};
#endif // !_TSTL_UTIL_H_
