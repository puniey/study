#pragma once
#include "allocator.h"
#include <vector>
#include <iterator>
#include <algorithm>
#include <utility>

using namespace std;

//查询质数
static const int __stl_num_primes = 28;
static const unsigned long __stl_prime_list[__stl_num_primes] =
{
	53, 97, 193, 389, 769, 1543, 3079, 6151, 12289, 24593, 49157, 98317, 196613, 393241, 786433, 
	1572869, 3145739, 6291469, 1258917, 25165843, 50331653, 100663319, 201326611, 402653189, 805306457,
	1610612741, 3221225473ul, 4294967291ul
};
//以上28个质数中，找出最接近并大于n的质数
inline unsigned long __stl_next_prime(unsigned long n)
{
	const unsigned long* first = __stl_prime_list;
	const unsigned long* last = __stl_prime_list + __stl_num_primes;
	const unsigned long* pos = lower_bound(first, last, n);
	return pos == last ? *(last - 1) : *pos;
}

//hash table 节点定义
template <typename Value>
struct __hashtable_node
{
	__hashtable_node(const Value& v) : val(v), next(0){}
	__hashtable_node(const __hashtable_node& other) : val(other.val), next(other.next){}
	__hashtable_node* next;
	Value val;
};

template <typename Value, typename Key, typename HashFunc, typename ExtractKey, typename EqualKey>
class hashtable;

//hash table 迭代器定义
template <typename Value, typename Key, typename HashFunc, typename ExtractKey, typename EqualKey>
struct __hashtable_iterator
{
	typedef hashtable<Value, Key, HashFunc, ExtractKey, EqualKey>			hashtable;
	typedef __hashtable_iterator<Value, Key, HashFunc, ExtractKey, EqualKey> iterator;
	typedef __hashtable_node<Value> node;

	typedef std::forward_iterator_tag	iterator_category;
	typedef Value						value_type;
	typedef Key							key_type;
	typedef ptrdiff_t					difference_type;
	typedef size_t						size_type;
	typedef Value&						reference;
	typedef Value*						pointer;

	node* cur;			//迭代器目前所指之节点
	hashtable* ht;		//保持对容器的连结关系，因为可能需要从bucket跳到bucket

	__hashtable_iterator(node* n, hashtable* tab) : cur(n), ht(tab) {}
	__hashtable_iterator() {}
	reference operator*() const
	{
		return cur->val;
	}
	pointer operator->() const
	{
		return &(operator*());
	}
	iterator& operator++()
	{
		const node* old = cur;
		cur = cur->next;
		if (!cur)
		{
			size_type bucket = ht->bkt_num(old->val);
			while (!cur && ++bucket < ht->buckets.size())
			{
				cur = ht->buckets[bucket];
			}
		}
		return *this;
	}
	iterator operator++(int)
	{
		iterator tmp = *this;
		++*this;
		return tmp;
	}
	bool operator==(const iterator& it) const
	{
		return cur == it.cur;
	}
	bool operator!=(const iterator& it) const
	{
		return cur != it.cur;
	}
};

//hashtable的数据结构
template <typename Value, typename Key, typename HashFunc, 
		  typename ExtractKey, typename EqualKey>
class hashtable
{
public:
	typedef HashFunc	hasher;
	typedef EqualKey	key_equal;
	typedef size_t		size_type;
	typedef Value		value_type;
	typedef Key			key_type;
	typedef __hashtable_node<Value>	node;
	typedef __hashtable_iterator<Value, Key, HashFunc, ExtractKey, EqualKey> iterator;

	friend struct iterator;

private:
	hasher				hash;
	key_equal			equals;
	ExtractKey			get_key;
	CAllocator<node>	node_allocator;
	vector<node*>		buckets;
	size_type			num_elements;

public:
	size_type bucket_count() const
	{
		return buckets.size();
	}
	size_type size() const
	{
		return num_elements;
	}
	//最大的buckets数量
	size_type max_bucket_count() const
	{
		return __stl_prime_list[__stl_num_primes - 1];
	}
	//配置节点
	node* new_node(const value_type& obj)
	{
		node* n = node_allocator.allocate(1);
		node_allocator.construct(n, obj);
		return n;
	}
	void delete_node(node* n)
	{
		node_allocator.destory(n);
		node_allocator.deallocate(n);
	}
	size_type next_size(size_type n) const
	{
		return __stl_next_prime(n);
	}
	//构造函数
	hashtable(size_type n = 0) : num_elements(0)
	{
		initialize_buckets(n);
	}
	void initialize_buckets(size_type n)
	{
		const size_type n_buckets = next_size(n);
		buckets.reserve(n_buckets);
		buckets.insert(buckets.end(), n_buckets, (node*)0);
		num_elements = 0;
	}
	iterator begin()
	{
		if (num_elements == 0)
			return iterator((node*)0, this);
		else
		{
			for (size_type i = 0; i < buckets.size(); ++i)
			{
				if (buckets[i])
				{
					return iterator((node*)buckets[i], this);
				}
			}
			return iterator((node*)0, this);
		}
	}
	iterator end()
	{
		return iterator((node*)0, this);
	}
	//查询元素的bucket索引
	size_type bkt_num(const value_type& obj, size_t n) const
	{
		return bkt_num_key(get_key(obj), n);
	}
	size_type bkt_num(const value_type& obj) const
	{
		return bkt_num_key(get_key(obj));
	}
	size_type bkt_num_key(const key_type& key) const
	{
		return bkt_num_key(key, buckets.size());
	}
	size_type bkt_num_key(const key_type& key, size_t n) const
	{
		return hash(key) % n;
	}
	//重建bucket底层的vector
	void resize(size_type num_elements_hint)
	{
		//元素数量超过vector的大小时 重新配置
		const size_type old_n = buckets.size();
		if (num_elements_hint > old_n)
		{
			const size_type n = next_size(num_elements_hint);
			if (n > old_n)
			{
				vector<node*> tmp(n, (node*)0);
				//处理每个旧的bucket
				for (size_type bucket = 0; bucket < old_n; ++bucket)
				{
					node* first = buckets[bucket];
					while (first)
					{
						//找出在新的bucket中的索引
						size_type new_bucket = bkt_num(first->val, n);
						buckets[bucket] = first->next;
						first->next = tmp[new_bucket];
						tmp[new_bucket] = first;
						first = buckets[bucket];
					}
				}
				buckets.swap(tmp);
			}
		}
	}
	//插入元素 不允许重复
	pair<iterator, bool> insert_unique(const value_type& obj)
	{
		resize(num_elements + 1); //判断是否需要重建表格 如需要就扩充
		return insert_unique_noresize(obj);
	}
	
	pair<iterator, bool> insert_unique_noresize(const value_type& obj)
	{
		const size_type n = bkt_num(obj);
		node* first = buckets[n];
		for (node* cur = first; cur; cur = cur->next)
		{
			if (equals(get_key(cur->val), get_key(obj)))
			{
				//如果键值相同 则不插入 返回
				return pair<iterator, bool>(iterator(cur, this), false);
			}
		}
		//产生新节点
		node* tmp = new_node(obj);
		tmp->next = first;
		buckets[n] = tmp;
		++num_elements;
		return pair<iterator, bool>(iterator(tmp, this), true);
	}
	//插入元素 允许重复
	iterator insert_equal(const value_type& obj)
	{
		resize(num_elements + 1);
		return insert_equal_noresize(obj);
	}
	iterator insert_equal_noresize(const value_type& obj)
	{
		const size_type n = bkt_num(obj);
		node* first = buckets[n];

		for (node* cur = first; cur; cur = cur->next)
		{
			//如果发现相同键值的元素 则插入
			if (equals(get_key(cur->val), get_key(obj)))
			{
				node* tmp = new_node(obj);
				tmp->next = cur->next;
				cur->next = tmp;
				++num_elements;
				return iterator(tmp, this);
			}
		}
		//没有重复键值
		node* tmp = new_node(obj);
		tmp->next = first;
		buckets[n] = tmp;
		++num_elements;
		return iterator(tmp, this);
	}
	void clear()
	{
		for (size_type i = 0; i < buckets.size(); ++i)
		{
			node* cur = buckets[i];
			while (cur != 0)
			{
				node* next = cur->next;
				delete_node(cur);
				cur = next;
			}
			buckets[i] = (node*)0;
		}
		num_elements = 0;
		//buckets vector 并未释放空间 仍保有原来的大小
	}
	void copy_from(const hashtable& ht)
	{
		buckets.clear();
		//如果已方空间大于对方，则不动，反之，就会增大
		buckets.reserve(ht.buckets.size());
		buckets.insert(buckets.end(), ht.buckets.size(), (node*)0);
		for (size_type i = 0; i < ht.buckets.size(); ++i)
		{
			if (const node* cur = ht.buckets[i])
			{
				node* copy = new_node(cur->val);
				buckets[i] = copy;
				for (node* next = cur->next; next; cur = next, next = cur->next)
				{
					copy->next = cur->next;
					copy = copy->next;
				}
			}
		}
		num_elements = ht.num_elements;
	}
	iterator find(const key_type& key)
	{
		size_type n = bkt_num_key(key);
		node* first;
		for (first = buckets[n]; first && !equals(get_key(first->val), key); first = first->next)
		{}
		return iterator(first, this);
	}
	size_type count(const key_type& key) const
	{
		const size_type n = bkt_num_key(key);
		size_type result = 0;

		for (const node* cur = buckets[n]; cur; cur = cur->next)
		{
			if (equals(get_key(cur->val), key))
			{
				++result;
			}
		}
		return result;
	}
};