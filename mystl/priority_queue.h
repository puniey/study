#pragma once
#include "heap.h"
//优先级队列 底层容器可以是vector,deque,不能是list,必须是随机访问迭代器，底层容器必须有size(),empty(),push_back(),pop_front()操作
template <typename T, typename Sequence = vector<T>, typename Compare = std::less<typename Sequence::value_type> >
class priority_queue
{
public:
	typedef typename Sequence::value_type		value_type;
	typedef typename Sequence::size_type		size_type;
	typedef typename Sequence::reference		reference;
	typedef typename Sequence::const_reference	const_reference;
protected:
	Sequence c; //底层容器
	Compare comp; //元素比较标准
public:
	priority_queue() : c() {}
	explicit priority_queue(const Compare& x) : c(), comp(x) {}

	template <typename InputIterator>
	priority_queue(InputIterator first, InputIterator last, const Compare& x)
		: c(first, last), comp(x)
	{
		::make_heap(c.begin(), c.end(), comp);
	}
	template <typename InputIterator>
	priority_queue(InputIterator first, InputIterator last)
		: c(first, last)
	{
		::make_heap(c.begin(), c.end(), comp);
	}

	bool empty() const { return c.empty(); }
	size_type size() const { return c.size(); }
	const_reference top() const { return c.front(); }
	void push(const value_type& x)
	{
		c.push_back(x);
		::push_heap(c.begin(), c.end(), comp);
	}
	void pop()
	{
		::pop_heap(c.begin(), c.end(), comp);
		c.pop_back();
	}
};