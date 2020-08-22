#pragma once
#ifndef _FUNCTIONBASE_H_
#define _FUNCTIONBASE_H_

namespace TSTL
{
	//�º���	
	/*
	* unary_function ��������һԪ�����Ĳ������ͺͷ���ֵ����
	* TL �涨��ÿһ�� adaptable unaty function ��Ӧ�ü̳� unary_function
	*/
	template <typename _Arg,typename _Result>
	struct unary_function
	{
		typedef _Arg argument_type;
		typedef _Result result_type;
	};



	/*
	* binary_function �������ֶ�Ԫ�����ĵ�һ���������͡��ڶ����������͡�����ֵ����
	* STL �涨��ÿһ�� adaptable binary function ��Ӧ�ü̳� binary_function
	*/
	template<typename _Arg1,typename _Arg2,typename _Result>
	struct binary_function
	{
		typedef _Arg1 first_argument_type;
		typedef _Arg2 second_argument_type;
		typedef _Result result_type;
	};
}

#endif // !_FUNCTIONBASE_H_
