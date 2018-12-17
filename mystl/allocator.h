#ifndef __ALLOCATOR_H__
#define __ALLOCATOR_H__

#include "utility.h"

template <typename T>
class CAllocator
{
public:
	typedef size_t		size_type;
	typedef ptrdiff_t	difference_type;
	typedef T*			pointer;
	typedef const T*	const_pointer;
	typedef T&			reference;
	typedef const T&	const_reference;
	typedef T			value_type;

	template <typename U>
	struct rebind
	{
		typedef CAllocator<U> other;
	};

	pointer address(reference value) const
	{
		return &value;
	}

	const_pointer address(const_reference value) const
	{
		return &value;
	}

	CAllocator()  throw() 
	{
	}
	CAllocator(const CAllocator&) throw()
	{
	}
	template <typename U>
	CAllocator(const CAllocator<U>&) throw()
	{
	}
	~CAllocator() throw() 
	{
	}

	size_type max_size() const throw()
	{
		return ((size_t)(-1) / sizeof(T));
	}

	//pointer allocate(size_type num, CAllocator<void>::const_pointer hint = 0)
	pointer allocate(size_type num)
	{
		return (pointer)(::operator new(num * sizeof(T)));
	}

	void construct(pointer p)
	{
		//new((void*)p) T();
		::construct(p, T());
	}
	void construct(pointer p, const T& value)
	{
		//new((void*)p) T(value);
		::construct(p, value);
	}

	template <typename U, typename ... Args>
	void construct(T* p, Args&& ... args)
	{
		::new((void*)p) U(std::forward<Args>(args)...);
	}

	void destory(pointer p)
	{
		//p->~T();
		::destory(p);
	}

	template <typename ForwardIterator>
	void destory(ForwardIterator first, ForwardIterator last)
	{
		::destory(first, last);
	}

	void deallocate(pointer p, size_type num)
	{
		::operator delete((void*)p);
	}
};

template <typename T1, typename T2>
bool operator == (const CAllocator<T1>& alloc1, const CAllocator<T2>& alloc2) throw()
{	
	return true;
}

template <typename T1, typename T2>
bool operator != (const CAllocator<T1>& alloc1, const CAllocator<T2>& alloc2) throw()
{
	return (!(alloc1 == alloc2));
}
#endif
