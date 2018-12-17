#ifndef __STACK_H__
#define __STACK_H__

//容器适配器stack源码实现 底层默认使用deque 也可使用list,vector
//底层容器必须有size(),empty(),back(),push_back(),pop_back()操作
template <typename T, typename Sequence = deque<T> >
class CStack
{
	friend bool operator== (const CStack&, const CStack&);
	friend bool operator< (const CStack&, const CStack&);
public:
	typedef typename Sequence::value_type	value_type;
	typedef typename Sequence::size_type	size_type;
	typedef typename Sequence::reference	reference;
	typedef typename Sequence::const_reference	const_reference;

protected:
	Sequence c;//底层容器
public:
	bool empty() const
	{
		return c.empty();
	}
	size_type size() const
	{
		return c.size();
	}
	reference top()
	{
		return c.back();
	}
	const_reference top() const
	{
		return c.back();
	}
	void push(const value_type& x)
	{
		c.push_back(x);
	}
	void pop()
	{
		c.pop_back();
	}
};

template <typename T, typename Sequence>
bool operator==(const CStack<T, Sequence>& x, const CStack<T, Sequence>& y)
{
	return x.c == y.c;
}

template <typename T, typename Sequence>
bool operator<(const CStack<T, Sequence>& x, const CStack<T, Sequence>& y)
{
	return x.c < y.c;
}
#endif