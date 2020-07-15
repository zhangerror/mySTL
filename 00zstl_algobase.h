#ifndef _ZSTL_ALGOBASE_H_
#define _ZSTL_ALGOBASE_H_

//�����㷨

#include <cstring>
#include "00zstl_config.h"
#include "00zstl_iterator.h"


__MYSTL_NAMESPACE_BEGIN_

	/* fill: ��[first, last) �ڵ�����Ԫ�ظ�����ֵ */
	template<class ForwardIterator, class T>
	void fill(ForwardIterator first, ForwardIterator last, const T& value) {
		for (; first != last; ++first) {	//�������߹���������
			*first = value;					//�趨��ֵ
		}
	}

	/*	fill_n: ���� first ��ʼ��ǰ n ��Ԫ�ظ�����ֵ�����صĵ�����ָ����������һ��Ԫ�ص���һλ�� */
	template<class OutputIterator, class Size, class T>
	OutputIterator fill_n(OutputIterator first, Size n, const T& value) {
		for (; n > 0; --n, ++first) {		//�������߹� n ��Ԫ��
			*first = value;					//�趨��ֵ
		}
		return first;
	}

	/* max�� min */
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

	/* mismatch������ƽ�бȽ��������У�ָ������֮��ĵ�һ����ƥ��㣬����һ�Ե��������ֱ�ָ���������еĲ�ƥ��� */
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


	/* copy: ��[first, last) �ڵ�Ԫ�ظ��Ƶ� result ��ʼ��λ�ã��������һ������Ԫ�ص���һλ�� */
	template<class InputIterator, class OutputIterator> struct __copy_dispatch;		//ģ��������

	/* ��ȫ�����汾 */
	template<class InputIterator, class OutputIterator>
	inline OutputIterator copy(InputIterator first, InputIterator last, OutputIterator result) {
		return __copy_dispatch<InputIterator, OutputIterator>()(first, last, result);
	}
	/* ��� const char* ��ȫ�ػ��汾 */
	inline char* copy(const char* first, const char* last, char* result) {
		memmove(result, first, last - first);
		return result + (last - first);
	}
	/* ��� const wchar_t* ��ȫ�ػ��汾 */
	inline wchar_t* copy(const wchar_t* first, const wchar_t* last, wchar_t* result) {
		memmove(result, first, sizeof(wchar_t) * (last - first));
		return result + (last - first);
	}

	/* __copy_dispatch ��ȫ�����汾 */
	template<class InputIterator, class OutputIterator>
	struct __copy_dispatch {
		OutputIterator operator()(InputIterator first, InputIterator last, OutputIterator result) {
			return __copy(first, last, result, iterator_category(first));
		}
	};
	/* __copy_dispatch ��� T* ��ƫ�ػ��汾 */
	template<class T>
	struct __copy_dispatch<T*, T*> {	//������������ T* ָ����ʽ
		T* operator()(T* first, T* last, T* result) {
			typedef typename __type_traits<T>::has_trivial_assignment_operator t;
			return __copy_t(first, last, result, t());
		}
	};
	/* __copy_dispatch ��� const T* ��ƫ�ػ��汾 */
	template<class T>
	struct __copy_dispatch<const T*, T*> {	//��һ������Ϊ const T*���ڶ�������Ϊ T* ��ʽ
		T* operator()(const T* first, const T* last, T* result) {
			typedef typename __type_traits<T>::has_trivial_assignment_operator t;
			return __copy_t(first, last, result, t());
		}
	};

	/* __copy_dispatch() ����ȫ�����汾���ݵ���������Ĳ�ͬ�����˲�ͬ�� __copy()��Ϊ���ǲ�ͬ����ĵ�����ʹ�õ�ѭ��������ͬ���п���֮�֡� */
	/* InputIterator �汾 */
	template<class InputIterator, class OutputIterator>
	inline OutputIterator __copy(InputIterator first, InputIterator last, OutputIterator result, input_iterator_tag) {
		//�Ե������Ƿ���Ⱦ����Ƿ����ѭ�����ٶ���
		for (; first != last; ++first, ++result)
			*result = *first;
		return result;
	}
	/* RandomAccessIterator �汾 */
	template<class RandomAccessIterator, class OutputIterator>
	inline OutputIterator __copy(RandomAccessIterator first, RandomAccessIterator last, OutputIterator result, random_access_iterator_tag) {
		//�ֻ��ֳ�һ��������Ϊ���������ط�Ҳ���õ�
		return __copy_d(first, last, result, distance_type(first));
	}

	template<class RandomAccessIterator, class OutputIterator, class Distance>
	inline OutputIterator __copy_d(RandomAccessIterator first, RandomAccessIterator last, OutputIterator result, Distance*) {
		//�� n ����ѭ��ִ�д������ٶȿ�
		Distance n = last - first;
		for (; n > 0; --n, ++first, ++result)
			*result = *first;
		return result;
	}

	//���°汾�ʺ���ָ����ָ����߱� trivial assignment operator
	template<class T>
	inline T* __copy_t(const T* first, const T* last, T* result, __true_type) {
		memmove(result, first, sizeof(T) * (last - first));
		return result + (last - first);
	}

	//���°汾�ʺ���ָ����ָ����߱� non-trivial assignment operator
	template<class T>
	inline T* __copy_t(const T* first, const T* last, T* result, __false_type) {
		return __copy_d(first, last, result, (ptrdiff_t*)0);
	}


	/*	�� [first, last) �ڵ�Ԫ�شӺ���ǰ�ƶ��� result ��ǰ��λ��(result���洢Ԫ��)	*/
		//ע�⣺ֻ��˫����������ܵ��øú���
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

	/* lower_bound��Ӧ�����������䣩 */
		//�汾һ���������� forward_iterator �汾
	template<class ForwarIterator, class T, class Distance>
	ForwarIterator __lower_bound(ForwarIterator first, ForwarIterator last,
		const T& value, Distance*, forward_iterator_tag)
	{
		Distance len = 0;
		distance(first, last, len);		//�����������ĳ���
		Distance half = 0;
		ForwarIterator mid;

		while (len > 0) {
			half = (len >> 1);		//ͨ����λʵ�ֳ�2
			mid = first;
			advance(mid, half);		//�� mid ָ�������в�
			if (*mid < value) {
				first = ++mid;
				len = len - half - 1;	//���� half
			}
			else {
				len = half;
			}
		}
		return first;
	}
	//�汾һ���������� random_access_iterator_tag �汾
	template<class	RandomAccessIterator, class T, class Distace>
	RandomAccessIterator __lower_bound(RandomAccessIterator first, RandomAccessIterator last,
		const T& value, Distace*, random_access_iterator_tag)
	{
		Distace len = last - first;			//��ȡ�������䳤��
		Distace half = 0;
		RandomAccessIterator mid;

		while (len > 0) {
			half = (len >> 1);		//ͨ����λʵ�ֳ�2
			mid = first + half;		//ע�⣺���ָ����������ƶ�

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

	/* lower_bound �汾1 */
	template<class ForwarIterator, class T>
	inline ForwarIterator lower_bound(ForwarIterator first, ForwarIterator last, const T& value) {
		return __lower_bound(first, last, value, distance_type(first), iterator_category(first));
	}

	/* lower_bound �汾2 */
		/*template<class ForwarIterator, class T, class Compare>
		inline ForwarIterator lower_bound(ForwarIterator first, ForwarIterator last, const T& value, Compare comp) {
			return __lower_bound(first, last, value, comp, distance_type(first), iterator_category(first));
		}*/



		/* upper_bound��Ӧ�����������䣩 */
			//�汾һ���������� forward_iterator �汾
	template<class ForwarIterator, class T, class Distance>
	ForwarIterator __upper_bound(ForwarIterator first, ForwarIterator last,
		const T& value, Distance*, forward_iterator_tag)
	{
		Distance len = 0;
		distance(first, last, len);		//�����������ĳ���
		Distance half = 0;
		ForwarIterator mid;

		while (len > 0) {
			half = (len >> 1);		//ͨ����λʵ�ֳ�2
			mid = first;
			advance(mid, half);		//�� mid ָ�������в�
			if (value < *mid) {
				len = half;
			}
			else {
				first = ++mid;
				len = len - half - 1;	//���� half
			}
		}
		return first;
	}
	//�汾һ���������� random_access_iterator_tag �汾
	template<class	RandomAccessIterator, class T, class Distace>
	RandomAccessIterator __upper_bound(RandomAccessIterator first, RandomAccessIterator last,
		const T& value, Distace*, random_access_iterator_tag)
	{
		Distace len = last - first;			//��ȡ�������䳤��
		Distace half = 0;
		RandomAccessIterator mid;

		while (len > 0) {
			half = (len >> 1);		//ͨ����λʵ�ֳ�2
			mid = first + half;		//ע�⣺���ָ����������ƶ�

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


	/* unary_function ��������һԪ�����Ĳ����б�ͷ���ֵ�ͱ�, ÿһ��һԪ������Ӧ�ü̳д���� */
	template<class Arg, class Result>
	struct unary_function {
		typedef Arg argument_type;
		typedef Result result_type;
	};

	/* binaty_function �������ֶ�Ԫ�����ĵ�һ�����ͱ𡢵ڶ������ͱ�ͷ���ֵ�ͱ�, ÿһ����Ԫ������Ӧ�ü̳д���� */
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

	/* + - * / % �ķº��� */
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


	/* ֤ͬ(identity)��ѡ��(select)��Ͷ��(project)���� */
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

	/* Ͷ�亯�������ص�һ���������Եڶ����� */
	template<class Arg1, class Arg2>
	struct project1st : public binary_function<Arg1, Arg2, Arg1> {
		Arg1 operator()(const Arg1& x, const Arg2&) { return x; }
	};

	/* Ͷ�亯�������صڶ����������Ե�һ���� */
	template<class Arg1, class Arg2>
	struct project2nd : public binary_function<Arg1, Arg2, Arg2> {
		Arg2 operator()(const Arg1&, const Arg2& y) { return y; }
	};

	template<class T>
	struct equal_to {
		bool operator()(const T& x, const T& y) const { return (x == y); }
	};

	/* �ַ����е� */
	struct str_equal {
		bool operator()(const char* s1, const char* s2) const {
			return strcmp(s1, s2) == 0;
		}
	};
__MYSTL_NAMESPACE_END_

#endif // !_ZSTL_ALGOBASE_H_
