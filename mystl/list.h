#ifndef __LIST_H__
#define __LIST_H__

#include "allocator.h"
#include <iterator>
#include <algorithm>
#include <cstddef>

template <typename T>
struct __list_node
{
	typedef __list_node*	void_pointer;
	void_pointer	prev;
	void_pointer	next;
	T				data;
};

template <typename T, typename Ref, typename Ptr>
struct __list_iterator
{
	typedef __list_iterator<T,	T&, T*>			iterator;
	typedef __list_iterator<T, Ref, Ptr>		self;
	typedef std::bidirectional_iterator_tag		iterator_category;
	typedef T									value_type;
	typedef Ptr									pointer;
	typedef Ref									reference;
	typedef __list_node<T>*						link_type;
	typedef size_t								size_type;
	typedef ptrdiff_t							difference_type;

	link_type node;

	//constructor
	__list_iterator(link_type x) : node(x) {}
	__list_iterator() {}
	__list_iterator(const iterator& x) : node(x.node) {}

	bool operator==(const self& x) const { return node == x.node; }
	bool operator!=(const self& x) const { return node != x.node; }

	reference operator*() const
	{
		return node->data;
	}

	pointer operator->() const
	{
		return &(operator*());
	}

	self& operator++()
	{
		node = (link_type)(node->next);
		return *this;
	}

	self operator++(int)
	{
		self tmp = *this;
		++*this;
		return tmp;
	}

	self& operator--()
	{
		node = (link_type)(node->prev);
		return *this;
	}

	self operator--(int)
	{
		self tmp = *this;
		++*this;
		return tmp;
	}
};
//环状双向链表
template <typename T, typename Alloc = CAllocator<T> >
class CList
{
public:
	typedef __list_node<T>										list_node;
	typedef list_node*											link_type;
	typedef __list_iterator<T, T&, T*>							iterator;
	typedef __list_iterator<T, const T&, const T*>				const_iterator;
	typedef T													value_type;
	typedef Alloc												allocator_type;
	typedef  typename allocator_type::pointer					pointer;
	typedef  typename allocator_type::const_pointer				const_pointer;
	typedef	 typename allocator_type::reference					reference;
	typedef  typename allocator_type::const_reference			const_reference;
	typedef  typename allocator_type::size_type					size_type;
	typedef  typename allocator_type::difference_type			difference_type;

private:
	CAllocator<list_node> list_node_alloc;
	link_type get_node()
	{
		return (link_type)list_node_alloc.allocate(1);
	}
	void put_node(link_type p)
	{
		list_node_alloc.deallocate(p, sizeof(list_node));
	}
	link_type create_node(const T& value)
	{
		link_type node = get_node();
		::construct(&node->data, value);
		return node;
	}
	void destory_node(link_type node)
	{
		list_node_alloc.destory(node);
		put_node(node);
	}
	void empty_initialize()
	{
		node = get_node();
		node->next = node;
		node->prev = node;
	}
public:
	CList() { empty_initialize(); }
	~CList()
	{
		clear();
		put_node(node);
	}

	iterator begin();
	iterator end();
	bool empty() const;
	size_type size() const;
	reference front();
	reference back();
	void push_back(const T&);
	void push_front(const T&);
	iterator insert(iterator, const T&);
	iterator erase(iterator);
	void pop_front();
	void pop_back();
	void clear();
	void clear2();
	void remove(const T& value);//将数值为value之所有元素移除
	void unique();//移除数值相同的连续元素 剩一个
	void transfer(iterator, iterator, iterator);
	void splice(iterator, CList&);
	void splice(iterator, CList&, iterator);
	void splice(iterator, CList&, iterator, iterator);
	void merge(CList&); //前提保证两个链表有序
	void reserve(); //逆序
	void sort();
	void swap(CList &other);

private:
	link_type		node;//只要一个指针便可表示整个环状双向链表 node 指于尾端空白节点
	allocator_type	alloc;
};

template<typename T, typename Alloc>
inline typename CList<T, Alloc>::iterator CList<T, Alloc>::begin()
{
	return (link_type)(node->next);
}

template<typename T, typename Alloc>
inline typename CList<T, Alloc>::iterator CList<T, Alloc>::end()
{
	return node;
}

template<typename T, typename Alloc>
inline bool CList<T, Alloc>::empty() const
{
	return node->next == node;
}

template<typename T, typename Alloc>
inline typename CList<T, Alloc>::size_type CList<T, Alloc>::size() const
{
	size_type result = 0;
	result = std::distance(begin(), end());
	return result;
}

template<typename T, typename Alloc>
inline typename CList<T, Alloc>::reference CList<T, Alloc>::front()
{
	return *begin();
}

template<typename T, typename Alloc>
inline typename CList<T, Alloc>::reference CList<T, Alloc>::back()
{
	return *(--end());
}

template<typename T, typename Alloc>
inline void CList<T, Alloc>::push_back(const T &x)
{
	insert(end(), x);
}

template<typename T, typename Alloc>
inline void CList<T, Alloc>::push_front(const T &x)
{
	insert(begin(), x);
}

template<typename T, typename Alloc>
inline typename CList<T, Alloc>::iterator CList<T, Alloc>::insert(iterator position, const T &x)
{
	link_type tmp = create_node(x);
	tmp->next = position.node;
	tmp->prev = position.node->prev;
	((link_type)position.node->prev)->next = tmp;
	position.node->prev = tmp;
	return tmp;
}

template<typename T, typename Alloc>
inline typename CList<T, Alloc>::iterator CList<T, Alloc>::erase(iterator position)
{
	link_type next_node = (link_type)position.node->next;
	link_type pre_node = (link_type)position.node->prev;
	pre_node->next = next_node;
	next_node->prev = pre_node;
	destory_node(position.node);
	return iterator(next_node);
}

template<typename T, typename Alloc>
inline void CList<T, Alloc>::pop_front()
{
	erase(begin());
}

template<typename T, typename Alloc>
inline void CList<T, Alloc>::pop_back()
{
	iterator tmp = end();
	erase(--tmp);
}

template<typename T, typename Alloc>
inline void CList<T, Alloc>::clear()
{
	link_type cur = (link_type)node->next;
	while (cur != node)
	{
		link_type tmp = cur;
		cur = (link_type)cur->next;
		destory_node(tmp);
	}
	node->next = node;
	node->prev = node;
}

template<typename T, typename Alloc>
inline void CList<T, Alloc>::clear2()
{
	iterator it = begin();
	while (it != end())
	{
		it = erase(it);
	}
}

template<typename T, typename Alloc>
inline void CList<T, Alloc>::remove(const T& value)
{
	iterator cur = begin();
	while (cur != end())
	{
		if (value == *cur)
		{
			cur = erase(cur);
		} 
		else
		{
			++cur;
		}
	}
}

template<typename T, typename Alloc>
inline void CList<T, Alloc>::unique()
{
	iterator first = begin();
	iterator last = end();
	if (first == last)
		return;
	iterator next = first;
	while (++next != last)
	{
		if (*first == *next)
		{
			erase(first);
		}
		first = next;
	}
}

template<typename T, typename Alloc>
inline void CList<T, Alloc>::transfer(iterator postion, iterator first, iterator last)
{
	if (postion != last)
	{
		((link_type)(last.node->prev))->next = postion.node;
		((link_type)(first.node->prev))->next = last.node;
		((link_type)(postion.node->prev))->next = first.node;
		link_type tmp = (link_type)postion.node->prev;
		postion.node->prev = last.node->prev;
		last.node->prev = first.node->prev;
		first.node->prev = tmp;
	}
}

template<typename T, typename Alloc>
inline void CList<T, Alloc>::splice(iterator postion, CList &x)
{
	if (!x.empty())
		transfer(postion, x.begin(), x.end());
}

template<typename T, typename Alloc>
inline void CList<T, Alloc>::splice(iterator postion, CList &x, iterator i)
{
	iterator j = i;
	++j;
	if (postion == i || postion == j)return;
	transfer(postion, i, j);
}

template<typename T, typename Alloc>
inline void CList<T, Alloc>::splice(iterator postion, CList &, iterator first, iterator last)
{
	if (first != last)
		transfer(postion, first, last);
}

template<typename T, typename Alloc>
inline void CList<T, Alloc>::merge(CList<T, Alloc> &x)
{
	iterator first1 = begin();
	iterator last1 = end();
	iterator first2 = x.begin();
	iterator last2 = x.end();

	while (first1 != last1 && first2 != last2)
	{
		if (*first2 < *first1)
		{
			iterator next = first2;
			transfer(first1, first2, ++next);
			first2 = next;
		}
		else
			++first1;
	}

	if (first2 != last2)
	{
		transfer(first1, first2, last2);
	}
}

template<typename T, typename Alloc>
inline void CList<T, Alloc>::reserve()
{
	if (node->next == node || ((link_type)(node->next))->next == node)
		return;

	iterator first = begin();
	++first;
	while (first != end())
	{
		iterator old = first;
		transfer(begin(), old, ++first);
	}
}

template<typename T, typename Alloc>
inline void CList<T, Alloc>::sort()
{
	if (node->next == node || link_type(node->next)->next == node)
	{
		return;
	}

	CList carry;
	CList counter[64];
	int fill = 0;
	while (!empty())
	{
		carry.splice(carry.begin(), *this, begin());
		int i = 0;
		while (i < fill && !counter[i].empty())
		{
			counter[i].merge(carry);
			carry.swap(counter[i++]);
		}
		carry.swap(counter[i]);
		if (i == fill)
		{
			++fill;
		}
	}

	for (int i = 1; i < fill; ++i)
	{
		counter[i].merge(counter[i - 1]);
	}

	swap(counter[fill - 1]);
}
template<typename T, typename Alloc>
inline void CList<T, Alloc>::swap(CList &other)
{
	std::swap(this->node, other.node);
}

#endif