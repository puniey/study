#pragma once
#include "utility.h"

//插入一个元素至max-heap尾部 同时调整为max-heap
template <typename RandomAccessIterator, typename Compare>
inline void push_heap(RandomAccessIterator first, RandomAccessIterator last, Compare comp)
{
	//注意 新元素已置于底层容器的最尾端
	__push_heap_aux(first, last, distance_type(first), value_type(first), comp);
}

template <typename RandomAccessIterator, typename Distance, typename T, typename Compare>
inline void __push_heap_aux(RandomAccessIterator first, RandomAccessIterator last, Distance*, T*, Compare comp)
{
	__push_heap(first, Distance(last - first - 1), Distance(0), T(*(last - 1)), comp);
}

template <typename RandomAccessIterator, typename Distance, typename T, typename Compare>
void __push_heap(RandomAccessIterator first, Distance holeIndex, Distance topIndex, T value, Compare comp)
{
	Distance parent = (holeIndex - 1) / 2;
	while (holeIndex > topIndex && comp(*(first + parent), value))
	{//持续到顶端或满足heap的次序特性为止
		*(first + holeIndex) = *(first + parent);
		holeIndex = parent;
		parent = (holeIndex - 1) / 2;
	}
	*(first + holeIndex) = value;
}
//pop_heap算法 取走根节点 其实是根节点和尾节点互换 同时调整新的根节点 使其满足max-heap
template <typename RandomAccessIterator, typename Compare>
inline void pop_heap(RandomAccessIterator first, RandomAccessIterator last, Compare comp)
{
	__pop_heap_aux(first, last, value_type(first), comp);
}

template <typename RandomAccessIterator, typename T, typename Compare>
inline void __pop_heap_aux(RandomAccessIterator first, RandomAccessIterator last, T*, Compare comp)
{
	__pop_heap(first, last - 1, last - 1, T(*(last - 1)), distance_type(first), comp);
}

template <typename RandomAccessIterator, typename Distance, typename T, typename Compare>
inline void __pop_heap(RandomAccessIterator first, RandomAccessIterator last, RandomAccessIterator result, T value, Distance*, Compare comp)
{
	*result = *first;
	__adjust_heap(first, Distance(0), Distance(last - first), value, comp);
}


template <typename RandomAccessIterator, typename Distance, typename T, typename Compare>
void __adjust_heap(RandomAccessIterator first, Distance holeIndex, Distance len, T value, Compare comp)
{
	Distance topIndex = holeIndex;
	Distance secondChild = 2 * topIndex + 2;
	while (secondChild < len)
	{
		//比较左右两个子节点 选择值大的节点
		if (comp(*(first + secondChild), *(first + (secondChild - 1))))
			--secondChild;
		//上移大的子节点
		*(first + holeIndex) = *(first + secondChild);
		holeIndex = secondChild;
		secondChild = 2 * holeIndex + 2;
	}
	if (secondChild == len) //没有右节点
	{
		*(first + holeIndex) = *(first + secondChild - 1);
		holeIndex = secondChild - 1;
	}
	//将欲调整的值加入堆内
	__push_heap(first, holeIndex, topIndex, value, comp);
}
//sort_heap 前提是heap底部容器是max-heap
template <typename RandomAccessIterator, typename Compare>
void sort_heap(RandomAccessIterator first, RandomAccessIterator last, Compare comp)
{
	//每执行一次pop_heap，极值即被放在尾端，扣除尾端再执行pop_heap
	while (last > first)
	{
		::pop_heap(first, last--, comp);
	}
}
//make_heap 将一段现有数据转化为heap
template <typename RandomAccessIterator, typename Compare>
inline void make_heap(RandomAccessIterator first, RandomAccessIterator last, Compare comp)
{
	__make_heap(first, last, value_type(first), distance_type(first), comp);
}

template <typename RandomAccessIterator, typename T, typename Distance, typename Compare>
void __make_heap(RandomAccessIterator first, RandomAccessIterator last, T*, Distance*, Compare comp)
{
	//如果长度为0或1，不必重新排列
	if (last - first < 2)
		return;
	Distance len = last - first;
	Distance parent = (len - 2) / 2;
	while (true)
	{
		__adjust_heap(first, parent, len, *(first + parent), comp);
		if (parent == 0)
			break;
		--parent;
	}
}