#pragma once
#ifndef _TYPE_TRAITS_H_
#define _TYPE_TRAITS_H_

namespace TSTL
{
	struct __true_type {};
	struct __false_type {};

	template<typename _Tp>
	struct __type_traits
	{
		typedef __true_type this_dummy_member_must_be_first;

		typedef __false_type has_trivail_destructor;
	};
}

#endif // !_TYPE_TRAITS_H_
