#ifndef _ZSTL_STACK_H_
#define _ZSTL_STACK_H_

#include "03zstl_deque.h"

__MYSTL_NAMESPACE_BEGIN_

	template <class T, class Sequence = deque<T> >
	class stack {
	public:
		typedef typename Sequence::value_type		value_type;
		typedef typename Sequence::size_type		size_type;
		typedef typename Sequence::reference		reference;
		typedef typename Sequence::const_reference	const_reference;
	protected:
		Sequence c;		//�ײ�����
	public:
		stack():c() { }
		//������ȫ���� Sequence c �Ĳ�������� stack �Ĳ���
		bool empty() const { return c.empty(); }
		size_type size() const { return c.size(); }
		reference top() { return c.back(); }
		const_reference top() const { return c.back(); }
		// deque ����ͷ�ɽ����� stack��ĩ�˽���ĩ�˳�
		void push(const value_type& x) { c.push_back(x);  }
		void pop() { c.pop_back(); }

		bool operator== (const stack<T, Sequence>& x) { return c == x.c; }
		bool operator<  (const stack<T, Sequence>& x) { return c < x.c; }
	};

__MYSTL_NAMESPACE_END_

#endif // !_ZSTL_STACK_H_
