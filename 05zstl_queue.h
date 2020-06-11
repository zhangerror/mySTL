#ifndef _ZSTL_QUEUE_H_
#define _ZSTL_QUEUE_H_

#include "03zstl_deque.h"

__MYSTL_NAMESPACE_BEGIN_
	template<class T, class Sequence = deque<T>>
	class queue {
	public:
		typedef typename Sequence::value_type		value_type;
		typedef typename Sequence::size_type		size_type;
		typedef typename Sequence::reference		reference;
		typedef typename Sequence::const_reference	const_reference;
	protected:
		Sequence c;		//�ײ�����
	public:
		//������ȫ���� Sequence c �Ĳ�������� queue �Ĳ���
		bool empty() const { return c.empty(); }
		size_type size() const { return c.size(); }
		reference front() { return c.front(); }
		const_reference front() const { return c.front(); }
		reference back() { return c.back(); }
		const_reference back() const { return c.back(); }
		//deque ����ͷ�ɽ����� queue��ĩ�˽���ǰ�˳�
		void push(const value_type& x) { c.push_back(x); }
		void pop() { c.pop_front(); }

		bool operator== (const queue<T, Sequence>& x) { return c == x.c; }
		bool operator< (const queue<T, Sequence>& x) { return c < x.c; }
	};

__MYSTL_NAMESPACE_END_

#endif // !_ZSTL_QUEUE_H_
