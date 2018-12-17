#ifndef __QUEUE_H__
#define __QUEUE_H__

//容器适配器queue源码实现 底层默认使用deque 也可使用list,不能使用vector，因为没有pop_front接口
//底层容器必须有size(),empty(),push_back(),back(),front(),pop_front()操作
template <typename T, typename Sequence = deque<T> >
class CQueue
{
	friend bool operator==(const CQueue&, const CQueue&);
	friend bool operator<(const CQueue&, const CQueue&);
public:
	typedef typename Sequence::value_type	value_type;
	typedef typename Sequence::size_type	size_type;
	typedef typename Sequence::reference	reference;
	typedef typename Sequence::const_reference	const_reference;

protected:
	Sequence c; //底层容器
public:
	bool empty() const
	{
		return c.empty();
	}
	size_type size() const
	{
		return c.size();
	}
	reference front()
	{
		return c.front();
	}
	reference back()
	{
		return c.back();
	}
	const_reference front() const
	{
		return c.front();
	}
	const_reference back() const
	{
		return c.back();
	}
	void push(const value_type& x)
	{
		c.push_back(x);
	}
	void pop()
	{
		c.pop_front();
	}
};

template <typename T, typename Sequence>
bool operator==(const CQueue<T, Sequence>& x, const CQueue<T, Sequence>& y)
{
	return x.c == y.c;
}

template <typename T, typename Sequence>
bool operator<(const CQueue<T, Sequence>& x, const CQueue<T, Sequence>& y)
{
	return x.c < y.c;
}

#endif
