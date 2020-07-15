#ifndef _ZSTL_ALGOBASE_H_
#define _ZSTL_ALGOBASE_H_

//基本算法

#include <cstring>
#include "00zstl_config.h"
#include "00zstl_iterator.h"


__MYSTL_NAMESPACE_BEGIN_

	/* fill: 将[first, last) 内的所有元素改填新值 */
	template<class ForwardIterator, class T>
	void fill(ForwardIterator first, ForwardIterator last, const T& value) {
		for (; first != last; ++first) {	//迭代器走过整个区间
			*first = value;					//设定新值
		}
	}

	/*	fill_n: 将从 first 开始的前 n 个元素改填新值，返回的迭代器指向被填入的最后一个元素的下一位置 */
	template<class OutputIterator, class Size, class T>
	OutputIterator fill_n(OutputIterator first, Size n, const T& value) {
		for (; n > 0; --n, ++first) {		//迭代器走过 n 个元素
			*first = value;					//设定新值
		}
		return first;
	}

	/* max、 min */
	template<class T>
	inline const T& max(const T& a, const T& b) {
		return (a < b) ? b : a;
	}
	template<class T, class Compare>
	inline const T& max(const T& a, const T& b, Compare comp) {
		return comp(a, b) ? b : a;
	}

	template<class T>
	inline const T& min(const T& a, const T& b) {
		return (a < b) ? a : b;
	}
	template<class T, class Compare>
	inline const T& min(const T& a, const T& b, Compare comp) {
		return comp(b, a) ? b : a;
	}

	/* mismatch：用来平行比较两个序列，指处两者之间的第一个不匹配点，返回一对迭代器，分别指向两序列中的不匹配点 */
	template <class InputIterator1, class InputIterator2>
	pair<InputIterator1, InputIterator2> mismatch(InputIterator1 first1,
		InputIterator1 last1,
		InputIterator2 first2) {
		while (first1 != last1 && *first1 == *first2) {
			++first1;
			++first2;
		}
		return pair<InputIterator1, InputIterator2>(first1, first2);
	}

	template <class InputIterator1, class InputIterator2, class BinaryPredicate>
	pair<InputIterator1, InputIterator2> mismatch(InputIterator1 first1,
		InputIterator1 last1,
		InputIterator2 first2,
		BinaryPredicate binary_pred) {
		while (first1 != last1 && binary_pred(*first1, *first2)) {
			++first1;
			++first2;
		}
		return pair<InputIterator1, InputIterator2>(first1, first2);
	}


	/* copy: 将[first, last) 内的元素复制到 result 开始的位置，返回最后一个复制元素的下一位置 */
	template<class InputIterator, class OutputIterator> struct __copy_dispatch;		//模板类声明

	/* 完全泛化版本 */
	template<class InputIterator, class OutputIterator>
	inline OutputIterator copy(InputIterator first, InputIterator last, OutputIterator result) {
		return __copy_dispatch<InputIterator, OutputIterator>()(first, last, result);
	}
	/* 针对 const char* 的全特化版本 */
	inline char* copy(const char* first, const char* last, char* result) {
		memmove(result, first, last - first);
		return result + (last - first);
	}
	/* 针对 const wchar_t* 的全特化版本 */
	inline wchar_t* copy(const wchar_t* first, const wchar_t* last, wchar_t* result) {
		memmove(result, first, sizeof(wchar_t) * (last - first));
		return result + (last - first);
	}

	/* __copy_dispatch 完全泛化版本 */
	template<class InputIterator, class OutputIterator>
	struct __copy_dispatch {
		OutputIterator operator()(InputIterator first, InputIterator last, OutputIterator result) {
			return __copy(first, last, result, iterator_category(first));
		}
	};
	/* __copy_dispatch 针对 T* 的偏特化版本 */
	template<class T>
	struct __copy_dispatch<T*, T*> {	//两个参数都是 T* 指针形式
		T* operator()(T* first, T* last, T* result) {
			typedef typename __type_traits<T>::has_trivial_assignment_operator t;
			return __copy_t(first, last, result, t());
		}
	};
	/* __copy_dispatch 针对 const T* 的偏特化版本 */
	template<class T>
	struct __copy_dispatch<const T*, T*> {	//第一个参数为 const T*，第二个参数为 T* 形式
		T* operator()(const T* first, const T* last, T* result) {
			typedef typename __type_traits<T>::has_trivial_assignment_operator t;
			return __copy_t(first, last, result, t());
		}
	};

	/* __copy_dispatch() 的完全泛化版本根据迭代器种类的不同调用了不同的 __copy()，为的是不同种类的迭代器使用的循环条件不同，有快慢之分。 */
	/* InputIterator 版本 */
	template<class InputIterator, class OutputIterator>
	inline OutputIterator __copy(InputIterator first, InputIterator last, OutputIterator result, input_iterator_tag) {
		//以迭代器是否相等决定是否继续循环，速度慢
		for (; first != last; ++first, ++result)
			*result = *first;
		return result;
	}
	/* RandomAccessIterator 版本 */
	template<class RandomAccessIterator, class OutputIterator>
	inline OutputIterator __copy(RandomAccessIterator first, RandomAccessIterator last, OutputIterator result, random_access_iterator_tag) {
		//又划分出一个函数，为的是其他地方也能用到
		return __copy_d(first, last, result, distance_type(first));
	}

	template<class RandomAccessIterator, class OutputIterator, class Distance>
	inline OutputIterator __copy_d(RandomAccessIterator first, RandomAccessIterator last, OutputIterator result, Distance*) {
		//以 n 决定循环执行次数，速度快
		Distance n = last - first;
		for (; n > 0; --n, ++first, ++result)
			*result = *first;
		return result;
	}

	//以下版本适合于指针所指对象具备 trivial assignment operator
	template<class T>
	inline T* __copy_t(const T* first, const T* last, T* result, __true_type) {
		memmove(result, first, sizeof(T) * (last - first));
		return result + (last - first);
	}

	//以下版本适合于指针所指对象具备 non-trivial assignment operator
	template<class T>
	inline T* __copy_t(const T* first, const T* last, T* result, __false_type) {
		return __copy_d(first, last, result, (ptrdiff_t*)0);
	}


	/*	将 [first, last) 内的元素从后往前移动到 result 往前的位置(result不存储元素)	*/
		//注意：只有双向迭代器才能调用该函数
	template<typename InputIterator, typename ForwardIterator>
	void copy_backward(InputIterator first, InputIterator last, ForwardIterator result) {
		InputIterator begin = last;
		InputIterator stop = first;
		ForwardIterator dest = result;
		--begin; --stop; --dest;
		while (begin != stop) {
			*dest = *begin;
			--begin;
			--dest;
		}
	}

	/* lower_bound（应用于有序区间） */
		//版本一辅助函数的 forward_iterator 版本
	template<class ForwarIterator, class T, class Distance>
	ForwarIterator __lower_bound(ForwarIterator first, ForwarIterator last,
		const T& value, Distance*, forward_iterator_tag)
	{
		Distance len = 0;
		distance(first, last, len);		//求得整个区间的长度
		Distance half = 0;
		ForwarIterator mid;

		while (len > 0) {
			half = (len >> 1);		//通过移位实现除2
			mid = first;
			advance(mid, half);		//令 mid 指向区间中部
			if (*mid < value) {
				first = ++mid;
				len = len - half - 1;	//修正 half
			}
			else {
				len = half;
			}
		}
		return first;
	}
	//版本一辅助函数的 random_access_iterator_tag 版本
	template<class	RandomAccessIterator, class T, class Distace>
	RandomAccessIterator __lower_bound(RandomAccessIterator first, RandomAccessIterator last,
		const T& value, Distace*, random_access_iterator_tag)
	{
		Distace len = last - first;			//求取整个区间长度
		Distace half = 0;
		RandomAccessIterator mid;

		while (len > 0) {
			half = (len >> 1);		//通过移位实现除2
			mid = first + half;		//注意：随机指针才能这样移动

			if (*mid < value) {
				first = mid + 1;
				len = len - half - 1;
			}
			else {
				len = half;
			}
		}
		return first;
	}

	/* lower_bound 版本1 */
	template<class ForwarIterator, class T>
	inline ForwarIterator lower_bound(ForwarIterator first, ForwarIterator last, const T& value) {
		return __lower_bound(first, last, value, distance_type(first), iterator_category(first));
	}

	/* lower_bound 版本2 */
		/*template<class ForwarIterator, class T, class Compare>
		inline ForwarIterator lower_bound(ForwarIterator first, ForwarIterator last, const T& value, Compare comp) {
			return __lower_bound(first, last, value, comp, distance_type(first), iterator_category(first));
		}*/



		/* upper_bound（应用于有序区间） */
			//版本一辅助函数的 forward_iterator 版本
	template<class ForwarIterator, class T, class Distance>
	ForwarIterator __upper_bound(ForwarIterator first, ForwarIterator last,
		const T& value, Distance*, forward_iterator_tag)
	{
		Distance len = 0;
		distance(first, last, len);		//求得整个区间的长度
		Distance half = 0;
		ForwarIterator mid;

		while (len > 0) {
			half = (len >> 1);		//通过移位实现除2
			mid = first;
			advance(mid, half);		//令 mid 指向区间中部
			if (value < *mid) {
				len = half;
			}
			else {
				first = ++mid;
				len = len - half - 1;	//修正 half
			}
		}
		return first;
	}
	//版本一辅助函数的 random_access_iterator_tag 版本
	template<class	RandomAccessIterator, class T, class Distace>
	RandomAccessIterator __upper_bound(RandomAccessIterator first, RandomAccessIterator last,
		const T& value, Distace*, random_access_iterator_tag)
	{
		Distace len = last - first;			//求取整个区间长度
		Distace half = 0;
		RandomAccessIterator mid;

		while (len > 0) {
			half = (len >> 1);		//通过移位实现除2
			mid = first + half;		//注意：随机指针才能这样移动

			if (value < *mid) {
				len = half;
			}
			else {
				first = mid + 1;
				len = len - half - 1;
			}
		}
		return first;
	}

	/* upper_bound */
	template<class ForwarIterator, class T>
	inline ForwarIterator upper_bound(ForwarIterator first, ForwarIterator last, const T& value) {
		return __upper_bound(first, last, value, distance_type(first), iterator_category(first));
	}


	/* unary_function 用来呈现一元函数的参数行别和返回值型别, 每一个一元函数都应该继承此类别 */
	template<class Arg, class Result>
	struct unary_function {
		typedef Arg argument_type;
		typedef Result result_type;
	};

	/* binaty_function 用来呈现二元函数的第一参数型别、第二参数型别和返回值型别, 每一个二元函数都应该继承此类别 */
	template<class Arg1, class Arg2, class Result>
	struct binary_function {
		typedef Arg1 first_argument_type;
		typedef Arg2 second_argument_type;
		typedef Result result_type;
	};

	template<class T>
	struct negate : public unary_function<T, T> {
		T operator()(const T& x) const { return -x; }
	};

	/* + - * / % 的仿函数 */
	template<class T>
	struct plus : public binary_function<T, T, T> {
		T operator()(const T& x, const T& y) const { return x + y; }
	};
	template<class T>
	struct minus : public binary_function<T, T, T> {
		T operator()(const T& x, const T& y) const { return x - y; }
	};
	template<class T>
	struct multiplies : public binary_function<T, T, T> {
		T operator()(const T& x, const T& y) const { return x * y; }
	};
	template<class T>
	struct divides : public binary_function<T, T, T> {
		T operator()(const T& x, const T& y) const { return x / y; }
	};
	template<class T>
	struct modulus : public binary_function<T, T, T> {
		T operator()(const T& x, const T& y) const { return x % y; }
	};


	template<class T>
	struct less : public binary_function<T, T, bool> {
		bool operator()(const T& x, const T& y) const { return x < y; }
	};

	template<class T> 
	struct greater :public binary_function<T, T, bool> {
		bool operator()(const T& x, const T& y) const { return x > y; }
	};


	/* 证同(identity)、选择(select)、投射(project)函数 */
	template<class T>
	struct identity : public unary_function<T, T> {
		const T& operator()(const T& x) const { return x; }
	};

	template<class Pair>
	struct select1st : public unary_function<Pair, typename Pair::first_type> {
		const typename Pair::first_type& operator()(const Pair& x) const {
			return x.first;
		}
	};

	template<class Pair>
	struct select2nd : public unary_function<Pair, typename Pair::second_type> {
		const typename Pair::second_type& operator()(const Pair& x) const {
			return x.second;
		}
	};

	/* 投射函数：传回第一参数，忽略第二参数 */
	template<class Arg1, class Arg2>
	struct project1st : public binary_function<Arg1, Arg2, Arg1> {
		Arg1 operator()(const Arg1& x, const Arg2&) { return x; }
	};

	/* 投射函数：传回第二参数，忽略第一参数 */
	template<class Arg1, class Arg2>
	struct project2nd : public binary_function<Arg1, Arg2, Arg2> {
		Arg2 operator()(const Arg1&, const Arg2& y) { return y; }
	};

	template<class T>
	struct equal_to {
		bool operator()(const T& x, const T& y) const { return (x == y); }
	};

	/* 字符串判等 */
	struct str_equal {
		bool operator()(const char* s1, const char* s2) const {
			return strcmp(s1, s2) == 0;
		}
	};
__MYSTL_NAMESPACE_END_

#endif // !_ZSTL_ALGOBASE_H_
