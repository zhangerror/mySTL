#ifndef _ZSTL_LIST_H_
#define _ZSTL_LIST_H_

#include "00zstl_config.h"
#include "00zstl_algobase.h"
#include "00zstl_alloc.h"
#include "00zstl_construct.h"

__MYSTL_NAMESPACE_BEGIN_
	//�ڵ�ṹ
	template <class T>
	struct __list_node {
		typedef void* void_pointer;
		void_pointer prev;		//�ͱ�Ϊvoid*������Ϊ__list_node<T>*
		void_pointer next;
		T data;
	};

	//������
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

		link_type node;	//�������ڲ���ȻҪ��һ����ָͨ�룬ָ��list�Ľڵ�

		//constructor
		__list_iterator(link_type x) : node(x) { }
		__list_iterator() { }
		__list_iterator(const iterator& x) : node(x.node) { }

		bool operator == (const self& x) const { return node == x.node;  }
		bool operator != (const self& x) const { return node != x.node;  }
		//���¶Ե�����ȡֵ(dereference)��ȡ���ǽڵ������ֵ
		reference operator*() const { return (*node).data;  }

		//�����ǵ������ĳ�Ա��ȡ(member access)�����ӵı�׼����
		pointer operator->() const { return &(operator*());  }

		//�Ե������ۼ�1������ǰ��һ���ڵ�
		self& operator++() {
			node = (link_type)((*node).next);
			return *this;
		}
		self operator++(int) {
			self tmp = *this;
			++*this;
			return tmp;
		}

		//�Ե������ݼ�1�����Ǻ���һ���ڵ�
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

	template <class T, class Alloc = alloc>		//ȱʡʹ��allocΪ������
	class list {
	protected:
		typedef __list_node<T> list_node;

		//ר��֮�ռ���������ÿ������һ���ڵ��С
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
		link_type node;		//ֻҪһ��ָ�룬��ɱ�ʾ������״˫������
		
		//����һ���ڵ㲢����
		link_type get_node() { return list_node_allocator::allocate(); }
		//�ͷ�һ���ڵ�
		void put_node(link_type p) { list_node_allocator::deallocate(p); }

		//����(���ò�����)һ���ڵ㣬����Ԫ��ֵ
		link_type create_node(const T& x) {
			link_type p = get_node();
			construct(&p->data, x);	//global
			return p;
		}
		//����(�������ͷ�)һ���ڵ�
		void destroy_node(link_type p) {
			destroy(&p->data);		//global
			put_node(p);
		}

		void empty_initialize() {
			node = get_node();		//����һ���ڵ�ռ䣬��nodeָ����
			node->next = node;		//��nodeͷβ��ָ���Լ�������Ԫ��ֵ
			node->prev = node;
		}

		//��[first, last)�ڵ�����Ԫ���ƶ���position֮ǰ
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
		list() { empty_initialize();  }		//����һ��������
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
		//ȡͷ�ڵ������(Ԫ��ֵ)
		reference front() { return *begin(); }
		//ȡβ�ڵ������(Ԫ��ֵ)
		reference back() { return *(--end()); }

		//�ڵ�����position��ָ��λ�ò���һ���ڵ㣬����Ϊx
		iterator insert(iterator position, const T& x) {
			link_type tmp = create_node(x);		//����һ���ڵ�(��������Ϊx)
			//����˫��ָ�룬ʹtmp�����ȥ
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
		//�����������
		void clear();
		//����ֵΪvalue������Ԫ���Ƴ�
		void remove(const T& value);
		//�Ƴ���ֵ��ͬ������Ԫ��(��������ͬ���Ƴ���ֻʣһ��)
		void unique();

		//��x�Ӻ���position��ָλ��֮ǰ��x���벻ͬ��*this
		void splice(iterator position, list& x) {
			if (!x.empty()) {
				transfer(position, x.begin(), x.end());
			}
		}

		//��i��ָԪ�ؽӺ���position��ָλ��֮ǰ��position��i��ָ��ͬһ��list
		void splice(iterator position, list&, iterator i) {
			iterator j = i;
			++j;
			if (position == i || position == j) return;
			transfer(position, i, j);
		}

		//��[first, last)������Ԫ�ؽӺ���position��ָλ��֮ǰ
		//position��[first, last)��ָ��ͬһ��list
		//��position����λ��[first, last)֮��
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
			destroy_node(tmp);	//����(�������ͷ�)һ���ڵ�
		}
		//�ָ�nodeԭʼ״̬
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
			if (*first == value) erase(first);		//�ҵ����Ƴ�
			first = next;
		}
	}

	template <class T, class Alloc>
	void list<T, Alloc>::unique() {
		iterator first = begin();
		iterator last = end();
		if (first == last) return;		//������ʲô��������
		iterator next = first;
		while (++next != last) {
			//����ڴ�����������ͬ��Ԫ�أ��Ƴ�
			if (*first == *next) {
				erase(next);
			}
			else {
				first = next;	//����ָ��
			}
			next = first;	//�������η�Χ
		}
	}

	//��x�ϲ���*this���ϣ�����lists�����ݶ������Ⱦ�����������
	template <class T, class Alloc>
	void list<T, Alloc>::merge(list<T, Alloc>& x) {
		iterator first1 = begin();
		iterator last1 = end();
		iterator first2 = x.begin();
		iterator last2 = x.end();

		//ע�⣺ǰ���ǣ�����lists���Ѿ�����������
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

	//��*this��������������
	template <class T, class Alloc> 
	void list<T, Alloc>::reverse() {
		//�����жϣ�����ǿ����������һ��Ԫ�أ��Ͳ������κβ���
		//ʹ��size() == 0 || size() == 1���жϻ�Ƚ���
		if (node->next == node || link_type(node->next)->next == node) return;
		iterator first = begin();
		++first;
		while (first != end()) {
			iterator old = first;
			++first;
			transfer(begin(), old, first);
		}
	}

	//list����ʹ��STL�㷨sort()������ʹ���Լ���sort() member function
	//��ΪSTL�㷨sort()ֻ����RamdonAccessIterator
	//����������quick sort
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
		return low;		//������������λ��
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
				quickSort(low, --pivot2);		//��ǰ�벿�ֽ�������
				quickSort(++pivot, high);		//�Ժ�벿�ֽ�������
			}
		}
	}
	//����ӿ�
	template<class T, class Alloc>
	void list<T, Alloc>::sort() {
		//�����ж�����Ϊ�ջ�ֻ��һ��Ԫ���򲻽����κβ�����ʹ��size()�ж��ٶȽ���
		if (node->next == node || link_type(node->next)->next == node) return;
		quickSort(begin(), --end());
	}

__MYSTL_NAMESPACE_END_

#endif //!_ZSTL_LIST_H_