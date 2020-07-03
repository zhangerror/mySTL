#ifndef _ZSTL_SLIST_H_
#define _ZSTL_SLIST_H_

#include "00zstl_alloc.h"
#include "00zstl_construct.h"
#include "00zstl_algobase.h"

__MYSTL_NAMESPACE_BEGIN_
	//��������Ľڵ�����ṹ
	struct __slist_node_base {
		__slist_node_base * next;
	};

	//��������Ľڵ�ṹ
	template <class T>
	struct __slist_node : public __slist_node_base {
		T data;
	};

	//ȫ�ֺ�������֪ĳһ�ڵ㣬�����½ڵ������
	inline __slist_node_base * __slist_make_link(
		__slist_node_base * prev_node,
		__slist_node_base * new_node) {
		new_node->next = prev_node->next;	//�� new �ڵ����һ�ڵ�Ϊ prev �ڵ����һ�ڵ�
		prev_node->next = new_node;			//�� prev �ڵ����һ�ڵ�ָ�� new �ڵ�

		return new_node;
	}

	//ȫ�ֺ�������������Ĵ�С(Ԫ�ظ���)
	inline size_t __slist_size(__slist_node_base * node) {
		size_t result = 0;
		for (; node != 0; node = node->next) {
			++result;
		}

		return result;
	}

	//��������ĵ����������ṹ
	struct __slist_iterator_base {
		typedef size_t					size_type;
		typedef ptrdiff_t				difference_type;
		typedef forward_iterator_tag	iterator_category;	//����

		__slist_node_base * node;	//ָ��ڵ�����ṹ

		__slist_iterator_base(__slist_node_base * x) : node(x) { }

		void incr() { node = node->next; }		//ǰ��һ���ڵ�

		bool operator == (const __slist_iterator_base & x) const {
			return node == x.node;
		}
		bool operator != (const __slist_iterator_base & x) const {
			return node != x.node;
		}
	};

	//��������ĵ������ṹ
	template <class T, class Ref, class Ptr>
	struct __slist_iterator : public __slist_iterator_base {
		typedef __slist_iterator<T, T&, T*>				iterator;
		typedef __slist_iterator<T, const T&, const T*>	const_iterator;
		typedef __slist_iterator<T, Ref, Ptr>			self;

		typedef T				value_type;
		typedef Ptr				pointer;
		typedef Ref				reference;
		typedef __slist_node<T>	list_node;

		__slist_iterator(list_node * x) : __slist_iterator_base(x) { }
		//���� slist<T>::end() ʱ����� __slist_iterator(0)�����ǵ�����������
		__slist_iterator() : __slist_iterator_base(0) { }
		__slist_iterator(const iterator & x) : __slist_iterator_base(x.node) { }

		reference operator*() const { return ((list_node*)node)->data; }
		pointer operator->() const { return &(operator*()); }

		self& operator++() {
			incr();		//ǰ��һ���ڵ�
			return *this;
		}
		self operator++(int) {
			self tmp = *this;
			incr();		//ǰ��һ���ڵ�
			return tmp;
		}
		// û��ʵ�� operator--����Ϊ����һ�� forward iterator
	};

	//slist �����ݽṹ
	template <class T, class Alloc = alloc> 
	class slist {
	public:
		typedef T					value_type;
		typedef value_type*			pointer;
		typedef const value_type*	const_pointer;
		typedef value_type&			reference;
		typedef const value_type&	const_reference;
		typedef size_t				size_type;
		typedef ptrdiff_t			difference_type;

		typedef __slist_iterator<T, T&, T*>				iterator;
		typedef __slist_iterator<T, const T&, const T*>	const_iterator;

	private:
		typedef __slist_node<T>					list_node;
		typedef __slist_node_base				list_node_base;
		typedef __slist_iterator_base			iterator_base;
		typedef simple_alloc<list_node, Alloc>	list_node_allocator;

		static list_node* create_node(const value_type& x) {
			list_node* node = list_node_allocator::allocate();	//���ÿռ�
			try {
				construct(&node->data, x);		//����Ԫ��
				node->next = 0;
			}
			catch (...) {
				list_node_allocator::deallocate(node);
				throw;
			}
			return node;
		}

		static void destroy_node(list_node* node) {
			destroy(&node->data);		//��Ԫ������
			list_node_allocator::deallocate(node);		//�ͷſռ�
		}

		void clear() {
			list_node* node = (list_node*)head.next;
			while (node != 0) {
				list_node* tmp = node;
				node = (list_node*)node->next;
				destroy_node(tmp);
			}
		}

	private:
		list_node_base	head;		//ͷ��������ָ�룬��ʵ��

	public:
		slist() { head.next = 0; }
		~slist() { clear(); }

		iterator begin() { return iterator((list_node*)head.next); }
		iterator end() { return iterator(0); }
		size_type size() const { return __slist_size(head.next); }
		bool empty() const { return head.next == 0; }

		//���� slist ������ֻҪ�� head ������ָ����
		void swap(slist& L) {
			list_node_base* tmp = head.next;
			head.next = L.head.next;
			L.head.next = tmp;
		}

		//ȡͷ��Ԫ��
		reference front() { return ((list_node*)head.next)->data; }

		//��ͷ������Ԫ��(��Ԫ�س�Ϊ slist �ĵ�һ��Ԫ��)
		void push_front(const value_type& x) {
			__slist_make_link(&head, create_node(x));
		}

		//����Ԫ��(�ڵ�����֮��)
		void insert(iterator ite, const value_type& x) {
			__slist_make_link(ite.node, create_node(x));
		}

		//��ͷ��ȡ��Ԫ��(��ɾ��)���޸� head
		void pop_front() {
			list_node* node = (list_node*)head.next;
			head.next = node->next;
			destroy_node(node);
		}

		//��ȡָ���ڵ��ǰһ���ڵ�
		list_node_base* get_previous(list_node_base* _head, const list_node_base* _node) {
			while (_head && _head->next != _node) {
				_head = _head->next;
			}
			return _head;
		}

		//ʵ��ɾ���ڵ����
		list_node_base* erase_base(list_node_base* pos) {
			list_node* next = (list_node*)(pos->next);
			list_node_base* next_next = next->next;
			pos->next = next_next;
			destroy_node(next);
			return next_next;
		}

		//ɾ����������ָλ��Ԫ�أ����� slist ��һ��������������ָ�������Ҫ��ͷ�ҵ���ɾ���ڵ��ǰһ���ڵ�
		iterator erase(iterator ite) {
			return iterator((list_node*)this->erase_base(get_previous(&this->head, ite.node)));
		}
	};

__MYSTL_NAMESPACE_END_

#endif // !_ZSTL_SLIST_H_
