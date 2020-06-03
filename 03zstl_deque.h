#ifndef _ZSTL_DEQUE_H_
#define _ZSTL_DEQUE_H_

#include "00zstl_alloc.h"
#include "00zstl_construct.h"

//ȫ�ֺ�����
//���n��Ϊ0�� ����n����ʾbuffer size���û��Զ���
//���nΪ0�� ��ʾbuffer sizeʹ��Ĭ��ֵ�� ��ô
//		���sz(Ԫ�ش�С��sizeof(value_type)С��512������512/sz��
//		���sz��С��512������1
inline size_t __deque_buf_size(size_t n, size_t sz) {
	return n != 0 ? n : (sz < 512 ? size_t(512 / sz) : size_t(1));
}

__MYSTL_NAMESPACE_BEGIN_
	template <class T, class Ref, class Ptr, size_t BufSiz>
	struct __deque_iterator {		//δ�̳� std::iterator
		typedef __deque_iterator<T, T&, T*, BufSiz>				iterator;
		typedef __deque_iterator<T, const T&, const T*, BufSiz> const_iterator;

		static size_t buffer_size() { return __deque_buf_size(BufSiz, sizeof(T)); }
	
		//δ�̳� std::iterator�����Ա�������׫д�����Ҫ�ĵ�������Ӧ�ͱ�
		typedef random_access_iterator_tag		iterator_category;
		typedef T								value_type;
		typedef Ptr								pointer;
		typedef Ref								reference;
		typedef const Ref						const_reference;
		typedef ptrdiff_t						difference_type;

		typedef size_t							size_type;
		typedef T**								map_pointer;

		typedef __deque_iterator				self;

		//����������������
		T*			cur;		//�˵�������ָ������[first, last)�е�����Ԫ��
		T*			first;		//�˵�������ָ��������ͷ
		T*			last;		//�˵�������ָ��������β(�����ÿռ�)
		map_pointer	node;		//ָ��ܿ�����

		//�����µĻ�����
		void set_node(map_pointer new_node) {
			node = new_node;
			first = *new_node;
			last = first + difference_type(buffer_size());
		}

		//���¸��������������� __deque_iterator<> �ɹ������Ĺؼ�
		reference operator*() const { return *cur;  }
		pointer operator->() const { return &(operator*());  }
		difference_type operator-(const self& x) const {
			return difference_type(buffer_size()) * (node - x.node - 1) +
				(cur - first) + (x.last - x.cur);
		}
		self& operator++() {
			++cur;					//�л�����һ��Ԫ��
			if (cur == last) {		//����Ѵ����ڻ�������β��
				set_node(node + 1);	//���л�����һ�ڵ�(�༴������)
				cur = first;		//�ĵ�һ��Ԫ��
			}
			return *this;
		}
		self operator++(int) {
			self tmp = *this;
			++*this;
			return tmp;
		}
		self& operator--() {
			if (cur == first) {		//����Ѵ����ڻ�������ͷ��
				set_node(node - 1);	//���л���ǰһ�ڵ�(�༴������)
				cur = last;			//�����һ��Ԫ��
			}
			--cur;					//�л���ǰһ��Ԫ��
			return *this;
		}
		self operator--(int) {
			self tmp = *this;
			--*this;
			return tmp;
		}

		//����ʵ�������ȡ������������ֱ����Ծn������
		self& operator += (difference_type n) {
			difference_type offset = n + (cur - first);
			if (offset >= 0 && offset < difference_type(buffer_size())) {
				//Ŀ��λ����ͬһ��������
				cur += n;
			}
			else {
				//Ŀ��λ���ڲ�ͬһ��������
				difference_type node_offset =
					(offset > 0) ? offset / difference_type(buffer_size())
					: (-difference_type((-offset - 1) / buffer_size()) - 1);
				//�л�����ȷ�Ľڵ�(�༴������)
				set_node(node + node_offset);
				//�л�����ȷ��Ԫ��
				cur = first + (offset - node_offset * difference_type(buffer_size()));
			}

			return *this;
		}

		self operator+(difference_type n) const {
			self tmp = *this;
			return tmp += n;		//����operator+=
		}

		self& operator -= (difference_type n) { return *this += -n; }
		//��������operator += ����� operator -=

		self operator-(difference_type n) const {
			self tmp = *this;
			return tmp -= n;		//����operator-=
		}

		//����ʵ�������ȡ������������ֱ����Ծn������
		reference operator[] (difference_type n) const { return *(*this + n); }
		//���ϵ���operator*�� operator+

		bool operator==(const self& x) const { return cur == x.cur; }
		bool operator!=(const self& x) const { return !(*this == x); }
		bool operator<(const self& x) const {
			return (node == x.node) ? (cur < x.cur) : (node < x.node);
		}
	};

	template <class T, class Alloc = alloc, size_t BufSiz = 0>
	class deque {
	public:		//Basic types
		typedef typename __deque_iterator<T, T&, T*, BufSiz>::iterator			iterator;
		typedef typename __deque_iterator<T, T&, T*, BufSiz>::iterator_category	category;
		typedef typename __deque_iterator<T, T&, T*, BufSiz>::value_type		value_type;
		typedef typename __deque_iterator<T, T&, T*, BufSiz>::pointer			pointer;
		typedef typename __deque_iterator<T, T&, T*, BufSiz>::reference			reference;
		typedef typename __deque_iterator<T, T&, T*, BufSiz>::const_reference	const_reference;
		typedef typename __deque_iterator<T, T&, T*, BufSiz>::size_type			size_type;
		typedef typename __deque_iterator<T, T&, T*, BufSiz>::difference_type	difference_type;

	protected:		//Internal typedefs
		//Ԫ�ص�ָ���ָ��(pointer of pointer of T)
		typedef pointer* map_pointer;

		//ר���ռ���������ÿ������һ��Ԫ�ش�С
		typedef simple_alloc<value_type, Alloc> data_allocator;
		//ר���ռ���������ÿ������һ��ָ���С
		typedef simple_alloc<pointer, Alloc> map_allocator;

		size_t(*buffer_size)() = __deque_iterator<T, T&, T*, BufSiz>::buffer_size;
		enum { initial_map_size = 8 };

	protected:		//Data members
		iterator start;		//��һ���������ĵ�һ��Ԫ��
		iterator finish;	//���һ�������������һ��Ԫ��

		map_pointer map;		//ָ��map��map�ǿ������ռ䣬���ڵ�ÿ��Ԫ�ض���һ��ָ��(��Ϊ�ڵ�)��ָ��һ�黺����
		size_type map_size;		//map�ڿ����ɶ���ָ��

	public:		//Basic accessors
		deque() : start(), finish(), map(0), map_size(0) {
			create_map_and_nodes(0);
		}
		deque(int n, const value_type& value) :start(), finish(), map(0), map_size(0) {
			fill_initialize(n, value);
		}

		iterator begin() { return start; }
		iterator end() { return finish; }

		reference operator[](size_type n) {
			return start[difference_type(n)];	//���� __deque_iterator<>::operator[]
		}

		reference front() { return *start; }		//���� __deque_iterator<>::operator*
		reference back() {
			iterator tmp = finish;
			--tmp;		//���� __deque_iterator<>::operator--
			return *tmp;		//���� __deque_iterator<>::operator*
		}

		size_type size() const { return finish - start; }
		//���ϵ���iterator::operator-
		size_type max_size() const { return size_type(-1); }
		bool empty() const { return finish == start; }

		void fill_initialize(size_type n, const value_type& value);
		void create_map_and_nodes(size_type num_elements);

		void push_back(const value_type& t) {
			if (finish.cur != finish.last - 1) {
				//��󻺳�������һ�����ϵı��ÿռ�
				construct(finish.cur, t);		//ֱ���ڱ��ÿռ��Ϲ���Ԫ��
				++finish.cur;			//������󻺳�����ʹ��״̬
			}
			else {	//��󻺳�������(��ֻʣһ��)Ԫ�ر��ÿռ�
				push_back_aux(t);
			}
		}
		//ֻ��finish.cur == finish.last - 1(�����һ��������ֻʣһ�����ÿռ�)ʱ�Żᱻ����
		void push_back_aux(const value_type& t);

		void push_front(const value_type& t) {
			if (start.cur != start.first) {	//��һ���������б��ÿռ�
				construct(start.cur - 1, t);	//ֱ���ڱ��ÿռ��Ϲ���Ԫ��
				--start.cur;		//������һ��������ʹ��״̬
			}
			else {	//��һ���������ޱ��ÿռ�
				push_front_aux(t);
			}
		}
		//ֻ�е�start.cur == start.first(��һ��������û���κα��ÿռ�)ʱ�Żᱻ����
		void push_front_aux(const value_type& t);

		//�������������ж�ʲôʱ��map��Ҫ��������
		void reserve_map_at_back(size_type nodes_to_add = 1) {
			if (nodes_to_add + 1 > map_size - (finish.node - map)) {
				//��� map β�˵Ľڵ㱸�ÿռ䲻��
				//������������������ػ�һ�� map (���ø���ģ�����ԭ���ģ��ͷ�ԭ����)
				reallocate_map(nodes_to_add, false);
			}
		}
		void reserve_map_at_front(size_type nodes_to_add = 1) {
			if (nodes_to_add > size_type(start.node - map)) {
				//��� map ǰ�˵Ľڵ㱸�ÿռ䲻��
				//������������������ػ�һ�� map (���ø���ģ�����ԭ���ģ��ͷ�ԭ����)
				reallocate_map(nodes_to_add, true);
			}
		}
		//�������� map
		void reallocate_map(size_type nodes_to_add, bool add_at_front);

		void pop_back() {
			if (finish.cur != finish.first) {
				//��󻺳�����һ��(�����)Ԫ��
				--finish.cur;			//����ָ�룬�൱���ų������Ԫ��
				destroy(finish.cur);	//�����Ԫ������
			}
			else {
				//��󻺳���û���κ�Ԫ��
				pop_back_aux();			//���ｫ���л��������ͷŹ���
			}
		}
		//ֻ�е� finish.cur == finish.first ʱ�Żᱻ����
		void pop_back_aux();

		void pop_front() {
			if (start.cur != start.last - 1) {
				//��һ����������һ��(�����)Ԫ��
				destroy(start.cur);		//����һԪ������
				++start.cur;			//����ָ�룬�൱���ų��˵�һԪ��
			}
			else {
				//��һ����������һ��Ԫ��
				pop_front_aux();		//���ｫ���л��������ͷŹ���
			}
		}
		//ֻ�е� start.cur == start.last - 1 ʱ�Żᱻ����
		void pop_front_aux();

		void clear();
		
		//��� pos ��ָ��Ԫ�أ�pos Ϊ�����
		iterator erase(iterator pos) {
			iterator next = pos;
			++next;
			difference_type index = pos - start;	//�����֮ǰ��Ԫ�ظ���
			if (index < (size() >> 1)) {			//��������֮ǰ��Ԫ�رȽ���
				copy_backward(start, pos, next);	//���ƶ������֮ǰ��Ԫ��
				pop_front();						//�ƶ���ϣ���ǰһ��Ԫ�����࣬ȥ��֮
			}
			else {						//�����֮���Ԫ�رȽ���
				copy(next, finish, pos);//���ƶ������֮���Ԫ��
				pop_back();				//�ƶ���ϣ����һ��Ԫ�����࣬ȥ��֮
			}
			return start + index;
		}

		//��� [first, last) �����ڵ�����Ԫ��
		iterator erase(iterator first, iterator last);

		//�� position ��(֮ǰ)����һ��Ԫ�أ���ֵΪ x
		iterator insert(iterator position, const value_type& x) {
			if (position.cur == start.cur) {	//���������� deque ��ǰ��
				push_front(x);				//���� push_front ȥ��
				return start;
			}
			else if (position.cur == finish.cur) {	//���������� deque ��β��
				push_back(x);					//���� push_back ȥ��
				iterator tmp = finish;
				--tmp;

				return tmp;
			}
			else {
				return insert_aux(position, x);		//���� insert_aux ȥ��
			}
		}
		iterator insert_aux(iterator pos, const value_type& x);
	};

	template <class T, class Alloc, size_t BufSiz> 
	void deque<T, Alloc, BufSiz>::fill_initialize(size_type n, const value_type& value) {
		create_map_and_nodes(n);		//��deque�Ľṹ�����������ź�
		map_pointer cur;
		try {
			//Ϊÿ���ڵ�Ļ������趨��ֵ
			for (cur = start.node; cur < finish.node; ++cur) {
				uninitialized_fill(*cur, *cur + buffer_size(), value);
			}
			//���һ���ڵ���趨���в�ͬ(��Ϊβ�˿����б��ÿռ䣬�������ֵ)
			uninitialized_fill(finish.first, finish.cur, value);
		}
		catch (...) {
			//���ٳɹ������Ԫ��
			for (map_pointer beg = start.node; beg < cur; ++beg)
				destroy(*cur, *cur + buffer_size());
			//�ͷŻ����� 
			for (cur = start.node; cur <= finish.node; ++cur)
				data_allocator::deallocate(*cur, buffer_size());
			// �ͷ� map ��ָ�ռ�
			map_allocator::deallocate(map, map_size);
			throw;
		}
	}

	template <class T, class Alloc, size_t BufSiz> 
	void deque<T, Alloc, BufSiz>::create_map_and_nodes(size_type num_elements) {
		//��Ҫ�ڵ���=(Ԫ�ظ���/ÿ�������������ɵ�Ԫ�ظ���)+1
		//����պ������������һ���ڵ�
		size_type num_nodes = num_elements / buffer_size() + 1;

		//һ��mapҪ�������ڵ㡣����8��������� ����ڵ�����2��ǰ���Ԥ��һ��������ʱ����
		map_size = initial_map_size>(num_nodes + 2) ? initial_map_size : (num_nodes+2);
		map = map_allocator::allocate(map_size);
		//�������ó�һ�� ����map_size���ڵ� ��map

		//������nstart��nfinishָ��map��ӵ��֮ȫ���ڵ������������
		//�����������룬��ʹͷβ���˵���������һ����ÿ���ڵ�ɶ�Ӧһ��������
		map_pointer nstart = map + (map_size - num_nodes) / 2;
		map_pointer nfinish = nstart + num_nodes - 1;

		map_pointer cur;
		try {
			//Ϊmap�ڵ�ÿ�����ýڵ����û����������л���������������deque�Ŀ��ÿռ�(���һ��������������һЩ����)
			for (cur = nstart; cur <= nfinish; ++cur) {
				*cur = data_allocator::allocate(buffer_size());
			}
		}
		catch (...) {
			//����ȫ���ɹ���һ������
			for (map_pointer b = nstart; b < cur; ++b) {
				data_allocator::deallocate(*b, buffer_size());
			}
			map_allocator::deallocate(map, map_size);
			throw;
		}
		/*Ϊdeque�ڵ�����������start��end�趨��ȷ���ݣ�
			startָ���һ����������start.curָ���һ���������ĵ�һ��Ԫ��
			finishָ�����һ����������finish.curָ�����һ�������������һ��Ԫ��*/
		start.set_node(nstart);
		finish.set_node(nfinish);
		start.cur = start.first;
		finish.cur = finish.first + num_elements % buffer_size();
		//ǰ���ᵽ������պ������������һ���ڵ㣬��ʱ��curָ���������Ľڵ�(����ӳ�Ļ�����)����ʼ��������û��Ԫ�أ�
	}

	template <class T, class Alloc, size_t BufSiz>
	void deque<T, Alloc, BufSiz>::push_back_aux(const value_type& t) {
		value_type t_copy = t;
		reserve_map_at_back();		//map�ı��ÿռ������꣬�޷������ӻ������ڵ㣬�����ػ�һ������� map
		*(finish.node + 1) = data_allocator::allocate(buffer_size());		//����һ���½ڵ�(������)
		try {
			construct(finish.cur, t_copy);		//���finish��ָ�����������һ���洢�ڵ㹹��Ԫ��
			finish.set_node(finish.node + 1);	//�ı�finish������ָ���½ڵ�
			finish.cur = finish.first;			//�趨finish��״̬
		}
		catch (...) {
			//�ͷ�finish��ָ�ڵ����һ���ڵ�(�������õĽڵ�)
			data_allocator::deallocate(*(finish.node + 1));
			throw;
		}
	}

	template <class T, class Alloc, size_t BufSiz>
	void deque<T, Alloc, BufSiz>::push_front_aux(const value_type& t) {
		value_type t_copy = t;
		reserve_map_at_front();		//map�ı��ÿռ������꣬�޷������ӻ������ڵ㣬�����ػ�һ������� map
		*(start.node - 1) = data_allocator::allocate(buffer_size());		//����һ���½ڵ�(������)
		try {
			start.set_node(start.node - 1);		//�ı�start������ָ���½ڵ�
			start.cur = start.last - 1;			//�趨start��״̬
			construct(start.cur, t_copy);		//����Ԫ��
		}
		catch (...) {
			//����ȫ�ɹ���һ������ȫ���ͷ�
			start.set_node(start.node + 1);
			start.cur = start.first;
			data_allocator::deallocate(*(start.node - 1));
			throw;
		}
	}

	template <class T, class Alloc, size_t BufSiz>
	void deque<T, Alloc, BufSiz>::reallocate_map(size_type nodes_to_add, bool add_at_front) {
		size_type old_num_nodes = finish.node - start.node + 1;
		size_type new_num_nodes = old_num_nodes + nodes_to_add;

		map_pointer new_nstart;
		if (map_size > 2 * new_num_nodes) {
			new_nstart = map + (map_size - new_num_nodes) / 2
				+ (add_at_front ? nodes_to_add : 0);
			if (new_nstart < start.node) {
				copy(start.node, finish.node + 1, new_nstart);
			}
			else {
				copy_backward(start.node, finish.node + 1, new_nstart + old_num_nodes);
			}
		}
		else {
			size_type new_map_size = map_size + max(map_size, nodes_to_add) + 2;
			//����һ��ռ䣬׼������ map ʹ��
			map_pointer new_map = map_allocator::allocate(new_map_size);
			new_nstart = new_map + (new_map_size - new_num_nodes) / 2
				+ (add_at_front ? nodes_to_add : 0);
			//��ԭ map ���ݿ�������
			copy(start.node, finish.node + 1, new_nstart);
			//�ͷ�ԭ map
			map_allocator::deallocate(map, map_size);
			//�趨�� map ����ʼ��ַ���С
			map = new_map;
			map_size = new_map_size;
		}

		//�����趨������ start �� finish
		start.set_node(new_nstart);
		finish.set_node(new_nstart + old_num_nodes - 1);
	}

	template <class T, class Alloc, size_t BufSiz>
	void deque<T, Alloc, BufSiz>::pop_back_aux() {
		data_allocator::deallocate(finish.first, buffer_size());		//�ͷ����һ��������
		finish.set_node(finish.node - 1);	//���� finish ��״̬��ʹָ��
		finish.cur = finish.last - 1;		// ��һ�������������һ��Ԫ��
		destroy(finish.cur);				//������Ԫ��
	}

	template <class T, class Alloc, size_t BufSiz> 
	void deque<T, Alloc, BufSiz>::pop_front_aux() {
		destroy(start.cur);					//����һ�������ĵ�һ��Ԫ������
		data_allocator::deallocate(start.first, buffer_size());		//�ͷŵ�һ������
		start.set_node(start.node + 1);		//���� start ��״̬��ʹָ��
		start.cur = start.first;			// ��һ���������ĵ�һ��Ԫ��
	}

	//clear���֮��ָ���ʼ״̬��Ҫ����һ��������
	template <class T, class Alloc, size_t BufSiz>
	void deque<T, Alloc, BufSiz>::clear() {
		//�������ͷβ�����ÿһ��������(����һ�����Ǳ�����)
		for (map_pointer node = start.node + 1; node < finish.node; ++node) {
			//���������ڵ�����Ԫ��������ע����õ���destroy()
			destroy(*node, *node + buffer_size());
			//�ͷŻ������ڴ�
			data_allocator::deallocate(*node, buffer_size());
		}

		if (start.node != finish.node) {		//������ͷβ����������
			destroy(start.cur, start.last);		//��ͷ��������Ŀǰ����Ԫ������
			destroy(finish.first, finish.cur);	//��β��������Ŀǰ����Ԫ������
			//�����ͷ�β��������ע�⣬ͷ����������
			data_allocator::deallocate(finish.first, buffer_size());
		}
		else {		//ֻ��һ��������
			destroy(start.cur, finish.cur);		//����Ψһ������������Ԫ������
			//ע�⣺�����ͷŻ������ռ䣬��Ψһ�Ļ�����������
		}
		finish = start;		//����״̬
	}

	template <class T, class Alloc, size_t BufSiz> 
	typename deque<T, Alloc, BufSiz>::iterator
		deque<T, Alloc, BufSiz>::erase(iterator first, iterator last) {
		if (first == start && last == finish) {	//����������������� deque
			clear();		//ֱ�ӵ��� clear() ����
			return finish;
		}
		else {
			difference_type n = last - first;				//������������
			difference_type elems_before = first - start;	//�������ǰ����Ԫ�ظ���
			if (elems_before < (size() - n) / 2) {			//���ǰ����Ԫ�رȽ���
				copy_backward(start, first, last);		//����ƶ�ǰ��Ԫ��(�����������)
				iterator new_start = start + n;			//��� deque �������
				destroy(start, new_start);				//�ƶ���ϣ��������Ԫ������
				//���½�����Ļ������ͷ�
				for (map_pointer cur = start.node; cur < new_start.node; ++cur) {
					data_allocator::deallocate(*cur, buffer_size());
				}
				start = new_start;			//�趨 deque �������
			}
			else {		//����������󷽵�Ԫ�رȽ���
				copy(last, finish, first);			//��ǰ�ƶ���Ԫ��(�����������)
				iterator new_finish = finish - n;	//��� deque ����β��
				destroy(new_finish, finish);		//�ƶ���ϣ��������Ԫ������
				//���½�����Ļ������ͷ�
				for (map_pointer cur = new_finish.node + 1; cur <= finish.node; ++cur) {
					data_allocator::deallocate(*cur, buffer_size());
				}
				finish = new_finish;		//�趨 deque ����β��
			}
			return start + elems_before;
		}
	}

	template <class T, class Alloc, size_t BufSiz>
	typename deque<T, Alloc, BufSiz>::iterator
		deque<T, Alloc, BufSiz>::insert_aux(iterator pos, const value_type& x) {
		difference_type index = pos - start;		//�����֮ǰ��Ԫ�ظ���
		value_type x_copy = x;
		if (index < size() / 2) {			//��������֮ǰ��Ԫ�ظ����Ƚ���
			push_front(front());			//����ǰ�˼������һԪ��ֵͬ��Ԫ��
			iterator front1 = start;		//���±�ǣ�Ȼ�����Ԫ���ƶ�
			++front1;
			iterator front2 = front1;
			++front2;
			pos = start + index;
			iterator pos1 = pos;
			++pos1;
			copy(front2, pos1, front1);		//Ԫ���ƶ�
		}
		else {								//�����֮���Ԫ�ظ����Ƚ���
			push_back(back());				//����β�˼��������Ԫ��ֵͬ��Ԫ��
			iterator back1 = finish;		//���±�ǣ�Ȼ�����Ԫ���ƶ�
			--back1;
			iterator back2 = back1;
			--back2;
			pos = start + index;
			copy_backward(pos, back2, back1);	//Ԫ���ƶ�
		}
		*pos = x_copy;			//�ڲ�������趨��ֵ
		return pos;
	}

__MYSTL_NAMESPACE_END_

#endif // !_ZSTL_DEQUE_H_
