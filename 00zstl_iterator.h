#pragma once
#ifndef _ZSTL_ITERATOR_H_
#define _ZSTL_ITERATOR_H_

#include "00zstl_config.h"
#include <cstddef>	//ptrdiff_t

__MYSTL_NAMESPACE_BEGIN_
	//����������
	struct input_iterator_tag {};
	struct output_iterator_tag {};
	struct forward_iterator_tag : public input_iterator_tag {};
	struct bidirectional_iterator_tag : public forward_iterator_tag {};
	struct random_access_iterator_tag : public bidirectional_iterator_tag {};

	//Ϊ����д����ʱ��һ©�����п����ĵ�������ü̳����������std::iterator
	template <class Category, class T, class Distance = ptrdiff_t,
		class Pointer = T* , class Reference = T& >
	struct iterator {
		typedef Category	iterator_category;
		typedef T			value_type;
		typedef Distance	difference_type;
		typedef Pointer		pointer;
		typedef Reference	reference;
	};

	//"ե֭��" traits��������ȡ������������
	template <class Iterator>
	struct iterator_traits {
		typedef typename Iterator::iterator_category	iterator_category;
		typedef typename Iterator::value_type			value_type;
		typedef typename Iterator::difference_type		difference_type;
		typedef typename Iterator::pointer				pointer;
		typedef typename Iterator::reference			reference;
	};

	//���ԭ��ָ��(native pointer)����Ƶ�traitsƫ�ػ���
	template <class T>
	struct iterator_traits<T*> {
		typedef random_access_iterator_tag	iterator_category;
		typedef T							value_type;
		typedef ptrdiff_t					difference_type;
		typedef T*							pointer;
		typedef T&							reference;
	};

	//���ԭ�� pointer-to-const ����Ƶ� traits ƫ�ػ���
	template <class T>
	struct iterator_traits<const T*> {
		typedef random_access_iterator_tag	iterator_category;
		typedef T							value_type;
		typedef ptrdiff_t					difference_type;
		typedef const T*					pointer;
		typedef const T&					reference;
	};

	//����ĳ��������������(category)
	template <class Iterator>
	inline typename iterator_traits<Iterator>::iterator_category
		iterator_category(const Iterator&) {
		typedef typename iterator_traits<Iterator>::iterator_category category;
		return category();
	}

	//����ĳ���������� distance type
	template <class Iterator>
	inline typename iterator_traits<Iterator>::difference_type*
		distance_type(const Iterator&) {
		return static_cast<typename iterator_traits<Iterator>::difference_type*>(0);
	}

	//����ĳ���������� value type
	template <class Iterator>
	inline typename iterator_traits<Iterator>::value_type*
		value_type(const Iterator&) {
		return static_cast<typename iterator_traits<Iterator>::value_type*>(0);
	}

			/*	����Ϊ���� distance ����	*/
	template <class InputIterator>
	inline typename iterator_traits<InputIterator>::difference_type
		__distance(InputIterator first, InputIterator last,	input_iterator_tag) {
		typename iterator_traits<InputIterator>::difference_type n = 0;

		while (first != last) {
			++first;
			++n;
		}

		return n;
	}

	template <class RandomAccessIterator>
	inline typename iterator_traits<RandomAccessIterator>::difference_type
		__distance(RandomAccessIterator first, RandomAccessIterator last, random_access_iterator_tag) {
		return last - first;
	}

	template <class InputIterator>
	inline typename iterator_traits<InputIterator>::difference_type
		distance(InputIterator first, InputIterator last, input_iterator_tag) {
		typedef typename
			iterator_traits<InputIterator>::iterator_category category;

		return __distance(first, last, category());
	}

	/*	����Ϊ���� advance ����		*/
	template <class InputIterator, class Distance>
	inline void __advance(InputIterator& i, Distance n, input_iterator_tag) {
		while (n--) {
			++i;
		}
	}

	template <class BidirectionalIterator, class Distance>
	inline void __advance(BidirectionalIterator& i, Distance n, bidirectional_iterator_tag) {
		if (n >= 0) {
			while (n--) {	++i;}
		}
		else {
			while (n++) {	--i;}
		}
	}

	template <class RandomAccessIterator, class Distance>
	inline void __advance(RandomAccessIterator& i, Distance n, random_access_iterator_tag) {
		i += n;
	}

	template <class InputIterator, class Distance>
	inline void advance(InputIterator& i, Distance n) {
		__advance(i, n, iterator_category(i));
	}

				/*	SGI STL �����ͱ��Ƶ�	*/
	//������/�����ʵġ����󡯣�ʹ��������������Ӧ��������в����Ƶ�
	//��������ֻ�����class object��ʽ�Ĳ����Ż��������Ƶ���
	struct __true_type { };
	struct __false_type { };

	//�ͱ��������ȡ
	template <class type> 
	struct __type_traits {
		typedef __true_type this_dummy_member_must_be_first;
				/* ��Ҫ�Ƴ������Ա����֪ͨ"�������Զ��� __type����traits�ػ�" �ı�����˵��
				������������������� __type_traits template ������ġ�
				����Ϊ��ȷ����һ������Ҳʹ��һ����Ϊ __type_traits 
				����ʵ��˴����岢�޹�����templateʱ���������鶼�Խ�˳������
				*/
		/* ��������Ӧ�ñ����أ���Ϊ�������п����Զ�Ϊ���ͱ����ר���� __type_traits �ػ��汾��
			- ����������������µĳ�Ա����
			- ������Գ������κγ�Ա
			- �������Խ����³�Ա����������ȴû�иı�������еĶ�Ӧ����
			- �¼���ĳ�Ա�ᱻ��Ϊһ���Ա���������ڱ������м����ʵ���֧��
		*/

		typedef __false_type	has_trivial_default_constructor;
		typedef __false_type	has_trivial_copy_constructor;
		typedef __false_type	has_trivial_assignment_operator;
		typedef __false_type	has_trivial_destructor;
		typedef __false_type	is_POD_type;
	};

				/*	 <type_traits.h> ������C++�����ͱ�������� __type_traits�ػ��汾	*/
	/*�������C++�����ͱ� char, signed char, unsigned char, short, unsigned short, int, unsigned int, long, usigned long, float, double, long double
	�ṩ�ػ��汾��
	ע�⣬ÿһ����Ա��ֵ���� __true_type����ʾ��Щ�ͱ𶼿��Բ�������ٷ�ʽ(����memcpy)�����п�����ֵ����
	*/
	__STL_TEMPLATE_NULL struct __type_traits<char> {
		typedef __true_type	has_trivial_default_constructor;
		typedef __true_type	has_trivial_copy_constructor;
		typedef __true_type	has_trivial_assignment_operator;
		typedef __true_type	has_trivial_destructor;
		typedef __true_type	is_POD_type;
	};

	__STL_TEMPLATE_NULL struct __type_traits<signed char> {
		typedef __true_type	has_trivial_default_constructor;
		typedef __true_type	has_trivial_copy_constructor;
		typedef __true_type	has_trivial_assignment_operator;
		typedef __true_type	has_trivial_destructor;
		typedef __true_type	is_POD_type;
	};

	__STL_TEMPLATE_NULL struct __type_traits<unsigned char> {
		typedef __true_type	has_trivial_default_constructor;
		typedef __true_type	has_trivial_copy_constructor;
		typedef __true_type	has_trivial_assignment_operator;
		typedef __true_type	has_trivial_destructor;
		typedef __true_type	is_POD_type;
	};

	__STL_TEMPLATE_NULL struct __type_traits<short> {
		typedef __true_type	has_trivial_default_constructor;
		typedef __true_type	has_trivial_copy_constructor;
		typedef __true_type	has_trivial_assignment_operator;
		typedef __true_type	has_trivial_destructor;
		typedef __true_type	is_POD_type;
	};

	__STL_TEMPLATE_NULL struct __type_traits<unsigned short> {
		typedef __true_type	has_trivial_default_constructor;
		typedef __true_type	has_trivial_copy_constructor;
		typedef __true_type	has_trivial_assignment_operator;
		typedef __true_type	has_trivial_destructor;
		typedef __true_type	is_POD_type;
	};

	__STL_TEMPLATE_NULL struct __type_traits<int> {
		typedef __true_type	has_trivial_default_constructor;
		typedef __true_type	has_trivial_copy_constructor;
		typedef __true_type	has_trivial_assignment_operator;
		typedef __true_type	has_trivial_destructor;
		typedef __true_type	is_POD_type;
	};

	__STL_TEMPLATE_NULL struct __type_traits<unsigned int> {
		typedef __true_type	has_trivial_default_constructor;
		typedef __true_type	has_trivial_copy_constructor;
		typedef __true_type	has_trivial_assignment_operator;
		typedef __true_type	has_trivial_destructor;
		typedef __true_type	is_POD_type;
	};

	__STL_TEMPLATE_NULL struct __type_traits<long> {
		typedef __true_type	has_trivial_default_constructor;
		typedef __true_type	has_trivial_copy_constructor;
		typedef __true_type	has_trivial_assignment_operator;
		typedef __true_type	has_trivial_destructor;
		typedef __true_type	is_POD_type;
	};

	__STL_TEMPLATE_NULL struct __type_traits<unsigned long> {
		typedef __true_type	has_trivial_default_constructor;
		typedef __true_type	has_trivial_copy_constructor;
		typedef __true_type	has_trivial_assignment_operator;
		typedef __true_type	has_trivial_destructor;
		typedef __true_type	is_POD_type;
	};

	__STL_TEMPLATE_NULL struct __type_traits<float> {
		typedef __true_type	has_trivial_default_constructor;
		typedef __true_type	has_trivial_copy_constructor;
		typedef __true_type	has_trivial_assignment_operator;
		typedef __true_type	has_trivial_destructor;
		typedef __true_type	is_POD_type;
	};

	__STL_TEMPLATE_NULL struct __type_traits<double> {
		typedef __true_type	has_trivial_default_constructor;
		typedef __true_type	has_trivial_copy_constructor;
		typedef __true_type	has_trivial_assignment_operator;
		typedef __true_type	has_trivial_destructor;
		typedef __true_type	is_POD_type;
	};

	__STL_TEMPLATE_NULL struct __type_traits<long double> {
		typedef __true_type	has_trivial_default_constructor;
		typedef __true_type	has_trivial_copy_constructor;
		typedef __true_type	has_trivial_assignment_operator;
		typedef __true_type	has_trivial_destructor;
		typedef __true_type	is_POD_type;
	};

	//ע�⣬�������ԭ��ָ����� __type_traits ƫ�ػ��汾
	//ԭ��ָ��Ҳ����Ϊһ�ֱ����ͱ�
	template <class T>
	struct __type_traits<T*> {
		typedef __true_type	has_trivial_default_constructor;
		typedef __true_type	has_trivial_copy_constructor;
		typedef __true_type	has_trivial_assignment_operator;
		typedef __true_type	has_trivial_destructor;
		typedef __true_type	is_POD_type;
	};
__MYSTL_NAMESPACE_END_

#endif // _ZSTL_ITERATOR_H_
