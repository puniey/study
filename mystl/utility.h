#ifndef __UTILITY_H__
#define __UTILITY_H__

#include <new>

struct __true_type{};
struct __false_type{};

template <typename T>
struct __type_traits
{
	typedef __false_type has_trivial_default_constructor;
	typedef __false_type has_trivial_copy_constructor;
	typedef __false_type has_trivial_assignment_operator;
	typedef __false_type has_trivial_destructor;
	typedef __false_type is_POD_type;
};

template<>
struct __type_traits<char>
{
	typedef __true_type has_trivial_default_constructor;
	typedef __true_type has_trivial_copy_constructor;
	typedef __true_type has_trivial_assignment_operator;
	typedef __true_type has_trivial_destructor;
	typedef __true_type is_POD_type;
};

template<>
struct __type_traits<signed char>
{
	typedef __true_type has_trivial_default_constructor;
	typedef __true_type has_trivial_copy_constructor;
	typedef __true_type has_trivial_assignment_operator;
	typedef __true_type has_trivial_destructor;
	typedef __true_type is_POD_type;
};

template<>
struct __type_traits<unsigned char>
{
	typedef __true_type has_trivial_default_constructor;
	typedef __true_type has_trivial_copy_constructor;
	typedef __true_type has_trivial_assignment_operator;
	typedef __true_type has_trivial_destructor;
	typedef __true_type is_POD_type;
};

template<>
struct __type_traits<short>
{
	typedef __true_type has_trivial_default_constructor;
	typedef __true_type has_trivial_copy_constructor;
	typedef __true_type has_trivial_assignment_operator;
	typedef __true_type has_trivial_destructor;
	typedef __true_type is_POD_type;
};

template<>
struct __type_traits<unsigned short>
{
	typedef __true_type has_trivial_default_constructor;
	typedef __true_type has_trivial_copy_constructor;
	typedef __true_type has_trivial_assignment_operator;
	typedef __true_type has_trivial_destructor;
	typedef __true_type is_POD_type;
};

template<>
struct __type_traits<int>
{
	typedef __true_type has_trivial_default_constructor;
	typedef __true_type has_trivial_copy_constructor;
	typedef __true_type has_trivial_assignment_operator;
	typedef __true_type has_trivial_destructor;
	typedef __true_type is_POD_type;
};

template<>
struct __type_traits<unsigned int>
{
	typedef __true_type has_trivial_default_constructor;
	typedef __true_type has_trivial_copy_constructor;
	typedef __true_type has_trivial_assignment_operator;
	typedef __true_type has_trivial_destructor;
	typedef __true_type is_POD_type;
};

template<>
struct __type_traits<long>
{
	typedef __true_type has_trivial_default_constructor;
	typedef __true_type has_trivial_copy_constructor;
	typedef __true_type has_trivial_assignment_operator;
	typedef __true_type has_trivial_destructor;
	typedef __true_type is_POD_type;
};

template<>
struct __type_traits<unsigned long>
{
	typedef __true_type has_trivial_default_constructor;
	typedef __true_type has_trivial_copy_constructor;
	typedef __true_type has_trivial_assignment_operator;
	typedef __true_type has_trivial_destructor;
	typedef __true_type is_POD_type;
};

template<>
struct __type_traits<long long>
{
	typedef __true_type has_trivial_default_constructor;
	typedef __true_type has_trivial_copy_constructor;
	typedef __true_type has_trivial_assignment_operator;
	typedef __true_type has_trivial_destructor;
	typedef __true_type is_POD_type;
};

template<>
struct __type_traits<float>
{
	typedef __true_type has_trivial_default_constructor;
	typedef __true_type has_trivial_copy_constructor;
	typedef __true_type has_trivial_assignment_operator;
	typedef __true_type has_trivial_destructor;
	typedef __true_type is_POD_type;
};

template<>
struct __type_traits<double>
{
	typedef __true_type has_trivial_default_constructor;
	typedef __true_type has_trivial_copy_constructor;
	typedef __true_type has_trivial_assignment_operator;
	typedef __true_type has_trivial_destructor;
	typedef __true_type is_POD_type;
};

template<>
struct __type_traits<long double>
{
	typedef __true_type has_trivial_default_constructor;
	typedef __true_type has_trivial_copy_constructor;
	typedef __true_type has_trivial_assignment_operator;
	typedef __true_type has_trivial_destructor;
	typedef __true_type is_POD_type;
};

template<typename T>
struct __type_traits<T*>
{
	typedef __true_type has_trivial_default_constructor;
	typedef __true_type has_trivial_copy_constructor;
	typedef __true_type has_trivial_assignment_operator;
	typedef __true_type has_trivial_destructor;
	typedef __true_type is_POD_type;
};

template <typename Iterator>
inline typename iterator_traits<Iterator>::value_type* value_type(const Iterator&)
{
	return static_cast<typename iterator_traits<Iterator>::value_type*>(0);
}

template <typename T, typename U>
inline void construct(T* p, const U& value)
{
	::new((void*)p) T(value);
}

template <typename T>
inline void destory(T* pointer)
{
	pointer->~T();
}

template <typename ForwardIterator>
inline void destory(ForwardIterator first, ForwardIterator last)
{//为了性能考虑，对没必要调用析构函数的对象 进行模板特化
	__destory(first, last, value_type(first));
}

template <typename ForwardIterator, typename T>
inline void __destory(ForwardIterator first, ForwardIterator last, T*)
{
	typedef typename __type_traits<T>::has_trivial_destructor trival_destructor;
	__destory_aux(first, last, trival_destructor());
}

template <typename ForwardIterator>
inline void __destory_aux(ForwardIterator first, ForwardIterator last, __false_type)
{
	for (; first < last; ++first)
	{
		destory(&*first);
	}
}

template <typename ForwardIterator>
inline void __destory_aux(ForwardIterator first, ForwardIterator last, __true_type)
{
	//基础数据类型无须析构
}

inline void destory(char*, char*){}
inline void destory(wchar_t*, wchar_t*){}


template<typename InputIterator, typename ForwardIterator>
inline ForwardIterator __unintialized_copy_aux(InputIterator first, InputIterator last, ForwardIterator result, __true_type)
{
	return std::copy(first, last, result);
}

template<typename InputIterator, typename ForwardIterator>
inline ForwardIterator __unintialized_copy_aux(InputIterator first, InputIterator last, ForwardIterator result, __false_type)
{
	for (; first != last; ++result, ++first)
	{
		construct(&*result, *first);
	}
	return result;
}

template <typename InputIterator, typename ForwardIterator, typename T>
inline ForwardIterator __unintialized_copy(InputIterator first, InputIterator last, ForwardIterator result, T*)
{
	typedef typename __type_traits<T>::is_POD_type is_POD;
	return __unintialized_copy_aux(first, last, result, is_POD());
}

template<typename InputIterator, typename ForwardIterator>
ForwardIterator uninitialized_copy(InputIterator first, InputIterator last,
	ForwardIterator result)
{
	return __unintialized_copy(first, last, result, value_type(result));
}

template<typename InputIterator, typename Size, typename ForwardIterator>
inline ForwardIterator __unintialized_copy_n_aux(InputIterator first, Size n, ForwardIterator result, __true_type)
{
	return std::copy_n(first, n, result);
}

template<typename InputIterator, typename Size, typename ForwardIterator>
inline ForwardIterator __unintialized_copy_n_aux(InputIterator first, Size n, ForwardIterator result, __false_type)
{
	for (; n > 0; ++result, ++first, --n)
	{
		construct(&*result, *first);
	}
	return result;
}

template <typename InputIterator, typename Size, typename ForwardIterator, typename T>
inline ForwardIterator __unintialized_copy_n(InputIterator first, Size n, ForwardIterator result, T*)
{
	typedef typename __type_traits<T>::is_POD_type is_POD;
	return __unintialized_copy_n_aux(first, n, result, is_POD());
}

template<typename InputIterator, typename Size, typename ForwardIterator>
ForwardIterator uninitialized_copy_n(InputIterator first, Size n, ForwardIterator result)
{
	return __unintialized_copy_n(first, n, result, value_type(result));
}

template<typename ForwardIterator, typename T>
inline ForwardIterator __unintialized_fill_aux(ForwardIterator first, ForwardIterator last, const T& x, __true_type)
{
	return std::fill(first, last, x);
}

template<typename ForwardIterator, typename T>
inline ForwardIterator __unintialized_fill_aux(ForwardIterator first, ForwardIterator last, const T& x, __false_type)
{
	for (; first != last; ++first)
	{
		construct(&*first, x);
	}
	return first;
}

template <typename ForwardIterator, typename T, typename U>
inline ForwardIterator __unintialized_fill(ForwardIterator first, ForwardIterator last, const T& x, U*)
{
	typedef typename __type_traits<U>::is_POD_type is_POD;
	return __unintialized_fill_aux(first, last, x, is_POD());
}

template < typename ForwardIterator, typename T >
ForwardIterator uninitialized_fill(ForwardIterator first, ForwardIterator last, const T& x)
{
	return __unintialized_fill(first, last, x, value_type(first));
}

template <typename ForwardIterator, typename Size, typename T>
ForwardIterator __unintialized_fill_n_aux(ForwardIterator first, Size n, const T& x, __true_type)
{
	return std::fill_n(first, n, x);
}

template <typename ForwardIterator, typename Size, typename T>
ForwardIterator __unintialized_fill_n_aux(ForwardIterator first, Size n, const T& x, __false_type)
{
	for (; n--; ++first)
	{
		construct(&*first, x);
	}
	return first;
}

template <typename ForwardIterator, typename Size, typename T, typename U>
ForwardIterator __unitialized_fill_n(ForwardIterator first, Size n, const T& x, U*)
{
	typedef typename __type_traits<U>::is_POD_type is_POD;
	return __unintialized_fill_n_aux(first, n, x, is_POD());
}

template < typename ForwardIterator, typename Size, typename T >
ForwardIterator uninitialized_fill_n(ForwardIterator first, Size n, const T& x)
{
	return __unitialized_fill_n(first, n, x, value_type(first));
}
#endif
