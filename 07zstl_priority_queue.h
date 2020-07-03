#ifndef _ZSTL_PRIORITY_QUEUE_H_
#define _ZSTL_PRIORITY_QUEUE_H_

#include "06zstl_heap_fix.h"
#include "00zstl_algobase.h"

__MYSTL_NAMESPACE_BEGIN_
	template <class T, class Sequence = heap<T> >
	class priority_queue {
	public:
		typedef typename Sequence::value_type		value_type;
		typedef typename Sequence::size_type		size_type;
		typedef typename Sequence::reference		reference;

	protected:
		Sequence c;		//µ×²ãÈÝÆ÷

	public:
		priority_queue() : c() { }
		priority_queue(const Sequence & new_c) : c(new_c) { }

		bool empty() const { return c.empty(); }
		size_type size() const { return c.size(); }
		reference front() { return c.top(); }
		void push(const value_type& x) {
			try {
				c.push(x);
			}
			catch (...) {
				c.m_c.clear();
				throw;
			}
		}

		void pop() {
			try {
				c.pop();
			}
			catch (...) {
				c.m_c.clear();
				throw;
			}
		}
	};
__MYSTL_NAMESPACE_END_

#endif // !_ZSTL_PRIORITY_QUEUE_H_
