#pragma once
#ifndef _ALOGBASE_H_
#define _ALGOBASE_H_

//ȡ���ֵ
template<typename T>
inline const T max(const T a,const T b)
{
	return a > b ? a : b;
}

//ȡ��Сֵ
template<typename T>
inline const T min(const T a, const T b)
{
	return a < b ? a : b;
}



#endif // !_ALOGBASE_H_
