#ifndef _ZSTL_RBTREE_H_
#define _ZSTL_RBTREE_H_

#include <iostream>
#include "00zstl_alloc.h"
#include "00zstl_construct.h"
#include "00zstl_algobase.h"
#include <utility>		//pair

__MYSTL_NAMESPACE_BEGIN_
	typedef bool __rb_tree_color_type;
	const __rb_tree_color_type __rb_tree_red = false;	//红色为0
	const __rb_tree_color_type __rb_tree_black = true;	//黑色为1

	//RB-tree 的基本节点结构
	struct __rb_tree_node_base {
		typedef __rb_tree_color_type color_type;
		typedef __rb_tree_node_base* base_ptr;

		color_type	color;	//节点颜色，非红即黑
		base_ptr	parent;	//RB 树的许多操作，必须知道父节点
		base_ptr	left;	//指向左节点
		base_ptr	right;	//指向右节点

		static base_ptr minimum(base_ptr x) {
			if (!x) return nullptr;

			while (x->left) x = x->left;	//一直向左走，就会找到最小值
			return x;
		}

		static base_ptr maximum(base_ptr x) {
			if (!x) return nullptr;

			while (x->right) x = x->right;	//一直向右走，就会找到最大值
			return x;
		}
	};

	//RB-tree 的节点结构
	template <class Value>
	struct __rb_tree_node : public __rb_tree_node_base {
		typedef __rb_tree_node<Value>* link_type;
		Value value_field;	//节点值
	};

	//RB-tree 的基层迭代器
	struct __rb_tree_base_iterator {
		typedef __rb_tree_node_base::base_ptr	base_ptr;
		typedef bidirectional_iterator_tag		iterator_category;
		typedef ptrdiff_t						difference_type;

		base_ptr node;	//用来与容器之间产生一个连结关系(make a reference)

		void increment() {
			if (node->right) {				//如果有右子节点 (1)
				node = node->right;			//向右走
				while (node->left) {		//然后一直往左子树走到底，即是解答
					node = node->left;
				}
			}
			else {							//没有右子节点 (2)
				base_ptr _parent = node->parent;	//找出父节点
				while (node == _parent->right) {	//如果现行节点本身是个右子节点
					node = _parent;				//就一直上溯，直到“不为右子节点”为止
					_parent = node->parent;
				}
				if (node->right != _parent) {		//若此时的右子节点不等于此时的父节点
					node = _parent;				//此时的父节点即为解答 (3)，否则此时的 node 为解答 (4)
				}
			}
			/*“若此时的右子节点不等于此时的父节点”判断是为了应付一种特殊情况：
				欲寻找根节点的下一节点，而恰巧根节点无右子节点
				以上做法必须配合 RB-tree 根节点与特殊的 header 之间的特殊关系
			*/
		}

		void decrement() {
			if (node->color == __rb_tree_red &&		//如果是红节点，且
				node->parent->parent == node) {		//父节点的父节点等于自己
				node = node->right;					//(1) 右子节点即为解答
			}
			//以上情况发生于 node 为 header 时(亦即 node 为 end() 时)
			//header 的右子节点即为 mostright，指向整棵树的 max 节点
			else if (node->left) {					//如果有左子节点 (2)
				node = node->left;					//令 node 指向左子节点
				while (node->right) {					//当 node 有右子节点时
					node = node->right;					//一直往右子节点走到底
				}
			}
			else {									//既非根节点，亦无左子节点
				base_ptr _parent = node->parent;			//(3) 找出父节点
				while (node == _parent->left) {			//当现行节点身为左子节点
					node = _parent;						//一直交替往上走，直到现行节点
					_parent = _parent->parent;					//不为左子节点
				}
				node = _parent;							//此时的父节点即为答案
			}
		}
	};

	//RB-tree 的正规迭代器
	template <class Value, class Ref, class Ptr>
	struct __rb_tree_iterator : public __rb_tree_base_iterator {
		typedef Value	value_type;
		typedef Ref		reference;
		typedef Ptr		pointer;
		typedef __rb_tree_iterator<Value, Value&, Value*>				iterator;
		typedef __rb_tree_iterator<Value, const Value&, const Value*>	const_iterator;
		typedef __rb_tree_iterator<Value, Ref, Ptr>						self;
		typedef __rb_tree_node<Value>*									link_type;

		__rb_tree_iterator() : node(nullptr) { }
		__rb_tree_iterator(link_type x) { node = x; }
		__rb_tree_iterator(const iterator& it) { node = it.node; }

		reference operator*() const { return link_type(node)->value_field; }
#ifndef __SGI_STL_NO_ARROW_OPERATOR
		pointer operator->() const { return &(operator*()); }
#endif /* __SGI_STL_NO_ARROW_OPERATOR */

		self& operator++() { increment(); return *this; }
		self operator++(int) {
			self tmp = *this;
			increment();
			return tmp;
		}

		self& operator--() { decrement(); return *this; }
		self operator--(int) {
			self tmp = *this;
			decrement();
			return tmp;
		}
	};

	inline bool operator==(const __rb_tree_base_iterator& _x,
		const __rb_tree_base_iterator& _y) {
		return _x.node == _y.node;
	}
	inline bool operator!=(const __rb_tree_base_iterator& _x,
		const __rb_tree_base_iterator& _y) {
		return _x.node != _y.node;
	}

	//RB-tree 的数据结构
	// Key:			键值型别 
	// Value:		元素型别
	// KeyOfValue:	键值提取(根据实值)
	// Compare:		键值比较规则
	template <class Key, class Value, class KeyOfValue, class Compare = less<int>, class Alloc = alloc> 
	class rb_tree {
	protected:
		typedef void*								void_pointer;
		typedef __rb_tree_node_base*				base_ptr;
		typedef __rb_tree_node<Value>				rb_tree_node;
		typedef simple_alloc<rb_tree_node, Alloc>	rb_tree_node_allocator;
		typedef __rb_tree_color_type				color_type;

	public:
		typedef Key					key_type;
		typedef Value				value_type;
		typedef value_type*			pointer;
		typedef const value_type*	const_pointer;
		typedef value_type&			reference;
		typedef	const value_type&	const_reference;
		typedef rb_tree_node*		link_type;
		typedef size_t				size_type;
		typedef ptrdiff_t			difference_type;
		typedef typename __rb_tree_iterator<value_type, reference, pointer>::iterator iterator;
		typedef typename __rb_tree_iterator<value_type, reference, pointer>::const_iterator const_iterator;
	protected:
		link_type get_node() { return rb_tree_node_allocator::allocate(); }
		void put_node(link_type p) { rb_tree_node_allocator::deallocate(p); }

		link_type create_node(const value_type& x) {
			link_type tmp = get_node();		//配置空间
			try {
				construct(&tmp->value_field, x);	//构造内容
			}
			catch (...) {
				put_node(tmp);
			}
			return tmp;
		}

		link_type clone_node(link_type x) {			//复制一个节点(的值和颜色)
			link_type tmp = create_node(x->value_field);
			tmp->color = x->color;
			tmp->left = nullptr;
			tmp->right = nullptr;
			return tmp;
		}

		void destroy_node(link_type x) {
			destroy(&x->value_field);		//析构内容
			put_node(x);					//释放内容
		}

	protected:
		//RB-tree 只以三笔数据表现
		size_type	node_count;		//追踪记录树的大小(节点数量)
		link_type	header;			//头指针，其 parent 和 root 互指，其 left 指向最小节点，right 指向最大节点
		Compare		key_compare;	//节点间的键值大小比较准则

		//以下三个函数用来方便取得 header 的成员
		link_type& root() const { return (link_type&)header->parent; }
		link_type& leftmost() const { return (link_type&)header->left; }
		link_type& rightmost() const { return (link_type&)header->right; }

		//以下六个函数用来方便取得节点 x 的成员
		static link_type& left(link_type x) { return (link_type&)(x->left); }
		static link_type& right(link_type x) { return (link_type&)(x->right); }
		static link_type& parent(link_type x) { return (link_type&)(x->parent);	}
		static reference value(link_type x) { return x->value_field; }
		static const Key& key(link_type x) { return KeyOfValue()(value(x));	}
		static color_type& color(link_type x) { return (color_type&)(x->color);	}

		//以下六个函数用来方便取得节点 x 的成员
		static link_type& left(base_ptr x) { return (link_type&)(x->left); }
		static link_type& right(base_ptr x) { return (link_type&)(x->right); }
		static link_type& parent(base_ptr x) { return (link_type&)(x->parent); }
		static reference value(base_ptr x) { return ((link_type)x)->value_field; }
		static const Key& key(base_ptr x) { return KeyOfValue()(value(x)); }
		static color_type& color(base_ptr x) { return (color_type&)(x->color); }

		//求取极大值和极小值，node class 有实现此功能，交给它们完成即可
		static link_type minimum(link_type x) {
			return (link_type)__rb_tree_node_base::minimum(x);
		}
		static link_type maximum(link_type x) {
			return (link_type)__rb_tree_node_base::maximum(x);
		}		

	public:
		// allocation / deallocation
		rb_tree(const Compare& comp = Compare()) : node_count(0), key_compare(comp) { init(); }
		rb_tree(const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& x) {		//拷贝构造函数
			init();
			*this = x;		//调用 operator=
		}		
		~rb_tree() {
			clear();
			put_node(header);
		}
		rb_tree<Key, Value, KeyOfValue, Compare, Alloc>&
			operator = (const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& x);

		void init() {
			header = get_node();			//产生一个节点空间，令 header 指向它
			color(header) = __rb_tree_red;	//令 header 为红色，用来区分 header 和 root，在 iterator.operator++ 之中
			root() = nullptr;
			leftmost() = header;			//令 header 的左子节点为自己
			rightmost() = header;			//令 header 的右子节点为自己
		}

		void clear() {
			if (node_count) {
				erase(root());
				leftmost() = header;
				root() = nullptr;
				rightmost() = header;
				node_count = 0;
			}
		}

	public:
		// accessors
		Compare key_comp() const { return key_compare; }
		iterator begin() { return leftmost(); }		//RB 树的起头为最左(最小)节点处
		const_iterator begin() const { return leftmost(); }
		iterator end() { return header; }			//RB 树的终点为 header 所指处
		const_iterator end() const { return header; }
		bool empty() const { return node_count == 0; }
		size_type size() const { return node_count; }
		size_type max_size() const { return size_type(-1); }

	private:
		// insert / erase
		iterator __insert(base_ptr x_, base_ptr y_, const Value& v);
		base_ptr __rebalance_erase(base_ptr p, base_ptr& root, base_ptr& leftmost, base_ptr& rightmost);

		link_type __copy(link_type x);

	public:
		// insert / erase
		std::pair<iterator, bool> insert_unique(const value_type& x);
		iterator insert_equal(const value_type& x);
		void erase(iterator p);

		iterator find(const Key& x);
		size_type count(const Key& x) {
			size_type _count = 0;
			iterator it = find(x);
			
			if (it != end()) {
				++_count;
				for (++it; it != end() && !key_compare(key(it.node), x) && !key_compare(x, key(it.node)); ++it) {
					++_count;
				}
			}

			return _count;
		}
	};

	//插入新值：节点键值允许重复
	//注意，返回值是一个 RB-tree 迭代器，指向新增节点
	template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
	typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
		rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::insert_equal(const value_type& v) {
		link_type y = header;
		link_type x = root();		//从根节点开始
		while (x) {			//往下寻找适当的插入点
			y = x;
			x = key_compare(KeyOfValue()(v), key(x)) ? left(x) : right(x);
			//以上，遇“大”则往左，遇“小于或等于”则往右
		}
		return __insert(x, y, v);
		//以上，x 为新值插入点，y 为插入点的父节点，v 为新值
	}

	//插入新值：节点键值不允许重复，若重复则插入无效
	//注意，返回值是个 pair，第一元素是个 RB-tree 迭代器，指向新增节点，第二元素表示插入成功与否
	template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
	std::pair<typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator,bool>
		rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::insert_unique(const value_type& v) {
		link_type y = header;
		link_type x = root();	//从根节点开始
		bool comp = true;
		while (x) {		//往下寻找适当的插入点
			y = x;
			comp = key_compare(KeyOfValue()(v), key(x));		//v 键值小于目前节点的键值？
			x = comp ? left(x) : right(x);		//遇“大”则往左，遇“小或等于”则往右
		}
		//离开 while 循环之后，y 所指即插入点的父节点(此时的它必为叶节点)

		iterator j = iterator(y);		//令迭代器 j 指向插入点的父节点 y
		if (comp) {						//如果离开 while 循环时 comp 为真(表示遇“大”，将插入于左侧)
			if (j == begin()) {			//如果插入点的父节点为最左节点
				return std::pair<iterator, bool>(__insert(x, y, v), true);
				//以上，x 为插入点，y 为插入点的父节点，v 为新值
			}
			else {						//否则(插入点的父节点不为最左节点
				--j;					//调整 j，回头准备测试
			}
		}
		if (key_compare(key(j.node), KeyOfValue()(v))) {
			//小于新值(表示遇“小”，将插入于右侧
			return std::pair<iterator, bool>(__insert(x, y, v), true);
			//以上，x 为新值插入点， y 为插入点的父节点，v 为新值
		}

		//进行至此，表示新值一定与树中键值重复，就不应该插入新值
		return std::pair<iterator, bool>(j, false);
	}

	//真正的插入执行程序 __insert()
	template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
	typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator 
		rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::__insert(base_ptr _x, base_ptr _y, const Value& v) {
		//参数 x_ 为新值插入点，参数 y_ 为插入点的父节点，参数 v 为新值
		link_type x = (link_type)_x;
		link_type y = (link_type)_y;
		link_type z;

		//key_compare 是键值大小比较准则
		if (y == header || x || key_compare(KeyOfValue()(v), key(y))) {
			z = create_node(v);			//产生一个新节点
			left(y) = z;				//使得当 y 即为 header 时，leftmost() = z
			if (y == header) {
				root() = z;
				rightmost() = z;
			}
			else if (y == leftmost()) {	//如果 y 为最左节点
				leftmost() = z;			//维护 leftmost()，使它永远指向最左节点
			}
		}
		else {
			z = create_node(v);			//产生一个新节点
			right(y) = z;				//令新节点成为插入点的父节点 y 的右子节点
			if (y == rightmost()) {
				rightmost() = z;		//维护 rightmost()，使它永远指向最右节点
			}
		}
		parent(z) = y;		//设定新节点的父节点
		left(z) = nullptr;		//设定新节点的左子节点
		right(z) = nullptr;		//设定新节点的右子节点
							//新节点的颜色在 rebalence 设定并调整
		__rb_tree_rebalance(z, header->parent);		//参数一为新增节点，参数二为 root
		++node_count;			//节点数增加
		return iterator(z);		//返回一个迭代器，指向新增节点
	}

	//全局函数：左旋转
	//新节点必为红节点，如果插入处的父节点也是红节点
	//则违反红黑树规则，必须做树形旋转
	inline void __rb_tree_rotate_left(__rb_tree_node_base* x, __rb_tree_node_base*& root) {
		// x 为旋转点
		__rb_tree_node_base* y = x->right;	//令 y 为旋转点的右子节点
		x->right = y->left;
		if (y->left) {
			y->left->parent = x;			//回马枪设定父节点
		}
		y->parent = x->parent;

		//令 y 完全顶替 x 的地位(必须将 x 对其父节点的关系完全接收过来)
		if (x == root) {					// x 为根节点
			root = y;
		}
		else if (x == x->parent->left) {	// x 为其父节点的左子节点
			x->parent->left = y;
		}
		else {								// x 为其父节点的右子节点
			x->parent->right = y;
		}
		y->left = x;
		x->parent = y;
	}

	//全局函数：右旋转
	//新节点必为红节点，如果插入处的父节点也是红节点
	//则违反红黑树规则，必须做树形旋转
	inline void __rb_tree_rotate_right(__rb_tree_node_base* x, __rb_tree_node_base*& root) {
		// x 为旋转点
		__rb_tree_node_base* y = x->left;	//令 y 为旋转点的左子节点
		x->left = y->right;
		if (y->right) {
			y->right->parent = x;			//回马枪设定父节点
		}
		y->parent = x->parent;

		//令 y 完全顶替 x 的地位(必须将 x 对其父节点的关系完全接收过来)
		if (x == root) {					// x 为根节点
			root = y;
		}
		else if (x == x->parent->right) {	// x 为其父节点的右子节点
			x->parent->right = y;
		}
		else {								// x 为其父节点的左子节点
			x->parent->left = y;
		}
		y->right = x;
		x->parent = y;
	}

	//全局函数：平衡树
	//重新令树平衡(改变颜色及旋转树形)
	//参数一为新增节点，参数二为 root
	inline void __rb_tree_rebalance(__rb_tree_node_base* x, __rb_tree_node_base*& root) {
		x->color = __rb_tree_red;		//新节点比为红
		while (x != root && x->parent->color == __rb_tree_red) {	//父节点为红
			if (x->parent == x->parent->parent->left) {				//父节点为祖父节点的左节点
				__rb_tree_node_base* y = x->parent->parent->right;	//令 y 为伯父节点
				if (y && y->color == __rb_tree_red) {				//伯父节点存在，且为红
					x->parent->color = __rb_tree_black;				//更改父节点为黑
					y->color = __rb_tree_black;						//更改伯父节点为黑
					x->parent->parent->color = __rb_tree_red;		//更改祖父节点为红
					x = x->parent->parent;
				}
				else {												//无伯父节点，或伯父节点为黑
					if (x == x->parent->right) {					//如果新节点为父节点的右子节点
						x = x->parent;
						__rb_tree_rotate_left(x, root);				//第一参数为左旋点
					}
					x->parent->color = __rb_tree_black;				//改变颜色
					x->parent->parent->color = __rb_tree_red;
					__rb_tree_rotate_right(x->parent->parent, root);//第一参数为右旋点
				}
			}
			else {													//父节点为祖父节点的右子节点
				__rb_tree_node_base* y = x->parent->parent->left;	//令 y 为伯父节点
				if (y && y->color == __rb_tree_red) {				//伯父节点存在，且为红
					x->parent->color = __rb_tree_black;				//更改父节点为黑
					y->color = __rb_tree_black;						//更改伯父节点为黑
					x->parent->parent->color = __rb_tree_red;		//更改祖父节点为红
					x = x->parent->parent;
				}
				else {												//无伯父节点，或伯父节点为黑
					if (x == x->parent->left) {					//如果新节点为父节点的右子节点
						x = x->parent;
						__rb_tree_rotate_right(x, root);			//第一参数为右旋点
					}
					x->parent->color = __rb_tree_black;				//改变颜色
					x->parent->parent->color = __rb_tree_red;
					__rb_tree_rotate_left(x->parent->parent, root);//第一参数为左旋点
				}
			}
		}// while 结束
		root->color = __rb_tree_black;		//根节点永远为黑
	}

	template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
	typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
		rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::find(const Key& x) {
		link_type _head = header;		//最后一个不比 x 小的节点
		link_type cur = root();			//当前节点

		while (cur) {
			if (!key_compare(key(cur), x)) {
				//进行到这里，表示 cur 键值大于 x ，遇到大值就向左走
				_head = cur;
				cur = left(cur);
			}
			else {
				//进行到这里，表示 cur 键值小于 x ，遇到小值就向左走
				cur = right(cur);
			}
		}

		iterator j = iterator(_head);
		return (j == end() || key_compare(x, key(j.node))) ? end() : j;
	}

	template <class Key, class Value, class KeyOfValue, class Compare, class Alloc> 
	typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::link_type
		rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::__copy(link_type x) {
		if (!x) {
			return nullptr;
		}
		link_type x_copy = clone_node(x);
		x_copy->left = __copy(left(x));
		x_copy->right = __copy(right(x));

		if (x_copy->left) {
			x_copy->left->parent = x_copy;
		}
		if (x_copy->right) {
			x_copy->right->parent = x_copy;
		}

		return x_copy;
	}

	template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
	void rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::erase(iterator p) {
		link_type _y = (link_type)__rebalance_erase(p.node, header->parent, header->left, header->right);
		destroy_node(_y);
		--node_count;
	}

	template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
	typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::base_ptr
		rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::__rebalance_erase(base_ptr p, base_ptr& root, base_ptr& leftmost, base_ptr& rightmost) {
		base_ptr _y = p;
		base_ptr _x = nullptr;
		base_ptr _x_parent = nullptr;

		if (_y->left == nullptr) {
			_x = _y->right;
		}
		else {
			if (_y->right == 0) {
				_x = _y->left;
			}
			else {
				_y = _y->right;
				while (_y->left) {
					_y = _y->left;
				}
				_x = _y->right;
			}
		}
		if (_y != p) {
			p->left->parent = _y;
			_y->left = p->left;
			if (_y != p->parent) {
				_x_parent = _y->parent;
				if (_x) _x->parent = _y->parent;
				_y->parent->left = _x;
				_y->right = p->right;
				p->right->parent = _y;
			}
			else {
				_x_parent = _y;
			}
			if (root == p) {
				root = _y;
			}
			else if (p->parent->left == p) {
				p->parent->left = _y;
			}
			else {
				p->parent->right = _y;
			}
			_y->parent = p->parent;
			std::swap(_y->color, p->color);
			_y = p;
		}
		else {
			_x_parent = _y->parent;
			if (_x) _x->parent = _y->parent;
			if (root == p) {
				root = _x;
			}
			else {
				if (p->parent->left == p) p->parent->left = _x;
				else p->parent->right = _x;
			}
			if (leftmost == p) {
				if (p->right == 0) 	leftmost = p->parent;
				else leftmost = __rb_tree_node_base::minimum(_x);
			}
			if (rightmost == p) {
				if (p->left == 0) rightmost = p->parent;
				else rightmost = __rb_tree_node_base::maximum(_x);
			}
		}
		if (_y->color != __rb_tree_red) {
			while (_x != root && (_x == 0 || _x->color == __rb_tree_black)) {
				if (_x == _x_parent->left) {
					base_ptr _w = _x_parent->right;
					if (_w->color == __rb_tree_red) {
						_w->color = __rb_tree_black;
						_x_parent->color = __rb_tree_red;
						__rb_tree_rotate_left(_x_parent, root);
						_w = _x_parent->right;
					}
					if ((_w->left == 0 ||
							_w->left->color == __rb_tree_black) &&
						(_w->right == 0 ||
							_w->right->color == __rb_tree_black)) {
						_w->color = __rb_tree_red;
						_x = _x_parent;
						_x_parent = _x_parent->parent;
					}
					else {
						if (_w->right == 0 ||
							_w->right->color == __rb_tree_black) {
							if (_w->left) _w->left->color = __rb_tree_black;
							_w->color = __rb_tree_red;
							__rb_tree_rotate_right(_w, root);
							_w = _x_parent->right;
						}
						_w->color = _x_parent->color;
						_x_parent->color = __rb_tree_black;
						if (_w->right) _w->right->color = __rb_tree_black;
						__rb_tree_rotate_left(_x_parent, root);
						break;
					}
				}
				else {
					base_ptr _w = _x_parent->left;
					if (_w->color == __rb_tree_red) {
						_w->color = __rb_tree_black;
						_x_parent->color = __rb_tree_red;
						__rb_tree_rotate_right(_x_parent, root);
						_w = _x_parent->left;
					}
					if ((_w->right == 0 ||
							_w->right->color == __rb_tree_black) &&
						(_w->left == 0 ||
							_w->left->color == __rb_tree_black)) {
						_w->color = __rb_tree_red;
						_x = _x_parent;
						_x_parent = _x_parent->parent;
					}
					else {
						if (_w->left == 0 ||
							_w->left->color == __rb_tree_black) {
							if (_w->right) _w->right->color = __rb_tree_black;
							_w->color = __rb_tree_red;
							__rb_tree_rotate_left(_w, root);
							_w = _x_parent->left;
						}
						_w->color = _x_parent->color;
						_x_parent->color = __rb_tree_black;
						if (_w->left) _w->left->color = __rb_tree_black;
						__rb_tree_rotate_right(_x_parent, root);
						break;
					}
				}
			}
			if (_x) _x->color = __rb_tree_black;
		}
		return _y;
	}

	template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
	rb_tree<Key, Value, KeyOfValue, Compare, Alloc>&
		rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::operator=(const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& x) {
		if (this != &x) {
			clear();
			node_count = 0;
			key_compare = x.key_compare;
			if (x.root() == nullptr) {
				root() = nullptr;
				leftmost() = header;
				rightmost() = header;
			}
			else {
				root() = __copy(x.root(), header);
				leftmost() = minimum(root());
				rightmost() = maximum(root());
				node_count = x.node_count;
			}
		}
		return *this;
	}

__MYSTL_NAMESPACE_END_

#endif // !_ZSTL_RBTREE_H_
