#ifndef __DEQUE_H__
#define __DEQUE_H__

#include "allocator.h"
#include <iterator>

inline size_t __deque_buf_size(size_t n, size_t sz)
{
	return n != 0 ? n : (sz < 512 ? size_t(512 / sz) : size_t(1));
}

#define INITIAL_MAP_SIZE	8

//迭代器定义
template <typename T, typename Ref, typename Ptr, size_t BufSize>
struct __deque_iterator
{
	typedef __deque_iterator<T, T&, T*, BufSize>				iterator;
	typedef __deque_iterator<T, const T&, const T*, BufSize>	const_iterator;
	static size_t buffer_size()
	{
		return __deque_buf_size(BufSize, sizeof(T));
	}

	typedef std::random_access_iterator_tag iterator_category;
	typedef T			value_type;
	typedef Ptr			pointer;
	typedef Ref			reference;
	typedef size_t		size_type;
	typedef ptrdiff_t	difference_type;
	typedef T**			map_pointer;

	typedef __deque_iterator self;

	//保持与容器的联结
	T*			cur;	//此迭代器所指缓冲区中的当前元素
	T*			first;  //此迭代器所指缓冲区的头
	T*			last;	//此迭代器所指缓冲区的尾
	map_pointer	node;	//指向管控中心

	//设置迭代器所指的缓冲区
	void set_node(map_pointer new_node)
	{
		node = new_node;
		first = *new_node;
		last = first + difference_type(buffer_size());
	}

	reference operator*() const
	{
		return *cur;
	}
	pointer operator->() const
	{
		return &(operator*());
	}

	difference_type operator-(const self& x) const
	{
		//自己写的比较直观的计算方法
		return (cur - first + 1) - (x.cur - x.first + 1) 
			+ difference_type((buffer_size())) * (node - x.node);
		/* STL模板详解中的写法 和上面的是等价的
		return difference_type(buffer_size()) * (node - x.node - 1) +
			(cur - first) + (x.last - x.cur);
			*/
	}

	self& operator++()
	{
		++cur;
		if (cur == last)
		{
			set_node(node + 1);
			cur = first;
		}
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
		if (cur == first)
		{
			set_node(node - 1);
			cur = last;
		}
		--cur;
		return *this;
	}

	self operator--(int)
	{
		self tmp = *this;
		--*this;
		return tmp;
	}

	self& operator+=(difference_type n)
	{
		difference_type offset = n + (cur - first);
		if (offset >= 0 && offset < difference_type(buffer_size()))
		{ //目标在同一个缓冲区内
			cur += n;
		}
		else
		{ //目标不在同一个缓冲区内
			difference_type node_offset =
				offset > 0 ? offset / difference_type(buffer_size()) :
				-difference_type((-offset - 1) / buffer_size()) - 1;

			set_node(node + node_offset);
			cur = first + (offset - node_offset * difference_type(buffer_size()));
		}
		return *this;
	}

	self operator+(difference_type n) const
	{
		self tmp = *this;
		return tmp += n;
	}

	self& operator-=(difference_type n)
	{
		return *this += -n;
	}

	self operator-(difference_type n) const
	{
		self tmp = *this;
		return tmp -= n;
	}

	reference operator[](difference_type n) const
	{
		return *(*this + n);
	}

	bool operator==(const self& x) const
	{
		return cur == x.cur;
	}
	bool operator!=(const self& x) const
	{
		return !(*this == x);
	}
	bool operator<(const self& x) const
	{
		return (node == x.node) ? (cur < x.cur) : (node < x.node);
	}
};

template <typename T, typename Alloc = CAllocator<T>, size_t Bufsize = 0>
class CDeque
{
public:
	typedef T													value_type;
	typedef Alloc												allocator_type;
	typedef  typename allocator_type::pointer					pointer;
	typedef  typename allocator_type::const_pointer				const_pointer;
	typedef	 typename allocator_type::reference					reference;
	typedef  typename allocator_type::const_reference			const_reference;
	typedef  typename allocator_type::size_type					size_type;
	typedef  typename allocator_type::difference_type			difference_type;

	typedef __deque_iterator<T, T&, T*, Bufsize>				iterator;
	typedef __deque_iterator<T, const T&, const T*, Bufsize>	const_iterator;

protected:
	typedef pointer*		map_pointer;

	iterator	start;
	iterator	finish;
	map_pointer	map;
	size_type	map_size;
	CAllocator<T>	data_allocator;
	CAllocator<T*>	map_allocator;

public:
	iterator begin()
	{
		return start;
	}
	iterator end()
	{
		return finish;
	}
	reference operator[](size_type n)
	{
		return start[difference_type(n)];
	}
	reference front()
	{
		return *start;
	}
	reference back()
	{
		iterator tmp = finish;
		--tmp;
		return *tmp;
	}
	size_type size() const
	{
		return finish - start;
	}
	size_type max_size() const
	{
		return size_type(-1);
	}
	bool empty() const
	{
		return finish == start;
	}
	//初始状态下有一个缓冲区内存
	explicit CDeque(int n = 0)
		: start(), finish(), map(0), map_size(0)
	{
		fill_initialize(n, value_type());
	}

	CDeque(int n, const value_type& value)
		: start(), finish(), map(0), map_size(0)
	{
		fill_initialize(n, value);
	}

	void fill_initialize(size_type n, const value_type& value)
	{
		create_map_and_nodes(n);
		map_pointer cur;
		for (cur = start.node; cur < finish.node; ++cur)
		{
			::uninitialized_fill(*cur, *cur + iterator::buffer_size(), value);
		}
		::uninitialized_fill(finish.first, finish.cur, value);
	}

	void create_map_and_nodes(size_type num_elements)
	{
		size_type num_nodes = num_elements / iterator::buffer_size() + 1;

		map_size = max(INITIAL_MAP_SIZE, num_nodes + 2);
		map = map_allocator.allocate(map_size);
		map_pointer nstart = map + (map_size - num_nodes) / 2;
		map_pointer nfinish = nstart + num_nodes - 1;

		map_pointer cur;
		for (cur = nstart; cur <= nfinish; ++cur)
		{
			*cur = data_allocator.allocate(iterator::buffer_size());
		}

		start.set_node(nstart);
		finish.set_node(nfinish);
		start.cur = start.first;
		finish.cur = finish.first + num_elements % iterator::buffer_size();
	}

	void push_back(const value_type& t)
	{
		if (finish.cur != finish.last - 1)
		{
			data_allocator.construct(finish.cur++, t);
		}
		else //只剩一个元素备用空间
		{
			push_back_aux(t);
		}
	}

	void push_back_aux(const value_type& t)
	{
		value_type t_copy = t;
		reserve_map_at_back();
		*(finish.node + 1) = data_allocator.allocate(iterator::buffer_size());
		data_allocator.construct(finish.cur, t_copy);
		finish.set_node(finish.node + 1);
		finish.cur = finish.first;
	}

	void reserve_map_at_back(size_type nodes_to_add = 1)
	{
		//map上只剩最后一块内存
		if (nodes_to_add > map_size - (finish.node - map + 1))
		{
			reallocate_map(nodes_to_add, false);
		}
	}

	void push_front(const value_type& t)
	{
		if (start.cur != start.first)
		{
			data_allocator.construct(--start.cur, t);
		}
		else
		{
			push_front_aux(t);
		}
	}

	void push_front_aux(const value_type& t)
	{
		value_type t_copy = t;
		reserve_map_at_front();
		*(start.node - 1) = data_allocator.allocate(iterator::buffer_size());
		start.set_node(start.node - 1);
		start.cur = start.last - 1;
		data_allocator.construct(start.cur, t_copy);
	}

	void reserve_map_at_front(size_type nodes_to_add = 1)
	{
		//map上只剩最后一块内存
		if (nodes_to_add > start.node - map)
		{
			reallocate_map(nodes_to_add, true);
		}
	}

	void reallocate_map(size_type nodes_to_add, bool add_at_front)
	{
		size_type old_num_nodes = finish.node - start.node + 1;
		size_type new_num_nodes = old_num_nodes + nodes_to_add;

		map_pointer new_start;
		if (map_size > 2 * new_num_nodes)
		{
			new_start = map + (map_size - new_num_nodes) / 2
				+ (add_at_front ? nodes_to_add : 0);
			if (new_start < start.node)
				std::copy(start.node, finish.node + 1, new_start);
			else
				std::copy_backward(start.node, finish.node + 1, new_start + old_num_nodes);
		}
		else
		{
			size_type new_map_size = map_size + max(map_size, nodes_to_add) + 2;
			//配置一块新空间和map使用
			map_pointer new_map = map_allocator.allocate(new_map_size);
			new_start = new_map + (new_map_size - new_num_nodes) / 2
				+ (add_at_front ? nodes_to_add : 0);
			//把原map的内容拷贝过来
			copy(start.node, finish.node + 1, new_start);
			//释放原map
			map_allocator.deallocate(map, map_size);
			//设定新map的起始地址与大小
			map = new_map;
			map_size = new_map_size;
		}
		//重新设定迭代器start和finish
		start.set_node(new_start);
		finish.set_node(new_start + old_num_nodes - 1);
	}
	
	void pop_back()
	{
		if (finish.cur != finish.first)
		{
			//最后缓冲区有至少一个的元素
			--finish.cur;
			data_allocator.destory(finish.cur);
		}
		else
		{
			//最后缓冲区没有任何元素
			pop_back_aux();
		}
	}

	void pop_back_aux()
	{
		data_allocator.deallocate(finish.first, iterator::buffer_size());
		finish.set_node(finish.node - 1);
		finish.cur = finish.last - 1;
		data_allocator.destory(finish.cur);
	}

	void pop_front()
	{
		if (start.cur != start.last - 1)
		{
			//第一个缓冲区有一个以上的元素
			data_allocator.destory(start.cur);
			++start.cur;
		}
		else
		{
			//第一个缓冲区只有一个元素
			pop_front_aux();
		}
	}

	void pop_front_aux()
	{
		data_allocator.destory(start.cur);
		data_allocator.deallocate(start.first, iterator::buffer_size());
		start.set_node(start.node + 1);
		start.cur = start.first;
	}

	//最终需要保留一个缓冲区 deque的初始状态
	void clear()
	{
		//针对头尾以外的每一个缓冲区析构释放内存 它们一定都是饱满的
		for (map_pointer node = start.node + 1; node < finish.node; ++node)
		{
			data_allocator.destory(*node, *node + iterator::buffer_size());
			data_allocator.deallocate(*node, iterator::buffer_size());
		}
		//至少有头尾两个缓冲区
		if (start.node != finish.node)
		{
			data_allocator.destory(start.cur, start.last);
			data_allocator.destory(finish.first, finish.cur);
			//释放尾缓冲区 保留头缓冲区
			data_allocator.deallocate(finish.first, iterator::buffer_size());
		}
		else
		{
			data_allocator.destory(start.cur, finish.cur);
		}

		finish = start;
	}
	//清除pos所指的元素
	iterator erase(iterator pos)
	{
		iterator next = pos;
		++next;
		difference_type index = pos - start;
		//如果清除点之前的元素比较少 就移动清除点之前的元素
		if (index < (size() >> 1))
		{
			copy_backward(start, pos, next);
			//移动完毕 最前一个元素冗余 去除
			pop_front();
		}
		else //清除点之后的元素较少 就移动清除点之后的元素
		{
			copy(next, finish, pos);
			pop_back();
		}
		return start + index;
	}
	//清楚[first, last)区间内的所有元素
	iterator erase(iterator first, iterator last)
	{
		if (first == start && last == finish)
		{
			clear();
			return finish;
		}
		else
		{
			difference_type n = last - first; //清除区间的长度
			difference_type elems_before = first - start; //清除区间前方的元素个数
			//如果前方元素较少 则后移前方元素
			if (elems_before < (size() - n) / 2)
			{
				copy_backward(start, first, last);
				//标记deque的新起点
				iterator new_start = start + n;
				//移动完毕 将冗余的元素析构
				data_allocator.destory(start, new_start);
				//将以下冗余的缓冲区释放
				for (map_pointer cur = start.node; cur < new_start.node; ++cur)
				{
					data_allocator.deallocate(*cur, iterator::buffer_size());
				}
				start = new_start;
			}
			else //如果清除区间后方的元素比较少
			{
				//向前移动后方元素
				copy(last, finish, first);
				//标记deque的新尾点
				iterator new_finish = finish - n;
				//移动完毕，将冗余的元素析构
				data_allocator.destory(new_finish, finish);
				//将议下冗余的缓冲区释放
				for (map_pointer cur = new_finish.node + 1; node <= finish.node; ++cur)
				{
					data_allocator::deallocate(*cur, iterator::buffer_size());
				}
				finish = new_finish;
			}
			return start + elems_before;
		}
	}
	//在position处插入一个元素，其值为x
	iterator insert(iterator position, const value_type& x)
	{
		if (position.cur == start.cur)
		{
			push_front(x);
			return start;
		}
		else if (position.cur == finish.cur)
		{
			push_back(x);
			iterator tmp = finish;
			--tmp;
			return tmp;
		}
		else
		{
			return insert_aux(position, x);
		}
	}

	iterator insert_aux(iterator pos, const value_type& x)
	{
		difference_type index = pos - start;
		value_type x_copy = x;
		if (index < size() / 2)
		{//如果插入点之前的元素较少 在最前端加入与第一元素同值的元素
			push_front(front());
			iterator front1 = start;
			++front1;
			iterator front2 = front1;
			++front2;
			pos = start + index;
			iterator pos1 = pos;
			++pos1;
			copy(front2, pos1, front1);
		}
		else
		{//插入点之后的元素个数比较少
			push_back(back());
			iterator back1 = finish;
			--back1;
			iterator back2 = back1;
			--back2;
			pos = start + index;
			copy_backward(pos, back2, back1);
		}
		*pos = x_copy;
		return pos;
	}
};


#endif //__DEQUE_H__
