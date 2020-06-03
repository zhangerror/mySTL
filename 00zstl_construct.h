#ifndef _ZSTL_CONSTRUCT_H_
#define _ZSTL_CONSTRUCT_H_

#include "00zstl_iterator.h"
#include "00zstl_algobase.h"
#include <new.h>	//placement new
//#include <memory>	//memmove���ڴ�ײ�������ٶȼ���
#include <string.h>

__MYSTL_NAMESPACE_BEGIN_
	template <class T1, class T2>
	inline void construct(T1* p, const T2& value) {
		new(p) T1(value);	//����T1::T1(value)
	}

	template <class T>
	inline void destroy(T* pointer) {
		pointer->~T();
	}

	//������destroy()�ڶ��汾�������������������˺����跨�ҳ�Ԫ�ص���ֵ�ͱ�
	//��������__type_traits<>��ȡ���ʵ���ʩ
	template<typename ForwardIterator, typename T>		//��������
	inline void __destroy(ForwardIterator, ForwardIterator, T*);

	//���Ԫ�ص���ֵ�ͱ���non-trivial destructor
	template <class ForwardIterator>
	inline void
		__destroy_aux(ForwardIterator first, ForwardIterator last, __false_type) {
		for (; first < last; ++first) {
			destroy(&*first);
		}
	}

	//���Ԫ�ص���ֵ�ͱ���trivial destructor
	template <class ForwardIterator>
	inline void __destroy_aux(ForwardIterator, ForwardIterator, __true_type) {}

	template <class ForwardIterator>
	inline void destroy(ForwardIterator first, ForwardIterator last) {
		__destroy(first, last, value_type(first));
	}

	//�ж�Ԫ�ص���ֵ�ͱ�(value type)�Ƿ���trival destructor
	template <class ForwardIterator, class T>
	inline void __destroy(ForwardIterator first, ForwardIterator last, T*) {
		typedef typename __type_traits<T>::has_trivial_destructor trivial_destructor;
		__destroy_aux(first, last, trivial_destructor());
	}

	//������destory()�ڶ��汾��Ե�����Ϊchar*��wchar_t*���ػ���
	inline void destroy(char*, char*) {}
	inline void destroy(wchar_t*, wchar_t*) {}

				/*	�ڴ����������	*/

	//uninitialized_copy����Ӧ�߲�copy()
	/*	������firstָ������˵���ʼλ��
		������lastָ������˵Ľ���λ��
		������resultָ�������(����ʼ���ռ�)����ʼ��
	*//*
	template <class InputIterator, class ForwardIterator>
	ForwardIterator
		uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result);*/

	//���copy construction ��ͬ��assignment������destructor��trivial��������Ч
	//�����POD�ͱ�ִ�����̾ͻ�ת�������¸�ʽ��������function template�Ĳ����Ƶ����ƶ���
	template <class InputIterator, class ForwardIterator>
	inline ForwardIterator
		__uninitialized_copy_aux(InputIterator first, InputIterator last, ForwardIterator result, __true_type) {
		return copy(first, last, result);	//����STL�㷨copy()
	}

	//�����non-POD�ͱ�ִ������ת�������º���������function template�Ĳ����Ƶ����ƶ���
	template <class InputIterator, class ForwardIterator>
	ForwardIterator
		__uninitialized_copy_aux(InputIterator first, InputIterator last, ForwardIterator result, __false_type) {
		ForwardIterator cur = result;
		try {
			for (; first != last; ++first, ++cur) {
				construct(&*cur, *first);		//����һ��һ���Ĺ��죬�޷���������
			}
		}
		catch (...) {	//�����쳣��[result,cur)�ڵ�Ԫ������ɹ��죬�������
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
		//���ϣ���ͼ����is_POD()����õĽ�����ñ������������Ƶ�
	}

	template <class InputIterator, class ForwardIterator>
	inline ForwardIterator
		uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result) {
		return __uninitialized_copy(first, last, result, value_type(result));	//����value_typeȡ��first��value type
	}

	//���const char* ���ػ��汾
	inline char* uninitialized_copy(const char* first, const char* last, char* result) {
		memmove(result, first, last - first);
		return result + (last - first);
	}

	//���const wchar_t* ���ػ��汾
	inline wchar_t* uninitialized_copy(const wchar_t* first, const wchar_t* last, wchar_t* result) {
		memmove(result, first, sizeof(wchar_t) * (last - first));
		return result + (last - first);
	}


	//uninitialized_fill����Ӧ�߲�fill()
	/*	������firstָ�������(����ʼ���ռ�)����ʼλ��
		������lastָ�������(����ʼ���ռ�)�Ľ���λ��
		x ��ʾ��ֵ
	*//*
	template <class ForwardIterator, class T>
	void uninitialized_fill(ForwardIterator first, ForwardIterator last, const T& x);*/

	//���copy construction��ͬ��assignment������destructor��trivial�����¾���Ч
	//�����POD�ͱ�ִ�����̾ͻ�ת�������º�����������functioin template�Ĳ����Ƶ����ƶ���
	template <class ForwardIterator, class T>
	inline void
		__uninitialized_fill_aux(ForwardIterator first, ForwardIterator last, const T& x, __true_type) {
		fill(first, last, x);	//����STL�㷨fill()
	}

	//�����non-POD�ͱ�ִ�����̾ͻ�ת�������º�����������function template�Ĳ����Ƶ����ƶ���
	template <class ForwardIterator, class T>
	void
		__uninitialized_fill_aux(ForwardIterator first, ForwardIterator last, const T& x, __false_type) {
		ForwardIterator cur = first;
		try {
			for (; cur != last; ++cur) {
				construct(&*cur, x);		//����һ��һ���Ĺ��죬�޷���������
			}
		}
		catch (...) {	//�����쳣��[first,cur)�ڵ�Ԫ������ɹ��죬�������
			ForwardIterator p = first;
			for (; p != cur; ++p) {
				destroy(&*p);
			}
			throw;
		}
	}

	//����ȡ��������first��value type��Ȼ���жϸ��ͱ��Ƿ�ΪPOD
	template <class ForwardIterator, class T, class T1>
	inline void __uninitialized_fill(ForwardIterator first, ForwardIterator last, const T& x, T1*) {
		typedef typename __type_traits<T1>::is_POD_type is_POD;
		__uninitialized_fill_aux(first, last, x, is_POD());
	}

	template <class ForwardIterator, class T>
	inline void uninitialized_fill(ForwardIterator first, ForwardIterator last, const T& x) {
		__uninitialized_fill(first, last, x, value_type(first));
	}

	//uninitialized_fill_n����Ӧ�߲�fill_n()
	/*	������firstָ������ʼ���ռ����ʼ��
		n��ʾ��ʼ���ռ�Ĵ�С
		x��ʾ��ֵ
	*//*
	template <class ForwardIterator, class Size, class T>
	ForwardIterator
		uninitialized_fill_n(ForwardIterator first, Size n, const T& x);
*/
	//���copy construction��ͬ��assignment������destructor��trivial�����¾���Ч
	//�����POD�ͱ�ִ�����̾ͻ�ת�������º�����������function template�Ĳ����Ƶ�����
	template <class ForwardIterator, class Size, class T>
	inline ForwardIterator
		__uninitialized_fill_n_aux(ForwardIterator first, Size n, const T& x, __true_type) {
		//���ɸ߽׺���ִ��
		return fill_n(first, n, x);
	}
	//�������POD�ͱ�ִ�����̾ͻ�ת�������º�����������function template�Ĳ����Ƶ����ƶ���
	template <class ForwardIterator, class Size, class T>
	inline ForwardIterator
		__uninitialized_fill_n_aux(ForwardIterator first, Size n, const T& x, __false_type) {
		ForwardIterator cur = first;
		try {
			for (; n > 0; --n, ++cur) {
				construct(&*cur, x);		//����һ��һ���Ĺ��죬�޷���������
			}
		}
		catch (...) {	//�����쳣��[first,cur)�ڵ�Ԫ������ɹ��죬�������
			ForwardIterator p = first;
			for (; p != cur; ++p) {
				destroy(&*p);
			}
			throw;
		}
	}

	template <class ForwardIterator, class Size, class T, class T1>
	inline ForwardIterator __uninitialized_fill_n(ForwardIterator first, Size n, const T& x, T1*) {
		//���� __type_traits<>����
		typedef typename __type_traits<T1>::is_POD_type is_POD;
		return __uninitialized_fill_n_aux(first, n, x, is_POD());
	}

	template <class ForwardIterator, class Size, class T>
	inline ForwardIterator uninitialized_fill_n(ForwardIterator first, Size n, const T& x) {
		return __uninitialized_fill_n(first, n, x, value_type(first));
		//���ϣ�����vaule_type()ȡ��first��value type
	}

__MYSTL_NAMESPACE_END_

#endif // !_ZSTL_CONSTRUCT_H_
