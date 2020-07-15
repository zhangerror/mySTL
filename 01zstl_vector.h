#ifndef _ZSTL_VECTOR_H_
#define _ZSTL_VECTOR_H_

#include "00zstl_alloc.h"
#include "00zstl_construct.h"

__MYSTL_NAMESPACE_BEGIN_

	template <class T, class Alloc = alloc >
	class vector {
	public:
		// vector的嵌套型别定义
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
		iterator start;			//表示目前使用空间的头
		iterator finish;		//表示目前使用空间的尾
		iterator end_of_storage;//表示目前可用空间的尾

		void insert_aux(iterator position, const T& x);
		void deallocate() {
			if (start) {
				data_allocator::deallocate(start, end_of_storage - start);
			}
		}

		//ROUND_UP()将byte上调至8的倍数
		static size_t ROUND_UP(size_t bytes) {
			return ((bytes + __ALIGN - 1) & ~(__ALIGN - 1));
		}

		//配置空间并填满内容
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
		vector(const vector<T>& x) {				//拷贝构造函数
			fill_initialize(x.size(), T());
			for (size_type i = 0; i < size(); ++i)
				*(start + i) = *(x.start + i);
		}
		/* 列表初始化 */
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
		//从position开始，插入n个元素，元素初值为x
		void insert(iterator position, size_type n, const T& x);
		//在position插入值为x的元素
		void insert(iterator postion, const T& x);

		void push_back(const T& x) {
			if (finish != end_of_storage) {	//还有备有空间
				construct(finish, x);	//global
				++finish;				//调整水位高度
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
				copy(position + 1, finish, position); //后续元素往前移动
			}
			--finish;
			destroy(finish);		//global
			return position;
		}

		//删除[first, last)中所有元素
		iterator erase(iterator first, iterator last) {
			//将[last, finish)的内容移动到从first开始位置，返回最后复制的下一位置
			iterator i = copy(last, finish, first);

			/* 销毁 [i, finish) 内的所有内容 */
			destroy(i, finish);
			finish = i;		//更新 finish 指向新的结尾
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
		if (finish != end_of_storage) {	//还有备用空间
			//在备用空间起始处构造一个元素，并以vector最后一个元素值作为其初值
			construct(finish, *(finish - 1));
			++finish;	//调整水位
			T x_copy = x;
			copy_backward(position, finish - 2, finish - 1);
			*position = x_copy;
		}
		else {	//已无备用空间
			const size_type old_size = size();
			const size_type len = old_size != 0 ? 2 * old_size : 1;
			//以上配置原则：如果原大小为0，则配置1(个元素大小)
			//如果原大小不为0，则配置原大小的两倍
			//前半段用来放之原数数据，后半段准备用来放置新数据

			iterator new_start = data_allocator::allocate(len);	//实际配置
			iterator new_finish = new_start;
			try {
				//将原vector的内容拷贝到新vector
				new_finish = uninitialized_copy(start, position, new_start);
				//为新元素设定初值x
				construct(new_finish, x);
				++new_finish;	//调整水位
				//将安插点的原内容也拷贝过来(本函数也可能被insert(p,x)调用)
				new_finish = uninitialized_copy(position, finish, new_finish);
			}
			catch (...) {
				//"commit or rollback" semantics
				destroy(new_start, new_finish);
				data_allocator::deallocate(new_start, len);
				throw;
			}

			//析构并释放原vector
			destroy(begin(), end());
			deallocate();

			//调整迭代器，指向新vector
			start = new_start;
			finish = new_finish;
			end_of_storage = new_start + len;
		}
	}

	template <class T, class Alloc>
	void vector<T, Alloc>::insert(iterator position, size_type n, const T& x) {
		if (n != 0) {
			if (size_type(end_of_storage - finish) >= n) {	//备用空间大于新增元素个数
				T x_copy = x;
				//以下计算插入点之后的现有元素个数
				const size_type elems_after = finish - position;
				iterator old_finish = finish;
				if (elems_after > n) {	//安插点之后的现有元素个数大于新增元素个数
					uninitialized_copy(finish - n, finish, finish);
					finish += n;	//将vector尾端标记后移
					copy_backward(position, old_finish - n, old_finish);
					fill(position, position + n, x_copy);
				}
				else {
					//插入点之后的现有元素个数小于等于新增元素个数
					uninitialized_fill_n(finish, n - elems_after, x_copy);
					finish += n - elems_after;
					uninitialized_copy(position, old_finish, finish);
					fill(position, position + n, x_copy);
				}
			}
			else {
				//备用空间小于新增元素个数(必须配置额外内存)
				//首先决定新长度：旧长度的两倍，或旧长度+新增元素个数
				const size_type old_size = size();
				const size_type len = old_size + max(old_size, n);
				//以下配置新的vector空间
				iterator new_start = data_allocator::allocate(len);
				iterator new_finish = new_start;
				try {
					//以下首先将旧vector的插入点之前的元素复制到新空间
					new_finish = uninitialized_copy(start, position, new_start);
					//以下再将新增元素(初值为n)填入空间
					new_finish = uninitialized_fill_n(new_finish, n, x);
					//以下再将旧vector的插入点之后的元素复制到新空间
					new_finish = uninitialized_copy(position, finish, new_finish);
				}
				catch (...) {
					//如果异常发生，实现"commit or rollback" semantics
					destroy(new_start, new_finish);
					data_allocator::deallocate(new_start, len);
					throw;
				}
				//以下清除并释放旧的vector
				destroy(start, finish);
				deallocate();
				//以下调整水位标记
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
