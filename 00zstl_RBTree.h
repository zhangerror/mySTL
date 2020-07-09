#ifndef _ZSTL_RBTREE_H_
#define _ZSTL_RBTREE_H_

#include <iostream>
#include "00zstl_alloc.h"
#include "00zstl_construct.h"
#include "00zstl_algobase.h"
#include <utility>		//pair

__MYSTL_NAMESPACE_BEGIN_
	typedef bool __rb_tree_color_type;
	const __rb_tree_color_type __rb_tree_red = false;	//��ɫΪ0
	const __rb_tree_color_type __rb_tree_black = true;	//��ɫΪ1

	//RB-tree �Ļ����ڵ�ṹ
	struct __rb_tree_node_base {
		typedef __rb_tree_color_type color_type;
		typedef __rb_tree_node_base* base_ptr;

		color_type	color;	//�ڵ���ɫ���Ǻ켴��
		base_ptr	parent;	//RB ����������������֪�����ڵ�
		base_ptr	left;	//ָ����ڵ�
		base_ptr	right;	//ָ���ҽڵ�

		static base_ptr minimum(base_ptr x) {
			if (!x) return nullptr;

			while (x->left) x = x->left;	//һֱ�����ߣ��ͻ��ҵ���Сֵ
			return x;
		}

		static base_ptr maximum(base_ptr x) {
			if (!x) return nullptr;

			while (x->right) x = x->right;	//һֱ�����ߣ��ͻ��ҵ����ֵ
			return x;
		}
	};

	//RB-tree �Ľڵ�ṹ
	template <class Value>
	struct __rb_tree_node : public __rb_tree_node_base {
		typedef __rb_tree_node<Value>* link_type;
		Value value_field;	//�ڵ�ֵ
	};

	//RB-tree �Ļ��������
	struct __rb_tree_base_iterator {
		typedef __rb_tree_node_base::base_ptr	base_ptr;
		typedef bidirectional_iterator_tag		iterator_category;
		typedef ptrdiff_t						difference_type;

		base_ptr node;	//����������֮�����һ�������ϵ(make a reference)

		void increment() {
			if (node->right) {				//��������ӽڵ� (1)
				node = node->right;			//������
				while (node->left) {		//Ȼ��һֱ���������ߵ��ף����ǽ��
					node = node->left;
				}
			}
			else {							//û�����ӽڵ� (2)
				base_ptr _parent = node->parent;	//�ҳ����ڵ�
				while (node == _parent->right) {	//������нڵ㱾���Ǹ����ӽڵ�
					node = _parent;				//��һֱ���ݣ�ֱ������Ϊ���ӽڵ㡱Ϊֹ
					_parent = node->parent;
				}
				if (node->right != _parent) {		//����ʱ�����ӽڵ㲻���ڴ�ʱ�ĸ��ڵ�
					node = _parent;				//��ʱ�ĸ��ڵ㼴Ϊ��� (3)�������ʱ�� node Ϊ��� (4)
				}
			}
			/*������ʱ�����ӽڵ㲻���ڴ�ʱ�ĸ��ڵ㡱�ж���Ϊ��Ӧ��һ�����������
				��Ѱ�Ҹ��ڵ����һ�ڵ㣬��ǡ�ɸ��ڵ������ӽڵ�
				��������������� RB-tree ���ڵ�������� header ֮��������ϵ
			*/
		}

		void decrement() {
			if (node->color == __rb_tree_red &&		//����Ǻ�ڵ㣬��
				node->parent->parent == node) {		//���ڵ�ĸ��ڵ�����Լ�
				node = node->right;					//(1) ���ӽڵ㼴Ϊ���
			}
			//������������� node Ϊ header ʱ(�༴ node Ϊ end() ʱ)
			//header �����ӽڵ㼴Ϊ mostright��ָ���������� max �ڵ�
			else if (node->left) {					//��������ӽڵ� (2)
				node = node->left;					//�� node ָ�����ӽڵ�
				while (node->right) {					//�� node �����ӽڵ�ʱ
					node = node->right;					//һֱ�����ӽڵ��ߵ���
				}
			}
			else {									//�ȷǸ��ڵ㣬�������ӽڵ�
				base_ptr _parent = node->parent;			//(3) �ҳ����ڵ�
				while (node == _parent->left) {			//�����нڵ���Ϊ���ӽڵ�
					node = _parent;						//һֱ���������ߣ�ֱ�����нڵ�
					_parent = _parent->parent;					//��Ϊ���ӽڵ�
				}
				node = _parent;							//��ʱ�ĸ��ڵ㼴Ϊ��
			}
		}
	};

	//RB-tree �����������
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

	//RB-tree �����ݽṹ
	// Key:			��ֵ�ͱ� 
	// Value:		Ԫ���ͱ�
	// KeyOfValue:	��ֵ��ȡ(����ʵֵ)
	// Compare:		��ֵ�ȽϹ���
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
			link_type tmp = get_node();		//���ÿռ�
			try {
				construct(&tmp->value_field, x);	//��������
			}
			catch (...) {
				put_node(tmp);
			}
			return tmp;
		}

		link_type clone_node(link_type x) {			//����һ���ڵ�(��ֵ����ɫ)
			link_type tmp = create_node(x->value_field);
			tmp->color = x->color;
			tmp->left = nullptr;
			tmp->right = nullptr;
			return tmp;
		}

		void destroy_node(link_type x) {
			destroy(&x->value_field);		//��������
			put_node(x);					//�ͷ�����
		}

	protected:
		//RB-tree ֻ���������ݱ���
		size_type	node_count;		//׷�ټ�¼���Ĵ�С(�ڵ�����)
		link_type	header;			//ͷָ�룬�� parent �� root ��ָ���� left ָ����С�ڵ㣬right ָ�����ڵ�
		Compare		key_compare;	//�ڵ��ļ�ֵ��С�Ƚ�׼��

		//��������������������ȡ�� header �ĳ�Ա
		link_type& root() const { return (link_type&)header->parent; }
		link_type& leftmost() const { return (link_type&)header->left; }
		link_type& rightmost() const { return (link_type&)header->right; }

		//��������������������ȡ�ýڵ� x �ĳ�Ա
		static link_type& left(link_type x) { return (link_type&)(x->left); }
		static link_type& right(link_type x) { return (link_type&)(x->right); }
		static link_type& parent(link_type x) { return (link_type&)(x->parent);	}
		static reference value(link_type x) { return x->value_field; }
		static const Key& key(link_type x) { return KeyOfValue()(value(x));	}
		static color_type& color(link_type x) { return (color_type&)(x->color);	}

		//��������������������ȡ�ýڵ� x �ĳ�Ա
		static link_type& left(base_ptr x) { return (link_type&)(x->left); }
		static link_type& right(base_ptr x) { return (link_type&)(x->right); }
		static link_type& parent(base_ptr x) { return (link_type&)(x->parent); }
		static reference value(base_ptr x) { return ((link_type)x)->value_field; }
		static const Key& key(base_ptr x) { return KeyOfValue()(value(x)); }
		static color_type& color(base_ptr x) { return (color_type&)(x->color); }

		//��ȡ����ֵ�ͼ�Сֵ��node class ��ʵ�ִ˹��ܣ�����������ɼ���
		static link_type minimum(link_type x) {
			return (link_type)__rb_tree_node_base::minimum(x);
		}
		static link_type maximum(link_type x) {
			return (link_type)__rb_tree_node_base::maximum(x);
		}		

	public:
		// allocation / deallocation
		rb_tree(const Compare& comp = Compare()) : node_count(0), key_compare(comp) { init(); }
		rb_tree(const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& x) {		//�������캯��
			init();
			*this = x;		//���� operator=
		}		
		~rb_tree() {
			clear();
			put_node(header);
		}
		rb_tree<Key, Value, KeyOfValue, Compare, Alloc>&
			operator = (const rb_tree<Key, Value, KeyOfValue, Compare, Alloc>& x);

		void init() {
			header = get_node();			//����һ���ڵ�ռ䣬�� header ָ����
			color(header) = __rb_tree_red;	//�� header Ϊ��ɫ���������� header �� root���� iterator.operator++ ֮��
			root() = nullptr;
			leftmost() = header;			//�� header �����ӽڵ�Ϊ�Լ�
			rightmost() = header;			//�� header �����ӽڵ�Ϊ�Լ�
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
		iterator begin() { return leftmost(); }		//RB ������ͷΪ����(��С)�ڵ㴦
		const_iterator begin() const { return leftmost(); }
		iterator end() { return header; }			//RB �����յ�Ϊ header ��ָ��
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

	//������ֵ���ڵ��ֵ�����ظ�
	//ע�⣬����ֵ��һ�� RB-tree ��������ָ�������ڵ�
	template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
	typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
		rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::insert_equal(const value_type& v) {
		link_type y = header;
		link_type x = root();		//�Ӹ��ڵ㿪ʼ
		while (x) {			//����Ѱ���ʵ��Ĳ����
			y = x;
			x = key_compare(KeyOfValue()(v), key(x)) ? left(x) : right(x);
			//���ϣ�����������������С�ڻ���ڡ�������
		}
		return __insert(x, y, v);
		//���ϣ�x Ϊ��ֵ����㣬y Ϊ�����ĸ��ڵ㣬v Ϊ��ֵ
	}

	//������ֵ���ڵ��ֵ�������ظ������ظ��������Ч
	//ע�⣬����ֵ�Ǹ� pair����һԪ���Ǹ� RB-tree ��������ָ�������ڵ㣬�ڶ�Ԫ�ر�ʾ����ɹ����
	template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
	std::pair<typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator,bool>
		rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::insert_unique(const value_type& v) {
		link_type y = header;
		link_type x = root();	//�Ӹ��ڵ㿪ʼ
		bool comp = true;
		while (x) {		//����Ѱ���ʵ��Ĳ����
			y = x;
			comp = key_compare(KeyOfValue()(v), key(x));		//v ��ֵС��Ŀǰ�ڵ�ļ�ֵ��
			x = comp ? left(x) : right(x);		//����������������С����ڡ�������
		}
		//�뿪 while ѭ��֮��y ��ָ�������ĸ��ڵ�(��ʱ������ΪҶ�ڵ�)

		iterator j = iterator(y);		//������� j ָ������ĸ��ڵ� y
		if (comp) {						//����뿪 while ѭ��ʱ comp Ϊ��(��ʾ�����󡱣������������)
			if (j == begin()) {			//��������ĸ��ڵ�Ϊ����ڵ�
				return std::pair<iterator, bool>(__insert(x, y, v), true);
				//���ϣ�x Ϊ����㣬y Ϊ�����ĸ��ڵ㣬v Ϊ��ֵ
			}
			else {						//����(�����ĸ��ڵ㲻Ϊ����ڵ�
				--j;					//���� j����ͷ׼������
			}
		}
		if (key_compare(key(j.node), KeyOfValue()(v))) {
			//С����ֵ(��ʾ����С�������������Ҳ�
			return std::pair<iterator, bool>(__insert(x, y, v), true);
			//���ϣ�x Ϊ��ֵ����㣬 y Ϊ�����ĸ��ڵ㣬v Ϊ��ֵ
		}

		//�������ˣ���ʾ��ֵһ�������м�ֵ�ظ����Ͳ�Ӧ�ò�����ֵ
		return std::pair<iterator, bool>(j, false);
	}

	//�����Ĳ���ִ�г��� __insert()
	template<class Key, class Value, class KeyOfValue, class Compare, class Alloc>
	typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator 
		rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::__insert(base_ptr _x, base_ptr _y, const Value& v) {
		//���� x_ Ϊ��ֵ����㣬���� y_ Ϊ�����ĸ��ڵ㣬���� v Ϊ��ֵ
		link_type x = (link_type)_x;
		link_type y = (link_type)_y;
		link_type z;

		//key_compare �Ǽ�ֵ��С�Ƚ�׼��
		if (y == header || x || key_compare(KeyOfValue()(v), key(y))) {
			z = create_node(v);			//����һ���½ڵ�
			left(y) = z;				//ʹ�õ� y ��Ϊ header ʱ��leftmost() = z
			if (y == header) {
				root() = z;
				rightmost() = z;
			}
			else if (y == leftmost()) {	//��� y Ϊ����ڵ�
				leftmost() = z;			//ά�� leftmost()��ʹ����Զָ������ڵ�
			}
		}
		else {
			z = create_node(v);			//����һ���½ڵ�
			right(y) = z;				//���½ڵ��Ϊ�����ĸ��ڵ� y �����ӽڵ�
			if (y == rightmost()) {
				rightmost() = z;		//ά�� rightmost()��ʹ����Զָ�����ҽڵ�
			}
		}
		parent(z) = y;		//�趨�½ڵ�ĸ��ڵ�
		left(z) = nullptr;		//�趨�½ڵ�����ӽڵ�
		right(z) = nullptr;		//�趨�½ڵ�����ӽڵ�
							//�½ڵ����ɫ�� rebalence �趨������
		__rb_tree_rebalance(z, header->parent);		//����һΪ�����ڵ㣬������Ϊ root
		++node_count;			//�ڵ�������
		return iterator(z);		//����һ����������ָ�������ڵ�
	}

	//ȫ�ֺ���������ת
	//�½ڵ��Ϊ��ڵ㣬������봦�ĸ��ڵ�Ҳ�Ǻ�ڵ�
	//��Υ����������򣬱�����������ת
	inline void __rb_tree_rotate_left(__rb_tree_node_base* x, __rb_tree_node_base*& root) {
		// x Ϊ��ת��
		__rb_tree_node_base* y = x->right;	//�� y Ϊ��ת������ӽڵ�
		x->right = y->left;
		if (y->left) {
			y->left->parent = x;			//����ǹ�趨���ڵ�
		}
		y->parent = x->parent;

		//�� y ��ȫ���� x �ĵ�λ(���뽫 x ���丸�ڵ�Ĺ�ϵ��ȫ���չ���)
		if (x == root) {					// x Ϊ���ڵ�
			root = y;
		}
		else if (x == x->parent->left) {	// x Ϊ�丸�ڵ�����ӽڵ�
			x->parent->left = y;
		}
		else {								// x Ϊ�丸�ڵ�����ӽڵ�
			x->parent->right = y;
		}
		y->left = x;
		x->parent = y;
	}

	//ȫ�ֺ���������ת
	//�½ڵ��Ϊ��ڵ㣬������봦�ĸ��ڵ�Ҳ�Ǻ�ڵ�
	//��Υ����������򣬱�����������ת
	inline void __rb_tree_rotate_right(__rb_tree_node_base* x, __rb_tree_node_base*& root) {
		// x Ϊ��ת��
		__rb_tree_node_base* y = x->left;	//�� y Ϊ��ת������ӽڵ�
		x->left = y->right;
		if (y->right) {
			y->right->parent = x;			//����ǹ�趨���ڵ�
		}
		y->parent = x->parent;

		//�� y ��ȫ���� x �ĵ�λ(���뽫 x ���丸�ڵ�Ĺ�ϵ��ȫ���չ���)
		if (x == root) {					// x Ϊ���ڵ�
			root = y;
		}
		else if (x == x->parent->right) {	// x Ϊ�丸�ڵ�����ӽڵ�
			x->parent->right = y;
		}
		else {								// x Ϊ�丸�ڵ�����ӽڵ�
			x->parent->left = y;
		}
		y->right = x;
		x->parent = y;
	}

	//ȫ�ֺ�����ƽ����
	//��������ƽ��(�ı���ɫ����ת����)
	//����һΪ�����ڵ㣬������Ϊ root
	inline void __rb_tree_rebalance(__rb_tree_node_base* x, __rb_tree_node_base*& root) {
		x->color = __rb_tree_red;		//�½ڵ��Ϊ��
		while (x != root && x->parent->color == __rb_tree_red) {	//���ڵ�Ϊ��
			if (x->parent == x->parent->parent->left) {				//���ڵ�Ϊ�游�ڵ����ڵ�
				__rb_tree_node_base* y = x->parent->parent->right;	//�� y Ϊ�����ڵ�
				if (y && y->color == __rb_tree_red) {				//�����ڵ���ڣ���Ϊ��
					x->parent->color = __rb_tree_black;				//���ĸ��ڵ�Ϊ��
					y->color = __rb_tree_black;						//���Ĳ����ڵ�Ϊ��
					x->parent->parent->color = __rb_tree_red;		//�����游�ڵ�Ϊ��
					x = x->parent->parent;
				}
				else {												//�޲����ڵ㣬�򲮸��ڵ�Ϊ��
					if (x == x->parent->right) {					//����½ڵ�Ϊ���ڵ�����ӽڵ�
						x = x->parent;
						__rb_tree_rotate_left(x, root);				//��һ����Ϊ������
					}
					x->parent->color = __rb_tree_black;				//�ı���ɫ
					x->parent->parent->color = __rb_tree_red;
					__rb_tree_rotate_right(x->parent->parent, root);//��һ����Ϊ������
				}
			}
			else {													//���ڵ�Ϊ�游�ڵ�����ӽڵ�
				__rb_tree_node_base* y = x->parent->parent->left;	//�� y Ϊ�����ڵ�
				if (y && y->color == __rb_tree_red) {				//�����ڵ���ڣ���Ϊ��
					x->parent->color = __rb_tree_black;				//���ĸ��ڵ�Ϊ��
					y->color = __rb_tree_black;						//���Ĳ����ڵ�Ϊ��
					x->parent->parent->color = __rb_tree_red;		//�����游�ڵ�Ϊ��
					x = x->parent->parent;
				}
				else {												//�޲����ڵ㣬�򲮸��ڵ�Ϊ��
					if (x == x->parent->left) {					//����½ڵ�Ϊ���ڵ�����ӽڵ�
						x = x->parent;
						__rb_tree_rotate_right(x, root);			//��һ����Ϊ������
					}
					x->parent->color = __rb_tree_black;				//�ı���ɫ
					x->parent->parent->color = __rb_tree_red;
					__rb_tree_rotate_left(x->parent->parent, root);//��һ����Ϊ������
				}
			}
		}// while ����
		root->color = __rb_tree_black;		//���ڵ���ԶΪ��
	}

	template <class Key, class Value, class KeyOfValue, class Compare, class Alloc>
	typename rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::iterator
		rb_tree<Key, Value, KeyOfValue, Compare, Alloc>::find(const Key& x) {
		link_type _head = header;		//���һ������ x С�Ľڵ�
		link_type cur = root();			//��ǰ�ڵ�

		while (cur) {
			if (!key_compare(key(cur), x)) {
				//���е������ʾ cur ��ֵ���� x ��������ֵ��������
				_head = cur;
				cur = left(cur);
			}
			else {
				//���е������ʾ cur ��ֵС�� x ������Сֵ��������
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
