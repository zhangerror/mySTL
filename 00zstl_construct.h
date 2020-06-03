#ifndef _ZSTL_CONSTRUCT_H_
#define _ZSTL_CONSTRUCT_H_

#include "00zstl_iterator.h"
#include "00zstl_algobase.h"
#include <new.h>	//placement new
//#include <memory>	//memmove：内存底层操作，速度极快
#include <string.h>

__MYSTL_NAMESPACE_BEGIN_
	template <class T1, class T2>
	inline void construct(T1* p, const T2& value) {
		new(p) T1(value);	//调用T1::T1(value)
	}

	template <class T>
	inline void destroy(T* pointer) {
		pointer->~T();
	}

	//以下是destroy()第二版本，接受两个迭代器，此函数设法找出元素的数值型别，
	//进而利用__type_traits<>求取最适当措施
	template<typename ForwardIterator, typename T>		//函数声明
	inline void __destroy(ForwardIterator, ForwardIterator, T*);

	//如果元素的数值型别有non-trivial destructor
	template <class ForwardIterator>
	inline void
		__destroy_aux(ForwardIterator first, ForwardIterator last, __false_type) {
		for (; first < last; ++first) {
			destroy(&*first);
		}
	}

	//如果元素的数值型别有trivial destructor
	template <class ForwardIterator>
	inline void __destroy_aux(ForwardIterator, ForwardIterator, __true_type) {}

	template <class ForwardIterator>
	inline void destroy(ForwardIterator first, ForwardIterator last) {
		__destroy(first, last, value_type(first));
	}

	//判断元素的数值型别(value type)是否有trival destructor
	template <class ForwardIterator, class T>
	inline void __destroy(ForwardIterator first, ForwardIterator last, T*) {
		typedef typename __type_traits<T>::has_trivial_destructor trivial_destructor;
		__destroy_aux(first, last, trivial_destructor());
	}

	//以下是destory()第二版本针对迭代器为char*和wchar_t*的特化版
	inline void destroy(char*, char*) {}
	inline void destroy(wchar_t*, wchar_t*) {}

				/*	内存基本处理工具	*/

	//uninitialized_copy：对应高层copy()
	/*	迭代器first指向输入端的起始位置
		迭代器last指向输入端的结束位置
		迭代器result指向输出端(欲初始化空间)的起始处
	*//*
	template <class InputIterator, class ForwardIterator>
	ForwardIterator
		uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result);*/

	//如果copy construction 等同于assignment，而且destructor是trivial，以下有效
	//如果是POD型别，执行流程就会转进到以下格式，这是由function template的参数推导机制而得
	template <class InputIterator, class ForwardIterator>
	inline ForwardIterator
		__uninitialized_copy_aux(InputIterator first, InputIterator last, ForwardIterator result, __true_type) {
		return copy(first, last, result);	//调用STL算法copy()
	}

	//如果是non-POD型别，执行流程转进到以下函数，这是function template的参数推导机制而得
	template <class InputIterator, class ForwardIterator>
	ForwardIterator
		__uninitialized_copy_aux(InputIterator first, InputIterator last, ForwardIterator result, __false_type) {
		ForwardIterator cur = result;
		try {
			for (; first != last; ++first, ++cur) {
				construct(&*cur, *first);		//必须一个一个的构造，无法批量处理、
			}
		}
		catch (...) {	//发生异常，[result,cur)内的元素已完成构造，逐个析构
			ForwardIterator p = result;
			for (; p != cur; ++p) {
				destroy(&*p);
			}
			throw;
		}

		return cur;
	}

	template <class InputIterator, class ForwardIterator, class T>
	inline ForwardIterator
		__uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result, T*) {
		typedef typename __type_traits<T>::is_POD_type is_POD;
		return __uninitialized_copy_aux(first, last, result, is_POD());
		//以上，企图利用is_POD()所获得的结果，让编译器做参数推导
	}

	template <class InputIterator, class ForwardIterator>
	inline ForwardIterator
		uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result) {
		return __uninitialized_copy(first, last, result, value_type(result));	//利用value_type取出first的value type
	}

	//针对const char* 的特化版本
	inline char* uninitialized_copy(const char* first, const char* last, char* result) {
		memmove(result, first, last - first);
		return result + (last - first);
	}

	//针对const wchar_t* 的特化版本
	inline wchar_t* uninitialized_copy(const wchar_t* first, const wchar_t* last, wchar_t* result) {
		memmove(result, first, sizeof(wchar_t) * (last - first));
		return result + (last - first);
	}


	//uninitialized_fill：对应高层fill()
	/*	迭代器first指向输出端(欲初始化空间)的起始位置
		迭代器last指向输出端(欲初始化空间)的结束位置
		x 表示初值
	*//*
	template <class ForwardIterator, class T>
	void uninitialized_fill(ForwardIterator first, ForwardIterator last, const T& x);*/

	//如果copy construction等同于assignment，而且destructor是trivial，以下就有效
	//如果是POD型别，执行流程就会转进到以下函数，这是由functioin template的参数推导机制而得
	template <class ForwardIterator, class T>
	inline void
		__uninitialized_fill_aux(ForwardIterator first, ForwardIterator last, const T& x, __true_type) {
		fill(first, last, x);	//调用STL算法fill()
	}

	//如果是non-POD型别，执行流程就会转进到以下函数，这是由function template的参数推导机制而得
	template <class ForwardIterator, class T>
	void
		__uninitialized_fill_aux(ForwardIterator first, ForwardIterator last, const T& x, __false_type) {
		ForwardIterator cur = first;
		try {
			for (; cur != last; ++cur) {
				construct(&*cur, x);		//必须一个一个的构造，无法批量处理
			}
		}
		catch (...) {	//发生异常，[first,cur)内的元素已完成构造，逐个析构
			ForwardIterator p = first;
			for (; p != cur; ++p) {
				destroy(&*p);
			}
			throw;
		}
	}

	//先萃取出迭代器first的value type，然后判断该型别是否为POD
	template <class ForwardIterator, class T, class T1>
	inline void __uninitialized_fill(ForwardIterator first, ForwardIterator last, const T& x, T1*) {
		typedef typename __type_traits<T1>::is_POD_type is_POD;
		__uninitialized_fill_aux(first, last, x, is_POD());
	}

	template <class ForwardIterator, class T>
	inline void uninitialized_fill(ForwardIterator first, ForwardIterator last, const T& x) {
		__uninitialized_fill(first, last, x, value_type(first));
	}

	//uninitialized_fill_n：对应高层fill_n()
	/*	迭代器first指向欲初始化空间的起始处
		n表示初始化空间的大小
		x表示初值
	*//*
	template <class ForwardIterator, class Size, class T>
	ForwardIterator
		uninitialized_fill_n(ForwardIterator first, Size n, const T& x);
*/
	//如果copy construction等同于assignment，而且destructor是trivial，以下就有效
	//如果是POD型别，执行流程就会转进到以下函数，这是由function template的参数推导而得
	template <class ForwardIterator, class Size, class T>
	inline ForwardIterator
		__uninitialized_fill_n_aux(ForwardIterator first, Size n, const T& x, __true_type) {
		//交由高阶函数执行
		return fill_n(first, n, x);
	}
	//如果不是POD型别，执行流程就会转进到以下函数，这是由function template的参数推导机制而得
	template <class ForwardIterator, class Size, class T>
	inline ForwardIterator
		__uninitialized_fill_n_aux(ForwardIterator first, Size n, const T& x, __false_type) {
		ForwardIterator cur = first;
		try {
			for (; n > 0; --n, ++cur) {
				construct(&*cur, x);		//必须一个一个的构造，无法批量处理
			}
		}
		catch (...) {	//发生异常，[first,cur)内的元素已完成构造，逐个析构
			ForwardIterator p = first;
			for (; p != cur; ++p) {
				destroy(&*p);
			}
			throw;
		}
	}

	template <class ForwardIterator, class Size, class T, class T1>
	inline ForwardIterator __uninitialized_fill_n(ForwardIterator first, Size n, const T& x, T1*) {
		//以下 __type_traits<>技法
		typedef typename __type_traits<T1>::is_POD_type is_POD;
		return __uninitialized_fill_n_aux(first, n, x, is_POD());
	}

	template <class ForwardIterator, class Size, class T>
	inline ForwardIterator uninitialized_fill_n(ForwardIterator first, Size n, const T& x) {
		return __uninitialized_fill_n(first, n, x, value_type(first));
		//以上，利用vaule_type()取出first的value type
	}

__MYSTL_NAMESPACE_END_

#endif // !_ZSTL_CONSTRUCT_H_
