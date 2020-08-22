#pragma once
#ifndef _FUNCTIONBASE_H_
#define _FUNCTIONBASE_H_

namespace TSTL
{
	//仿函数	
	/*
	* unary_function 用来呈现一元函数的参数类型和返回值类型
	* TL 规定，每一个 adaptable unaty function 都应该继承 unary_function
	*/
	template <typename _Arg,typename _Result>
	struct unary_function
	{
		typedef _Arg argument_type;
		typedef _Result result_type;
	};



	/*
	* binary_function 用来呈现二元函数的第一个参数类型、第二个参数类型、返回值类型
	* STL 规定，每一个 adaptable binary function 都应该继承 binary_function
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
