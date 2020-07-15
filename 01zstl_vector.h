#ifndef _ZSTL_VECTOR_H_
#define _ZSTL_VECTOR_H_

#include "00zstl_alloc.h"
#include "00zstl_construct.h"

__MYSTL_NAMESPACE_BEGIN_

	template <class T, class Alloc = alloc >
	class vector {
	public:
		// vector��Ƕ���ͱ���
		typedef T					value_type;
		typedef value_type*			pointer;
		typedef const value_type*	const_pointer;
		typedef value_type*			iterator;
		typedef const value_type*	const_iterator;
		typedef value_type&			reference;
		typedef const value_type&	const_reference;
		typedef size_t				size_type;
		typedef ptrdiff_t			difference_type;
	protected:
		typedef simple_alloc<value_type, Alloc> data_allocator;

	protected:
		iterator start;			//��ʾĿǰʹ�ÿռ��ͷ
		iterator finish;		//��ʾĿǰʹ�ÿռ��β
		iterator end_of_storage;//��ʾĿǰ���ÿռ��β

		void insert_aux(iterator position, const T& x);
		void deallocate() {
			if (start) {
				data_allocator::deallocate(start, end_of_storage - start);
			}
		}

		//ROUND_UP()��byte�ϵ���8�ı���
		static size_t ROUND_UP(size_t bytes) {
			return ((bytes + __ALIGN - 1) & ~(__ALIGN - 1));
		}

		//���ÿռ䲢��������
		iterator allocate_and_fill(size_type n, const T& x) {
			iterator result = data_allocator::allocate(n);
			uninitialized_fill_n(result, n, x);		//global
			return result;
		}

		void fill_initialize(size_type n, const T& value) {
			start = allocate_and_fill(n, value);
			finish = start + n;
			//end_of_storage = finish;

			end_of_storage = start + ROUND_UP(n * sizeof(value_type)) / sizeof(value_type);
		}

		template<class ForwardIterator>
		iterator __allocate_and_copy(size_type n, ForwardIterator first, ForwardIterator last) {
			iterator result = data_allocator::allocate(n);
			try {
				uninitialized_copy(first, last, result);
				return result;
			}
			catch (...) {
				data_allocator::deallocate(result, n);
			}
		}
	public:
		iterator begin() { return start; }
		iterator end() { return finish;  }
		size_type size() const { return size_type(finish - start); }
		size_type capacity() const {
			return size_type(end_of_storage - start);
		}
		bool empty() const { return (size() == 0); }
		reference operator[](size_type n) { return *(begin() + n); }
		//iterator operator[](size_type n) { return *(begin() + n); }
		vector<T, Alloc>& operator=(const vector<T, Alloc>& x);

		vector() : start(0), finish(0), end_of_storage(0) { }
		vector(size_type n, const T& value) { fill_initialize(n, value); }
		vector(int n, const T& value) { fill_initialize(n, value); }
		vector(long n, const T& value) { fill_initialize(n, value); }
		explicit vector(size_type n) { file_initialize(n, T()); }
		vector(const vector<T>& x) {				//�������캯��
			fill_initialize(x.size(), T());
			for (size_type i = 0; i < size(); ++i)
				*(start + i) = *(x.start + i);
		}
		/* �б��ʼ�� */
		vector(std::initializer_list<T> init_list) {
			fill_initialize(init_list.size(), T());
			auto beg = init_list.begin();
			for (size_type i = 0; i < size(); ++i)
				*(start + i) = *(beg + i);
		}

		~vector() {
			destroy(start, finish);		//global
			deallocate();		//member function
		}
		reference front() { return *begin(); }
		reference back() { return *(end() - 1); }
		//��position��ʼ������n��Ԫ�أ�Ԫ�س�ֵΪx
		void insert(iterator position, size_type n, const T& x);
		//��position����ֵΪx��Ԫ��
		void insert(iterator postion, const T& x);

		void push_back(const T& x) {
			if (finish != end_of_storage) {	//���б��пռ�
				construct(finish, x);	//global
				++finish;				//����ˮλ�߶�
			}
			else {
				insert_aux(end(), x);	//member function
			}
		}

		void pop_back() {
			--finish;
			destroy(finish);	//global
		}
		iterator erase(iterator position) {
			if (position + 1 != end()) {
				copy(position + 1, finish, position); //����Ԫ����ǰ�ƶ�
			}
			--finish;
			destroy(finish);		//global
			return position;
		}

		//ɾ��[first, last)������Ԫ��
		iterator erase(iterator first, iterator last) {
			//��[last, finish)�������ƶ�����first��ʼλ�ã���������Ƶ���һλ��
			iterator i = copy(last, finish, first);

			/* ���� [i, finish) �ڵ��������� */
			destroy(i, finish);
			finish = i;		//���� finish ָ���µĽ�β
			return first;
		}

		void resize(size_type new_size, const T& x) {
			if (new_size < size()) {
				erase(begin() + new_size, end());
			}
			else {
				insert(end(), new_size - size(), x);
			}
		}
		void resize(size_type new_size) { resize(new_size, T()); }
		void swap(vector<T, Alloc>& vec) {	
			std::swap(start, vec.start);
			std::swap(finish, vec.finish);
			std::swap(end_of_storage, vec.end_of_storage);
		}
		void clear() {	erase(begin(), end());	}

		void reserve(size_type n) {
			if (capacity() < n) {
				const size_type old_size = size();
				iterator tmp = __allocate_and_copy(n, start, finish);
				destroy(start, finish);
				data_allocator::deallocate(start, end_of_storage - start);
				start = tmp;
				finish = tmp + old_size;
				end_of_storage = start + n;
			}
		}
	};		//end of class vector

	template <class T, class Alloc>
	vector<T, Alloc>&
		vector<T, Alloc>::operator=(const vector<T, Alloc>& x) {
		if (&x != this) {
			const size_type x_len = x.size();
			if (x_len > capacity()) {
				iterator tmp = __allocate_and_copy(x_len, x.begin(), x.end());
				destroy(start, finish);
				deallocate(start, end_of_storage-start);
				start = tmp;
				end_of_storage = start + x_len;
			}
			else if (size() >= x_len) {
				iterator i = copy(x.begin(), x.end(), begin());
				destroy(i, finish);
			}
			else {
				copy(x.begin(), x.begin() + size(), start);
				uninitialized_copy(x.begin() + size(), x.end(), finish);
			}
			finish = start + x_len;
		}
		return *this;
	}

	template <class T, class Alloc> 
	void vector<T, Alloc>::insert_aux(iterator position, const T& x) {
		if (finish != end_of_storage) {	//���б��ÿռ�
			//�ڱ��ÿռ���ʼ������һ��Ԫ�أ�����vector���һ��Ԫ��ֵ��Ϊ���ֵ
			construct(finish, *(finish - 1));
			++finish;	//����ˮλ
			T x_copy = x;
			copy_backward(position, finish - 2, finish - 1);
			*position = x_copy;
		}
		else {	//���ޱ��ÿռ�
			const size_type old_size = size();
			const size_type len = old_size != 0 ? 2 * old_size : 1;
			//��������ԭ�����ԭ��СΪ0��������1(��Ԫ�ش�С)
			//���ԭ��С��Ϊ0��������ԭ��С������
			//ǰ���������֮ԭ�����ݣ�����׼����������������

			iterator new_start = data_allocator::allocate(len);	//ʵ������
			iterator new_finish = new_start;
			try {
				//��ԭvector�����ݿ�������vector
				new_finish = uninitialized_copy(start, position, new_start);
				//Ϊ��Ԫ���趨��ֵx
				construct(new_finish, x);
				++new_finish;	//����ˮλ
				//��������ԭ����Ҳ��������(������Ҳ���ܱ�insert(p,x)����)
				new_finish = uninitialized_copy(position, finish, new_finish);
			}
			catch (...) {
				//"commit or rollback" semantics
				destroy(new_start, new_finish);
				data_allocator::deallocate(new_start, len);
				throw;
			}

			//�������ͷ�ԭvector
			destroy(begin(), end());
			deallocate();

			//������������ָ����vector
			start = new_start;
			finish = new_finish;
			end_of_storage = new_start + len;
		}
	}

	template <class T, class Alloc>
	void vector<T, Alloc>::insert(iterator position, size_type n, const T& x) {
		if (n != 0) {
			if (size_type(end_of_storage - finish) >= n) {	//���ÿռ��������Ԫ�ظ���
				T x_copy = x;
				//���¼�������֮�������Ԫ�ظ���
				const size_type elems_after = finish - position;
				iterator old_finish = finish;
				if (elems_after > n) {	//�����֮�������Ԫ�ظ�����������Ԫ�ظ���
					uninitialized_copy(finish - n, finish, finish);
					finish += n;	//��vectorβ�˱�Ǻ���
					copy_backward(position, old_finish - n, old_finish);
					fill(position, position + n, x_copy);
				}
				else {
					//�����֮�������Ԫ�ظ���С�ڵ�������Ԫ�ظ���
					uninitialized_fill_n(finish, n - elems_after, x_copy);
					finish += n - elems_after;
					uninitialized_copy(position, old_finish, finish);
					fill(position, position + n, x_copy);
				}
			}
			else {
				//���ÿռ�С������Ԫ�ظ���(�������ö����ڴ�)
				//���Ⱦ����³��ȣ��ɳ��ȵ���������ɳ���+����Ԫ�ظ���
				const size_type old_size = size();
				const size_type len = old_size + max(old_size, n);
				//���������µ�vector�ռ�
				iterator new_start = data_allocator::allocate(len);
				iterator new_finish = new_start;
				try {
					//�������Ƚ���vector�Ĳ����֮ǰ��Ԫ�ظ��Ƶ��¿ռ�
					new_finish = uninitialized_copy(start, position, new_start);
					//�����ٽ�����Ԫ��(��ֵΪn)����ռ�
					new_finish = uninitialized_fill_n(new_finish, n, x);
					//�����ٽ���vector�Ĳ����֮���Ԫ�ظ��Ƶ��¿ռ�
					new_finish = uninitialized_copy(position, finish, new_finish);
				}
				catch (...) {
					//����쳣������ʵ��"commit or rollback" semantics
					destroy(new_start, new_finish);
					data_allocator::deallocate(new_start, len);
					throw;
				}
				//����������ͷžɵ�vector
				destroy(start, finish);
				deallocate();
				//���µ���ˮλ���
				start = new_start;
				finish = new_finish;
				end_of_storage = new_start + len;
			}
		}
	}

	template <class T, class Alloc>
	void vector<T, Alloc>::insert(iterator position, const T& x) {
		insert(position, 1, x);
	}

	template<class T, class Alloc>
	inline bool
		operator==(const vector<T, Alloc>& x, const vector<T, Alloc>& y) {
		return x.size() == y.size() && std::equal(x.begin(), x.end(), y.begin());
	}

	template<class T, class Alloc>
	inline bool
		operator<(const vector<T, Alloc>& x, const vector<T, Alloc>& y) {
		return std::lexicographical_compare(x.begin(), x.end(),	y.begin(), y.end());
	}

	template<class T, class Alloc>
	inline void 
		swap(const vector<T, Alloc>& x, const vector<T, Alloc>& y) {
		x.swap(y);
	}

	template<class T, class Alloc>
	inline bool
		operator!=(const vector<T, Alloc>& x, const vector<T, Alloc>& y) {
		return !(x == y);
	}

	template<class T, class Alloc>
	inline bool
		operator>(const vector<T, Alloc>& x, const vector<T, Alloc>& y) {
		return y < x;
	}

	template<class T, class Alloc>
	inline bool
		operator<=(const vector<T, Alloc>& x, const vector<T, Alloc>& y) {
		return !(y < x);
	}

	template<class T, class Alloc>
	inline bool
		operator>=(const vector<T, Alloc>& x, const vector<T, Alloc>& y) {
		return !(x < y);
	}

__MYSTL_NAMESPACE_END_

#endif // !_ZSTL_VECTOR_H_
