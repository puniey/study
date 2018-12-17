#ifndef __VECTOR_H__
#define __VECTOR_H__

//#define _SCL_SECURE_NO_WARNINGS

#include "allocator.h"
#include <cstddef>
#include <algorithm>
//vector 容器的简单实现
template <typename T, typename Alloc = CAllocator<T> >
class CVector
{
public:
	typedef T			value_type;
	typedef value_type*	pointer;
	typedef value_type* iterator;
	typedef	value_type&	reference;
	typedef size_t		size_type;
	typedef ptrdiff_t	difference_type;

	CVector();
	CVector(size_type n, const T& value);
	CVector(int n, const T& value);
	CVector(long n, const T& value);
	explicit CVector(size_type n);

	~CVector();

	iterator begin();
	iterator end();
	size_type size();
	size_type capacity();
	bool empty();
	reference operator[](size_type n);
	reference front();
	reference back();
	void push_back(const T& x);
	void pop_back();
	iterator erase(iterator postion);
	void erase(iterator first, iterator last);
	iterator insert(iterator position, const T& x);
	void insert(iterator position, size_type n, const T& x);
	void resize(size_type new_size, const T& x);
	void resize(size_type new_size);
	void clear();

private:
	iterator	start;
	iterator	finish;
	iterator	end_of_storage;
	Alloc		alloc;

	void deallocate();

	void fill_initialize(size_type n, const T& value);
	iterator allocate_and_fill(size_type n, const T& x);


};

template <typename T, typename Alloc>
CVector<T, Alloc>::CVector() :start(0), finish(0), end_of_storage(0)
{
}

template <typename T, typename Alloc>
CVector<T, Alloc>::CVector(size_type n, const T& value)
{
	fill_initialize(n, value);
}

template <typename T, typename Alloc>
CVector<T, Alloc>::CVector(int n, const T& value)
{
	fill_initialize(n, value);
}

template <typename T, typename Alloc>
CVector<T, Alloc>::CVector(long n, const T& value)
{
	fill_initialize(n, value);
}

template <typename T, typename Alloc>
CVector<T, Alloc>::CVector(size_type n)
{
	fill_initialize(n, T());
}

template <typename T, typename Alloc>
CVector<T, Alloc>::~CVector()
{
	alloc.destory(start, finish);
	deallocate();
}

template <typename T, typename Alloc>
typename CVector<T, Alloc>::iterator CVector<T, Alloc>::begin()
{
	return start;
}

template <typename T, typename Alloc>
typename CVector<T, Alloc>::iterator CVector<T, Alloc>::end()
{
	return finish;
}

template <typename T, typename Alloc>
typename CVector<T, Alloc>::size_type CVector<T, Alloc>::size()
{
	return size_type(end() - begin());
}

template <typename T, typename Alloc>
typename CVector<T, Alloc>::size_type CVector<T, Alloc>::capacity()
{
	return size_type(end_of_storage - begin());
}

template <typename T, typename Alloc>
bool CVector<T, Alloc>::empty()
{
	return end() == begin();
}

template <typename T, typename Alloc>
typename CVector<T, Alloc>::reference CVector<T, Alloc>::operator[](size_type n)
{
	return *(begin() + n);
}

template <typename T, typename Alloc>
typename CVector<T, Alloc>::reference CVector<T, Alloc>::front()
{
	return *begin();
}

template <typename T, typename Alloc>
typename CVector<T, Alloc>::reference CVector<T, Alloc>::back()
{
	return *(end() - 1);
}

template <typename T, typename Alloc>
void CVector<T, Alloc>::push_back(const T& x)
{
	if (finish != end_of_storage)
	{
		alloc.construct(finish, x);
		++finish;
	} 
	else
	{
		insert(end(), x);
	}
}

template <typename T, typename Alloc>
void CVector<T, Alloc>::pop_back()
{
	--finish;
	alloc.destory(finish);
}

template <typename T, typename Alloc>
typename CVector<T, Alloc>::iterator CVector<T, Alloc>::erase(iterator postion)
{
	if ((postion + 1) != end())
		std::copy(postion + 1, finish, postion);
	--finish;
	alloc.destory(finish);
	return postion;
}

template <typename T, typename Alloc>
void CVector<T, Alloc>::erase(iterator first, iterator last)
{
	if (last != end())
		std::copy(last, finish, first);
	iterator old_finish = finish;
	finish -= last - first;
	alloc.destory(finish, old_finish);
}

template <typename T, typename Alloc>
typename CVector<T, Alloc>::iterator CVector<T, Alloc>::insert(iterator position, const T& x)
{
	insert(position, 1, x);
	return position;
}

template <typename T, typename Alloc>
void CVector<T, Alloc>::insert(iterator position, size_type n, const T& x)
{
	if (n == 0)
		return;

	size_type storage_left = end_of_storage - finish;

	if (storage_left >= n)//有足够的备用空间
	{
		T x_copy = x;
		const size_type elems_after = finish - position;
		iterator old_finish = finish;

		if (elems_after > n)
		{
			::uninitialized_copy(finish - n, finish, finish);
			finish += n;
			std::copy_backward(position, old_finish - n, old_finish);
			std::fill(position, position + n, x_copy);
		} 
		else
		{
			::uninitialized_fill_n(finish, n - elems_after, x_copy);
			finish += n - elems_after;
			::uninitialized_copy(position, old_finish, finish);
			finish += elems_after;
			std::fill(position, old_finish, x_copy);
		}
	} 
	else
	{//备用空间不足 额外分配内存
		const size_type old_size = size();
		const size_type new_size = old_size + max(old_size, n);
		iterator new_start = alloc.allocate(new_size);
		iterator new_finish = new_start;
		try
		{
			new_finish = ::uninitialized_copy(start, position, new_start);
			new_finish = ::uninitialized_fill_n(new_finish, n, x);
			new_finish = ::uninitialized_copy(position, finish, new_finish);
		}
		catch (...)
		{// commit or rollback
			alloc.destory(new_start, new_finish);
			alloc.deallocate(new_start, new_size);
			throw;
		}

		alloc.destory(start, finish);
		deallocate();
		start = new_start;
		finish = new_finish;
		end_of_storage = new_start + new_size;
	}
}

template <typename T, typename Alloc>
void CVector<T, Alloc>::resize(size_type new_size, const T& x)
{
	if (new_size < size())
	{
		erase(begin() + new_size, end());
	} 
	else
	{
		insert(end(), new_size - size(), x);
	}
}

template <typename T, typename Alloc>
void CVector<T, Alloc>::resize(size_type new_size)
{
	resize(new_size, T());
}

template <typename T, typename Alloc>
void CVector<T, Alloc>::clear()
{
	erase(begin(), end());
}

template <typename T, typename Alloc>
void CVector<T, Alloc>::deallocate()
{
	if (start)
		alloc.deallocate(start, end_of_storage - start);
}

template <typename T, typename Alloc>
void CVector<T, Alloc>::fill_initialize(size_type n, const T& value)
{
	start = allocate_and_fill(n, value);
	finish = start + n;
	end_of_storage = finish;
}

template <typename T, typename Alloc>
typename CVector<T, Alloc>::iterator CVector<T, Alloc>::allocate_and_fill(size_type n, const T& x)
{
	iterator result = alloc.allocate(n);
	::uninitialized_fill_n(result, n, x);
	return result;
}

#endif
