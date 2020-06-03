#ifndef _ZSTL_LIST_H_
#define _ZSTL_LIST_H_

#include "00zstl_config.h"
#include "00zstl_algobase.h"
#include "00zstl_alloc.h"
#include "00zstl_construct.h"

__MYSTL_NAMESPACE_BEGIN_
	//节点结构
	template <class T>
	struct __list_node {
		typedef void* void_pointer;
		void_pointer prev;		//型别为void*，可设为__list_node<T>*
		void_pointer next;
		T data;
	};

	//迭代器
	template <class T, class Ref, class Ptr>
	struct __list_iterator {
		typedef __list_iterator<T, T&, T*>		iterator;
		typedef __list_iterator<T, Ref, Ptr>	self;

		typedef bidirectional_iterator_tag		iterator_category;
		typedef T								value_type;
		typedef Ptr								pointer;
		typedef Ref								reference;
		typedef __list_node<T>*					link_type;
		typedef size_t							size_type;
		typedef ptrdiff_t						difference_type;

		link_type node;	//迭代器内部当然要有一个普通指针，指向list的节点

		//constructor
		__list_iterator(link_type x) : node(x) { }
		__list_iterator() { }
		__list_iterator(const iterator& x) : node(x.node) { }

		bool operator == (const self& x) const { return node == x.node;  }
		bool operator != (const self& x) const { return node != x.node;  }
		//以下对迭代器取值(dereference)，取的是节点的数据值
		reference operator*() const { return (*node).data;  }

		//以下是迭代器的成员存取(member access)运算子的标准做法
		pointer operator->() const { return &(operator*());  }

		//对迭代器累加1，就是前进一个节点
		self& operator++() {
			node = (link_type)((*node).next);
			return *this;
		}
		self operator++(int) {
			self tmp = *this;
			++*this;
			return tmp;
		}

		//对迭代器递减1，就是后退一个节点
		self& operator--() {
			node = (link_type)((*node).prev);
			return *this;
		}
		self operator--(int) {
			self tmp = *this;
			--*this;
			return tmp;
		}
	};

	template <class T, class Alloc = alloc>		//缺省使用alloc为配置器
	class list {
	protected:
		typedef __list_node<T> list_node;

		//专属之空间配置器，每次配置一个节点大小
		typedef simple_alloc<list_node, Alloc> list_node_allocator;
	public:
		typedef list_node* link_type;
		typedef typename __list_iterator<T, T&, T*>::iterator			iterator;
		typedef typename __list_iterator<T, T&, T*>::iterator_category	category;
		typedef typename __list_iterator<T, T&, T*>::value_type			value_type;
		typedef typename __list_iterator<T, T&, T*>::pointer			pointer;
		typedef typename __list_iterator<T, T&, T*>::reference			reference;
		typedef typename __list_iterator<T, T&, T*>::size_type			size_type;
		typedef typename __list_iterator<T, T&, T*>::difference_type	difference_type;

	protected:
		link_type node;		//只要一个指针，便可表示整个环状双向链表
		
		//配置一个节点并传回
		link_type get_node() { return list_node_allocator::allocate(); }
		//释放一个节点
		void put_node(link_type p) { list_node_allocator::deallocate(p); }

		//产生(配置并构造)一个节点，带有元素值
		link_type create_node(const T& x) {
			link_type p = get_node();
			construct(&p->data, x);	//global
			return p;
		}
		//销毁(析构并释放)一个节点
		void destroy_node(link_type p) {
			destroy(&p->data);		//global
			put_node(p);
		}

		void empty_initialize() {
			node = get_node();		//配置一个节点空间，令node指向它
			node->next = node;		//令node头尾都指向自己，不设元素值
			node->prev = node;
		}

		//将[first, last)内的所有元素移动到position之前
		void transfer(iterator position, iterator first, iterator last) {
			if (position != last) {
				(*(link_type((*last.node).prev))).next = position.node;
				(*(link_type((*first.node).prev))).next = last.node;
				(*(link_type((*position.node).prev))).next = first.node;
				link_type tmp = link_type((*position.node).prev);
				(*position.node).prev = (*last.node).prev;
				(*last.node).prev = (*first.node).prev;
				(*first.node).prev = tmp;
			}
		}
	public:
		list() { empty_initialize();  }		//产生一个空链表
		~list() {
			clear();
			destroy_node(node);
		}

		iterator begin() { return (link_type)((*node).next); }
		iterator end() { return node; }
		bool empty() const { return node->next == node; }
		size_type size() {
			return distance(begin(), end(), category());	//global
		}
		//取头节点的内容(元素值)
		reference front() { return *begin(); }
		//取尾节点的内容(元素值)
		reference back() { return *(--end()); }

		//在迭代器position所指的位置插入一个节点，内容为x
		iterator insert(iterator position, const T& x) {
			link_type tmp = create_node(x);		//产生一个节点(设置内容为x)
			//调整双向指针，使tmp插入进去
			tmp->next = position.node;
			tmp->prev = position.node->prev;
			(link_type(position.node->prev))->next = tmp;
			position.node->prev = tmp;

			return tmp;
		}
		void push_front(const T& x) { insert(begin(), x); }
		void push_back(const T& x) { insert(end(), x); }

		iterator erase(iterator position) {
			link_type next_node = link_type(position.node->next);
			link_type prev_node = link_type(position.node->prev);
			prev_node->next = next_node;
			next_node->prev = prev_node;
			destroy_node(position.node);
			return iterator(next_node);
		}
		void pop_front() { erase(begin()); }
		void pop_back() {
			iterator tmp = end();
			erase(--tmp);
		}
		//清除整个链表
		void clear();
		//将数值为value的所有元素移除
		void remove(const T& value);
		//移除数值相同的连续元素(连续、相同，移除后只剩一个)
		void unique();

		//将x接合于position所指位置之前。x必须不同于*this
		void splice(iterator position, list& x) {
			if (!x.empty()) {
				transfer(position, x.begin(), x.end());
			}
		}

		//将i所指元素接合于position所指位置之前。position和i可指向同一个list
		void splice(iterator position, list&, iterator i) {
			iterator j = i;
			++j;
			if (position == i || position == j) return;
			transfer(position, i, j);
		}

		//将[first, last)内所有元素接合于position所指位置之前
		//position和[first, last)可指向同一个list
		//但position不能位于[first, last)之内
		void splice(iterator position, list&, iterator first, iterator last) {
			if (first != last) {
				transfer(position, first, last);
			}
		}

		void merge(list<T, Alloc>& x);
		void reverse();
		iterator qSort(iterator low, iterator high);
		void quickSort(iterator low, iterator high);
		void sort();
	};

	template <class T, class Alloc>
	void list<T, Alloc>::clear() {
		link_type cur = (link_type)node->next;		//begin()
		while (cur != node) {
			link_type tmp = cur;
			cur = (link_type)cur->next;
			destroy_node(tmp);	//销毁(析构并释放)一个节点
		}
		//恢复node原始状态
		node->next = node;
		node->prev = node;
	}

	template <class T, class Alloc> 
	void list<T, Alloc>::remove(const T& value) {
		iterator first = begin();
		iterator last = end();
		while (first != last) {
			iterator next = first;
			++next;
			if (*first == value) erase(first);		//找到就移除
			first = next;
		}
	}

	template <class T, class Alloc>
	void list<T, Alloc>::unique() {
		iterator first = begin();
		iterator last = end();
		if (first == last) return;		//空链表，什么都不必做
		iterator next = first;
		while (++next != last) {
			//如果在此区段中有相同的元素，移除
			if (*first == *next) {
				erase(next);
			}
			else {
				first = next;	//调整指针
			}
			next = first;	//修正区段范围
		}
	}

	//将x合并到*this身上，两个lists的内容都必须先经过递增排序
	template <class T, class Alloc>
	void list<T, Alloc>::merge(list<T, Alloc>& x) {
		iterator first1 = begin();
		iterator last1 = end();
		iterator first2 = x.begin();
		iterator last2 = x.end();

		//注意：前提是，两个lists都已经过递增排序
		while (first1 != last1 && first2 != last2) {
			if (*first2 < *first1) {
				iterator next = first2;
				transfer(first1, first2, ++next);
				first2 = next;
			}
			else {
				++first1;
			}
		}
		if (first2 != last2) transfer(last1, first2, last2);
	}

	//将*this的内容逆向重置
	template <class T, class Alloc> 
	void list<T, Alloc>::reverse() {
		//以下判断，如果是空链表，或仅有一个元素，就不进行任何操作
		//使用size() == 0 || size() == 1来判断会比较慢
		if (node->next == node || link_type(node->next)->next == node) return;
		iterator first = begin();
		++first;
		while (first != end()) {
			iterator old = first;
			++first;
			transfer(begin(), old, first);
		}
	}

	//list不能使用STL算法sort()，必须使用自己的sort() member function
	//因为STL算法sort()只接受RamdonAccessIterator
	//本函数采用quick sort
	template<class T, class Alloc>
	typename list<T, Alloc>::iterator
		list<T, Alloc>::qSort(iterator low, iterator high) {
		value_type pivotkey = (low.node)->data;
		while (low != high) {
			while (low != high && (high.node)->data >= pivotkey)
				--high;
			(low.node)->data = (high.node)->data;
			while (low != high && (low.node)->data <= pivotkey)
				++low;
			(high.node)->data = (low.node)->data;
		}
		(low.node)->data = pivotkey;
		return low;		//返回枢轴所在位置
	}
	template<class T, class Alloc>
	void list<T, Alloc>::quickSort(iterator low, iterator high) {
		if (low != high) {
			iterator pivot = qSort(low, high);
			if (pivot == low) {
				quickSort(++pivot, high);
			}
			else if (pivot == high) {
				quickSort(low, --high);
			}
			else {
				iterator pivot2 = pivot;
				quickSort(low, --pivot2);		//对前半部分进行排序
				quickSort(++pivot, high);		//对后半部分进行排序
			}
		}
	}
	//对外接口
	template<class T, class Alloc>
	void list<T, Alloc>::sort() {
		//首先判断链表为空或只有一个元素则不进行任何操作，使用size()判断速度较慢
		if (node->next == node || link_type(node->next)->next == node) return;
		quickSort(begin(), --end());
	}

__MYSTL_NAMESPACE_END_

#endif //!_ZSTL_LIST_H_