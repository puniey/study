//红黑树的实现
//部分接口未完成  树的插入和平衡调整已完成
//红黑树定义：
//1.节点是红色或黑色
//2.根节点是黑色
//3.每个叶节点（NIL节点，空节点）是黑色的
//4.每个红色节点的两个子节点都是黑色。(从每个叶子到根的所有路径上不能有两个连续的红色节点)
//5.从任一节点到其每个叶子的所有路径都包含相同数目的黑色节点
//代码使用了一个技巧，定义了一个header的节点，parent指向根节点，left指向最左节点，right指向最右节点
#pragma once
#include <cstdlib>
#include <iterator>
#include "allocator.h"

using namespace std;

typedef bool __rb_tree_color_type;
const __rb_tree_color_type __rb_tree_red = false;
const __rb_tree_color_type __rb_tree_black = true;

struct __rb_tree_node_base
{
	typedef __rb_tree_color_type color_type;
	typedef __rb_tree_node_base* base_ptr;

	color_type color; //节点颜色，非红即黑
	base_ptr parent;
	base_ptr left;
	base_ptr right;

	static base_ptr minimun(base_ptr x)
	{
		while (x->left != NULL)
		{
			x = x->left;
		}
		return x;
	}

	static base_ptr maximum(base_ptr x)
	{
		while (x->right != NULL)
		{
			x = x->right;
		}
		return x;
	}
};

template <typename Value>
struct __rb_tree_node : public __rb_tree_node_base
{
	typedef __rb_tree_node<Value>* link_type;

	__rb_tree_node() : value_field(Value()) {}
	__rb_tree_node(Value v) : value_field(v) {}

	Value value_field;
};
//基层迭代器
struct __rb_tree_base_iterator
{
	typedef __rb_tree_node_base::base_ptr base_ptr;
	typedef std::bidirectional_iterator_tag iterator_category;
	typedef ptrdiff_t difference_type;

	base_ptr node;
	//operator++
	void increment()
	{
		if (node->right != NULL)
		{//存在右子树 则返回 右子树的最左节点
			node = node->right;
			while (node->left != NULL)
			{
				node = node->left;
			}
		}
		else
		{//不存在右子树 则查找父节点 如果node 为根节点或header节点 见特殊分支
			base_ptr y = node->parent;
			while (node == y->right)
			{
				node = y;
				y = y->parent;
			}
			if (node->right != y)
			{
				node = y;
			}
			else
			{
				//这个分支是为了 node为根节点，但是没有右子树的情况判断的 返回 header(end())
				//如果node 已经为header(end()) 返回值还是 header(end())
				//node 的父节点是header(end()),header的父节点是node
			}
		}
	}
	//operator--
	void decrement()
	{
		if (node->color == __rb_tree_red && node->parent->parent == node)
		{
			//这个特殊情况发生于node 为 header，header的右子树即为max节点
			node = node->right;
		}
		else if (node->left != NULL)
		{ //存在左子树 则返回则返回左子树的最右节点
			base_ptr y = node->left;
			while (y->right != NULL)
			{
				y = y->right;
			}
			node = y;
		}
		else
		{//不存在左子树 则查找父节点
			//如果node 为 root，返回还是root
			base_ptr y = node->parent;
			while (node == y->left)
			{
				node = y;
				y = y->parent;
			}
			node = y;
		}
	}

	friend bool operator==(__rb_tree_base_iterator x, __rb_tree_base_iterator y);
	friend bool operator!=(__rb_tree_base_iterator x, __rb_tree_base_iterator y);
};

bool operator==(__rb_tree_base_iterator x, __rb_tree_base_iterator y)
{
	return x.node == y.node;
}

bool operator!=(__rb_tree_base_iterator x, __rb_tree_base_iterator y)
{
	return !(x == y);
}

//RB-tree 迭代器
template <typename Value, typename Ref, typename Ptr>
struct __rb_tree_iterator : public __rb_tree_base_iterator
{
	typedef Value value_type;
	typedef Ref reference;
	typedef Ptr pointer;
	typedef __rb_tree_iterator<Value, Value&, Value*>	iterator;
	typedef __rb_tree_iterator<Value, const Value&, const Value*> const_iterator;
	typedef __rb_tree_iterator<Value, Ref, Ptr> self;
	typedef __rb_tree_node<Value>* link_type;

	__rb_tree_iterator() {}
	__rb_tree_iterator(link_type x)
	{
		node = x;
	}
	__rb_tree_iterator(const iterator& it)
	{
		node = it.node;
	}

	reference operator*() const
	{
		return ((link_type)node)->value_field;
	}

	pointer operator->() const
	{
		return &(operator*());
	}

	self& operator++()
	{
		increment();
		return *this;
	}

	self operator++(int)
	{
		self tmp = *this;
		increment();
		return tmp;
	}

	self& operator--()
	{
		decrement();
		return *this;
	}

	self operator--(int)
	{
		self tmp = *this;
		decrement();
		return *this;
	}
};
//全局函数 树形旋转
inline void __rb_tree_rotate_left(__rb_tree_node_base* x, __rb_tree_node_base*& root)
{
	//x为旋转点
	__rb_tree_node_base* y = x->right; //令y为旋转点的右子节点
	x->right = y->left;
	if (y->left != 0)
	{
		y->left->parent = x;
	}
	y->parent = x->parent;
	//令y完全顶替x的地位，必须将x对其父节点的关系完全接收过来
	if (x == root ) //x为根节点
	{
		root = y;
	}
	else if (x == x->parent->left) //x为其父节点的左子节点
	{
		x->parent->left = y;
	}
	else //x为其父节点的右子节点
	{
		x->parent->right = y;
	}
	y->left = x;
	x->parent = y;
}
inline void __rb_tree_rotate_right(__rb_tree_node_base* x, __rb_tree_node_base*& root)
{
	//x为旋转点
	__rb_tree_node_base* y = x->left;
	x->left = y->right;
	if (y->right != 0)
	{
		y->right->parent = x;
	}
	y->parent = x->parent;
	//令y完全顶替x的地位，必须将x对其父节点的关系完全接收过来
	if (x == root)
	{
		root = y;
	}
	else if (x == x->parent->right) // x为其父节点的右子节点
	{
		x->parent->right = y;
	}
	else // x为其父节点的左子节点
	{
		x->parent->left = y;
	}
	y->right = x;
	x->parent = y;
}
// 全局函数 重新令树形平衡(改变颜色及旋转树形)
// 参数1为新增节点，参数2为root
inline void __rb_tree_rebalance(__rb_tree_node_base* x, __rb_tree_node_base* &root)
{
	x->color = __rb_tree_red; //新节点必为红
	while (x != root && x->parent->color == __rb_tree_red) //父节点为红
	{
		if (x->parent == x->parent->parent->left) // 父节点为祖父节点之左子节点
		{
			__rb_tree_node_base* y = x->parent->parent->right; //令y为伯父节点
			if (y && y->color == __rb_tree_red) //伯父节点存在，且为红
			{
				x->parent->color = __rb_tree_black; //更改父节点为黑
				y->color = __rb_tree_black; //更改伯父节点为黑
				x->parent->parent->color = __rb_tree_red; //更改祖父节点为红
				x = x->parent->parent;
			}
			else // 无伯父节点或伯父节点为黑
			{
				if (x == x->parent->right) //如果新节点为父节点之右子节点
				{
					x = x->parent;
					__rb_tree_rotate_left(x, root); //第一个参数为左旋点
				}
				x->parent->color = __rb_tree_black; //改变颜色
				x->parent->parent->color = __rb_tree_red;
				__rb_tree_rotate_right(x->parent->parent, root); //第一个参数为右旋点
			}
		}
		else // 父节点为祖父节点之右子节点
		{
			__rb_tree_node_base* y = x->parent->parent->left; //令y为伯父节点
			if (y && y->color == __rb_tree_red) // 有伯父节点，且为红
			{
				x->parent->color = __rb_tree_black; //更改父节点为黑
				y->color = __rb_tree_black; //更改伯父节点为黑
				x->parent->parent->color = __rb_tree_red; //更改祖父节点为红
				x = x->parent->parent; //准备继续往上层检查
			}
			else //无伯父节点，或伯父节点为黑
			{
				if (x == x->parent->left) // 如果新节点为父节点之左子节点
				{
					x = x->parent;
					__rb_tree_rotate_right(x, root); // 第一参数为右旋点
				}
				x->parent->color = __rb_tree_black;
				x->parent->parent->color = __rb_tree_red;
				__rb_tree_rotate_left(x->parent->parent, root); // 第一参数为左旋点
			}
		}
	} //while循环结束
	root->color = __rb_tree_black;
}

template <typename Key, typename Value, typename KeyOfValue, typename Compare>
class rb_tree
{
protected:
	typedef void*						void_pointer;
	typedef __rb_tree_node_base*		base_ptr;
	typedef __rb_tree_node<Value>		rb_tree_node;
	typedef CAllocator<rb_tree_node>	rb_tree_node_allocator;
	typedef __rb_tree_color_type		color_type;

public:
	typedef Key						key_type;
	typedef Value					value_type;
	typedef value_type*				pointer;
	typedef const value_type*		const_pointer;
	typedef value_type&				reference;
	typedef const value_type&		const_reference;
	typedef rb_tree_node*			link_type;
	typedef size_t					size_type;
	typedef ptrdiff_t				defference_type;
	typedef __rb_tree_iterator<value_type, reference, pointer>				iterator;
	typedef __rb_tree_iterator<value_type, const_reference, const_pointer>	const_iterator;

protected:
	link_type get_node()
	{
		return alloc.allocate(1);
	}

	void put_node(link_type p)
	{
		alloc.deallocate(p, sizeof(rb_tree_node));
	}

	link_type create_node(const value_type& x)
	{
		link_type tmp = get_node(); //配置空间
		try
		{
			alloc.construct(tmp, x); //构造内容
		}
		catch (const std::exception&)
		{
			put_node(tmp);
		}
		return tmp;
	}

	link_type clone_node(link_type x) //复制一个节点
	{
		link_type tmp = create_node(x->value_field);
		tmp->color	= x->color;
		tmp->left	= 0;
		tmp->right	= 0;
		tmp->parent = 0;
		return tmp;
	}

	void destory_node(link_type p)
	{
		alloc.destory(&p->value_field);
		put_node(p);
	}

protected:
	size_type	node_count;		//节点数量
	link_type	header;			//头节点
	Compare		key_compare;	//节点间的键值大小比较准则 应该会是个functor
	rb_tree_node_allocator alloc; //节点内存配置器

	link_type& root() const
	{
		return (link_type&)header->parent;
	}

	link_type& leftmost() const
	{
		return (link_type&)header->left;
	}

	link_type& rightmost() const
	{
		return (link_type&)header->right;
	}

	static link_type& left(link_type x)
	{
		return (link_type&)(x->left);
	}

	static link_type& right(link_type x)
	{
		return (link_type&)(x->right);
	}

	static link_type& parent(link_type x)
	{
		return (link_type&)(x->parent);
	}

	static reference value(link_type x)
	{
		return x->value_field;
	}

	static const Key& key(link_type x)
	{
		return KeyOfValue()(value(x));
	}

	static color_type& color(link_type x)
	{
		return (color_type&)(x->color);
	}

	static link_type minimun(link_type x)
	{
		return (link_type)__rb_tree_node_base::minimun(x);
	}

	static link_type maximum(link_type x)
	{
		return (link_type)__rb_tree_node_base::maximum(x);
	}

private:
	iterator __insert(base_ptr x_, base_ptr y_, const value_type& v)
	{//参数x_为新值插入点，参数y_为插入点之父节点，参数v为新值
		link_type x = (link_type)x_;
		link_type y = (link_type)y_;
		link_type z;

		if (y == header || x != 0 || key_compare(KeyOfValue()(v), key(y)))
		{
			z = create_node(v);
			left(y) = z;
			if (y == header)
			{
				root() = z;
				rightmost() = z;
			}
			else if (y == leftmost())
			{
				leftmost() = z;
			}
		}
		else
		{
			z = create_node(v);
			right(y) = z;
			if (y ==rightmost())
			{
				rightmost() = z;
			}
		}

		parent(z) = y;
		left(z) = 0;
		right(z) = 0;

		__rb_tree_rebalance(z, header->parent); //参数1 为新增节点，参数2 为root
		++node_count; //节点数累加
		return iterator(z); //返回一个迭代器，指向新增节点
	}

	link_type __copy(link_type x, link_type p)
	{
		//未实现
	}
	void __erase(link_type x)
	{
		//未实现
	}
	void init()
	{
		header = get_node();
		color(header) = __rb_tree_red;
		root() = 0;
		leftmost() = header;
		rightmost() = header;
	}

public:
	rb_tree(const Compare& comp = Compare()) : node_count(0), key_compare(comp)
	{
		init();
	}
	~rb_tree()
	{
		clear();
		put_node(header);
	}
	rb_tree<Key, Value, KeyOfValue, Compare>&
		operator=(const rb_tree<Key, Value, KeyOfValue, Compare>& x)
	{
		//未实现
	}

	void clear()
	{
		//未实现
	}

public:
	Compare key_comp() const
	{
		return key_compare;
	}
	iterator begin()
	{
		return leftmost();
	}
	iterator end()
	{
		return header;
	}
	bool empty()
	{
		return node_count == 0;
	}
	size_type size() const
	{
		return node_count;
	}
	size_type max_size() const
	{
		return size_type(-1);
	}
	//允许节点键值重复
	//返回一个RB-tree迭代器，指向新增节点
	iterator insert_equal(const value_type& x)
	{
		link_type y = header;
		link_type x = root();
		while (x ! = 0) //从根节点开始寻找适当的插入点
		{
			y = x;
			x = key_compare(KeyOfValue()(v), key(x)) ? left(x) : right(x);
		}
		//x为新值插入点，y为插入点之父节点，v为新值
		return __insert(x, y, v);
	}
	//节点键值不允许重复
	//返回pair，第一个元素是RB-tree 迭代器，指向新增节点 第二个元素表示插入成功与否
	pair<iterator, bool> insert_unique(const value_type& v)
	{
		link_type y = header;
		link_type x = root();
		bool comp = true;
		while (x != 0)
		{
			y = x;
			comp = key_compare(KeyOfValue()(v), key(x));
			x = comp ? left(x) : right(x);
		}
		//离开while循环后，y所指即插入点之父节点，此时的它必为叶节点
		iterator j = iterator(y);
		if (comp) // 新节点插于y的左侧
		{
			if (j == begin()) //如果插入点之父节点为最左节点
			{
				return pair<iterator, bool>(__insert(x, y, v), true);
			}
			else //否则调整j，回头一步准备测试
			{
				--j;
			}
		}
		if (key_compare(key((link_type)j.node), KeyOfValue()(v)))
		{
			return pair<iterator, bool>(__insert(x, y, v), true);
		}
		else //新值与树中键值重复
		{
			return pair<iterator, bool>(j, false);
		}
	}
};